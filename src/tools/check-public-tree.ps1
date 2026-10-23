param(
    [Parameter(Mandatory = $true)][string]$Root,
    [string]$GitRef = ''
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$Root = (Resolve-Path -LiteralPath $Root).Path
$allowed = @('doc', 'src', 'readme.md', 'license')
if ($GitRef) {
    $paths = @(& git -C $Root ls-tree -r --name-only $GitRef)
    if ($LASTEXITCODE -ne 0) { throw 'Cannot inspect the Git tree.' }
    $top = @($paths | ForEach-Object { ($_ -split '/')[0] } | Sort-Object -Unique)
} else {
    $top = @(Get-ChildItem -LiteralPath $Root -Force | Select-Object -ExpandProperty Name | Sort-Object)
    $paths = @(Get-ChildItem -LiteralPath $Root -Recurse -Force -File | ForEach-Object {
        $_.FullName.Substring($Root.Length + 1).Replace('\', '/')
    })
}
if (($top -join '|') -cne (($allowed | Sort-Object) -join '|')) {
    throw "Public root must contain exactly doc/src/readme.md/license; found: $($top -join ', ')"
}
foreach ($path in $paths) {
    if ($path -match '(^|/)(\.git|\.codex|\.agents|__pycache__|CMakeFiles|CMakeCache\.txt|build-tools\.md|Agents\.md)(/|$)' -or
        $path -match '(^|/)(tests?|local-tests)(/|$)' -or
        $path -match '\.(exe|obj|pdb|lib|zip|pyc)$' -or
        ($path -match '^src/' -and $path -match '\.md$')) {
        throw "Private, generated or misplaced documentation file: $path"
    }
}
Write-Host "Public tree passed: $($paths.Count) files; doc/src/readme.md/license only."
