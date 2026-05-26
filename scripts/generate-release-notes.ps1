# Extract release notes for a version from RELEASE_NOTES.md into dist/release-notes-vX.Y.Z.md
param(
    [string]$Version = "",
    [string]$Output = ""
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)

function Get-ProjectVersion {
    $cmake = Join-Path $Root "CMakeLists.txt"
    if (-not (Test-Path $cmake)) { return "1.0.0" }
    $line = Select-String -Path $cmake -Pattern 'project\s*\(\s*StuckInTheMD\s+VERSION\s+([0-9.]+)' |
        Select-Object -First 1
    if ($line -and $line.Matches.Count -gt 0) {
        return $line.Matches[0].Groups[1].Value
    }
    return "1.0.0"
}

if (-not $Version) {
    $Version = Get-ProjectVersion
}

$Source = Join-Path $Root "RELEASE_NOTES.md"
if (-not (Test-Path $Source)) {
    throw "Missing $Source. Add a ## v$Version section before building a release."
}

$Dist = Join-Path $Root "dist"
New-Item -ItemType Directory -Force -Path $Dist | Out-Null

if (-not $Output) {
    $Output = Join-Path $Dist "release-notes-v$Version.md"
}

$text = Get-Content -Path $Source -Raw
$header = "## v$Version"
$start = $text.IndexOf($header)
if ($start -lt 0) {
    throw "No release notes section '$header' in RELEASE_NOTES.md. Add it before building v$Version."
}

$bodyStart = $start + $header.Length
$next = [regex]::Match($text.Substring($bodyStart), '(?m)^## v[0-9]+\.[0-9]+\.[0-9]+\s*$')
$bodyEnd = if ($next.Success) { $bodyStart + $next.Index } else { $text.Length }
$sectionBody = $text.Substring($bodyStart, $bodyEnd - $bodyStart).Trim()

if (-not $sectionBody) {
    throw "Release notes section '$header' is empty. Add bullet points in RELEASE_NOTES.md."
}

$markdown = @"
# StuckInTheMD v$Version

$sectionBody

## Install

Download **StuckInTheMD-$Version-setup.exe** from the assets below and run the installer.
"@

[System.IO.File]::WriteAllText($Output, $markdown.TrimEnd() + "`n", [System.Text.UTF8Encoding]::new($false))
Write-Host "Wrote release notes: $Output"
