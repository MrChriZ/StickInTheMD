# Build StickInTheMD Release and compile the Windows installer (Inno Setup 6).
param(
    [string]$Configuration = "Release",
    [string]$BuildDir = "",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
if (-not $BuildDir) {
    $BuildDir = Join-Path $Root "build"
}

function Get-ProjectVersion {
    $cmake = Join-Path $Root "CMakeLists.txt"
    if (-not (Test-Path $cmake)) { return "1.0.0" }
    $line = Select-String -Path $cmake -Pattern 'project\s*\(\s*StickInTheMD\s+VERSION\s+([0-9.]+)' |
        Select-Object -First 1
    if ($line -and $line.Matches.Count -gt 0) {
        return $line.Matches[0].Groups[1].Value
    }
    return "1.0.0"
}

function Find-InnoSetupCompiler {
    $candidates = @(
        "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe",
        "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
        "$env:ProgramFiles\Inno Setup 6\ISCC.exe"
    )
    foreach ($path in $candidates) {
        if (Test-Path $path) { return $path }
    }
    return $null
}

$Version = Get-ProjectVersion
$SourceDir = Join-Path $BuildDir $Configuration
$ExePath = Join-Path $SourceDir "stickinthemd.exe"

if (-not $SkipBuild) {
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        throw "cmake not found on PATH. Install CMake or Visual Studio Build Tools."
    }
    Write-Host "Configuring CMake..."
    & cmake -S $Root -B $BuildDir -G "Visual Studio 17 2022" -A x64
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Write-Host "Building stickinthemd ($Configuration)..."
    & cmake --build $BuildDir --config $Configuration --target stickinthemd
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

if (-not (Test-Path $ExePath)) {
    throw "Missing $ExePath. Build the application first."
}

$Iscc = Find-InnoSetupCompiler
if (-not $Iscc) {
    throw "Inno Setup 6 not found. Install from https://jrsoftware.org/isinfo.php then re-run with -SkipBuild."
}

$Dist = Join-Path $Root "dist"
New-Item -ItemType Directory -Force -Path $Dist | Out-Null

Write-Host "Compiling installer (version $Version)..."
$IssDir = Join-Path $Root "packaging\windows"
Push-Location $IssDir
try {
    & $Iscc "stickinthemd.iss" `
        "/DMyAppVersion=$Version" `
        "/DMyAppSourceDir=$SourceDir"
} finally {
    Pop-Location
}
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$Installer = Get-ChildItem -Path $Dist -Filter "StickInTheMD-$Version-setup.exe" | Select-Object -First 1
if ($Installer) {
    $mb = [math]::Round($Installer.Length / 1MB, 2)
    Write-Host "Created: $($Installer.FullName) ($mb MB)"
} else {
    Write-Host "Installer built in $Dist"
}
