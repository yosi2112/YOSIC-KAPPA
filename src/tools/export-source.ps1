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
New-Item -ItemType Directory -Path $destinationPath | Out-Null
foreach ($name in @('doc', 'src', 'readme.md', 'license')) {
    Copy-Item -LiteralPath (Join-Path $root $name) -Destination $destinationPath -Recurse
}
& (Join-Path $PSScriptRoot 'check-public-tree.ps1') -Root $destinationPath
