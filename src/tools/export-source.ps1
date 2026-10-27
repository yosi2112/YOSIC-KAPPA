param([Parameter(Mandatory = $true)][string]$Destination)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$destinationPath = [IO.Path]::GetFullPath($Destination)
if (Test-Path -LiteralPath $destinationPath) { throw 'Choose a new export directory.' }
foreach ($inputDirectory in @('src', 'doc')) {
    $inputPath = [IO.Path]::GetFullPath((Join-Path $root $inputDirectory))
    if ($destinationPath.Equals($inputPath, [StringComparison]::OrdinalIgnoreCase) -or
        $destinationPath.StartsWith($inputPath + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Export directory must be outside src/doc.'
    }
}
$paths = @(& git -C $root -c core.quotepath=false ls-files)
if ($LASTEXITCODE -ne 0) { throw 'Source export requires a Git working tree.' }
New-Item -ItemType Directory -Path $destinationPath | Out-Null
foreach ($relative in $paths) {
    if ($relative -notmatch '^(doc/|src/|readme\.md$|license$)') { continue }
    if ($relative -match '(^|/)(\.git|\.codex|\.agents|__pycache__|CMakeFiles|CMakeCache\.txt|build-tools\.md|Agents\.md|plan|resume|out|PLDULL|tests?|local-tests)(/|$)' -or
        $relative -match '\.(exe|obj|pdb|lib|zip|pyc|bin|rom|img|d88|kfd|yke|map|lst)$' -or
        ($relative -match '^src/' -and $relative -match '\.md$')) { continue }
    $source = Join-Path $root $relative
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) { continue }
    $target = Join-Path $destinationPath $relative
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
    Copy-Item -LiteralPath $source -Destination $target
}
& (Join-Path $PSScriptRoot 'check-public-tree.ps1') -Root $destinationPath
