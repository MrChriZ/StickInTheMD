# Full Windows release: tests, installer, release notes, optional GitHub publish.
param(
    [string]$Tag = "",
    [switch]$SkipTests,
    [switch]$SkipBuild,
    [switch]$Publish
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$RunTests = Join-Path $Root "scripts\run-tests.ps1"
$BuildInstaller = Join-Path $Root "scripts\build-windows-installer.ps1"
$GenNotes = Join-Path $Root "scripts\generate-release-notes.ps1"

function Get-ProjectVersion {
    $cmake = Join-Path $Root "CMakeLists.txt"
    $line = Select-String -Path $cmake -Pattern 'project\s*\(\s*StuckInTheMD\s+VERSION\s+([0-9.]+)' |
        Select-Object -First 1
    if ($line -and $line.Matches.Count -gt 0) { return $line.Matches[0].Groups[1].Value }
    return "1.0.0"
}

$Version = Get-ProjectVersion
if (-not $Tag) { $Tag = "v$Version" }
if ($Tag -notmatch '^v') { $Tag = "v$Tag" }

if (-not $SkipTests) {
    Write-Host "Running tests..."
    & $RunTests
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

$buildArgs = @()
if ($SkipBuild) { $buildArgs += "-SkipBuild" }
Write-Host "Building installer and release notes..."
& $BuildInstaller @buildArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$Installer = Join-Path $Root "dist\StuckInTheMD-$Version-setup.exe"
$Notes = Join-Path $Root "dist\release-notes-v$Version.md"
if (-not (Test-Path $Installer)) {
    throw "Missing installer: $Installer"
}
if (-not (Test-Path $Notes)) {
    throw "Missing release notes: $Notes"
}

Write-Host "Ready: $Installer"
Write-Host "Notes:  $Notes"

if ($Publish) {
    $gh = Get-Command gh -ErrorAction SilentlyContinue
    if (-not $gh) { throw "gh CLI not found; install GitHub CLI or omit -Publish." }
    Write-Host "Publishing GitHub release $Tag..."
    $existing = & gh release view $Tag --repo MrChriZ/StuckInTheMD 2>$null
    if ($LASTEXITCODE -eq 0) {
        & gh release upload $Tag $Installer --repo MrChriZ/StuckInTheMD --clobber
        & gh release edit $Tag --repo MrChriZ/StuckInTheMD --notes-file $Notes
    } else {
        & gh release create $Tag $Installer --repo MrChriZ/StuckInTheMD --title $Tag --notes-file $Notes
    }
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Write-Host "Published: https://github.com/MrChriZ/StuckInTheMD/releases/tag/$Tag"
} else {
    Write-Host "To publish: git tag $Tag && git push origin $Tag"
    Write-Host "Or: .\scripts\publish-windows-release.ps1 -Publish"
}
