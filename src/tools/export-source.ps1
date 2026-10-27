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
    $source = Join-Path $root $name
    $files = if (Test-Path -LiteralPath $source -PathType Container) {
        Get-ChildItem -LiteralPath $source -Recurse -File -Force
    } else { Get-Item -LiteralPath $source }
    foreach ($file in $files) {
        $relative = $file.FullName.Substring($root.Length + 1).Replace('\', '/')
        if ($relative -match '(^|/)(\.git|\.codex|\.agents|__pycache__|CMakeFiles|CMakeCache\.txt|build-tools\.md|Agents\.md|plan|resume|out|PLDULL|tests?|local-tests)(/|$)' -or
            $relative -match '\.(exe|obj|pdb|lib|zip|pyc|bin|rom|img|d88|kfd|yke|map|lst)$' -or
            ($relative -match '^src/' -and $relative -match '\.md$')) { continue }
        $target = Join-Path $destinationPath $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $target
    }
}
& (Join-Path $PSScriptRoot 'check-public-tree.ps1') -Root $destinationPath
