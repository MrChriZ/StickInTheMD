# Run the same configure / build / test steps as CI (Windows).
param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $Root

if (-not (Test-Path "third_party/md4c/.git")) {
    bash scripts/bootstrap-third-party.sh
}

$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    $vsCmake = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    if (Test-Path $vsCmake) { $cmake = $vsCmake }
}
if (-not $cmake) {
    throw "cmake not found on PATH."
}

& $cmake -S . -B build -DCMAKE_BUILD_TYPE=$Configuration -DSTUCKINTHEMD_BUILD_TESTS=ON
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $cmake --build build --config $Configuration
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $cmake --build build --config $Configuration --target stuckinthemd_tests
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$testExe = Join-Path $Root "build/tests/$Configuration/stuckinthemd_tests.exe"
if (-not (Test-Path $testExe)) {
    throw "Missing $testExe"
}
& $testExe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "All tests passed."
