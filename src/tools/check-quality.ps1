param(
    [string]$ClangFormatPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Invoke-CheckedTool {
    param([string]$ToolPath, [string[]]$ToolArguments)
    & $ToolPath @ToolArguments
    if ($LASTEXITCODE -ne 0) {
        throw "$ToolPath failed with exit code $LASTEXITCODE"
    }
}

function Find-ClangFormat {
    $onPath = Get-Command clang-format.exe -CommandType Application -ErrorAction SilentlyContinue
    if ($onPath) {
        return $onPath.Source
    }

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (Test-Path -LiteralPath $vswhere -PathType Leaf) {
        $installations = @(& $vswhere -products '*' -version '[18.0,19.0)' -property installationPath)
        if ($LASTEXITCODE -ne 0) {
            throw 'vswhere failed while looking for clang-format.'
        }
        foreach ($installation in $installations) {
            foreach ($relative in @('VC/Tools/Llvm/x64/bin/clang-format.exe', 'VC/Tools/Llvm/bin/clang-format.exe')) {
                $candidate = Join-Path $installation $relative
                if (Test-Path -LiteralPath $candidate -PathType Leaf) {
                    return $candidate
                }
            }
        }
    }
    throw 'clang-format not found. Install VS18 LLVM tools or use -ClangFormatPath.'
}

try {
    $projectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
    foreach ($requiredTool in @('cl.exe', 'cmake.exe', 'ninja.exe')) {
        if (-not (Get-Command $requiredTool -CommandType Application -ErrorAction SilentlyContinue)) {
            throw "$requiredTool not found. Run in the VS18 x64 Developer Command Prompt."
        }
    }
    $codePage = & "$env:SystemRoot/System32/chcp.com"
    if ($LASTEXITCODE -ne 0 -or $codePage -notmatch '\b65001\b') {
        throw 'Run chcp 65001 in the Developer Command Prompt before configuring or building.'
    }

    if ([string]::IsNullOrWhiteSpace($ClangFormatPath)) {
        $ClangFormatPath = Find-ClangFormat
    }
    if (-not (Test-Path -LiteralPath $ClangFormatPath -PathType Leaf)) {
        throw "clang-format does not exist: $ClangFormatPath"
    }
    $ClangFormatPath = (Resolve-Path -LiteralPath $ClangFormatPath).Path

    $sourceFiles = @(foreach ($sourceRoot in @('src')) {
        $sourceDirectory = Join-Path $projectRoot $sourceRoot
        if (Test-Path -LiteralPath $sourceDirectory -PathType Container) {
            Get-ChildItem -LiteralPath $sourceDirectory -Recurse -File |
                Where-Object { $_.Extension -in @('.cpp', '.h', '.hpp') }
        }
    })
    if ($sourceFiles.Count -eq 0) {
        throw 'No C++ source files found; refusing an empty quality check.'
    }

    Invoke-CheckedTool -ToolPath $ClangFormatPath -ToolArguments @('--version')
    foreach ($sourceFile in $sourceFiles) {
        Invoke-CheckedTool -ToolPath $ClangFormatPath -ToolArguments @('--dry-run', '--Werror', $sourceFile.FullName)
    }
    Write-Host "Formatting passed: $($sourceFiles.Count) files."

    $analysisDirectory = Join-Path $projectRoot 'out/analyze'
    Invoke-CheckedTool -ToolPath 'cmake.exe' -ToolArguments @(
        '--fresh', '-S', (Join-Path $projectRoot 'src'), '-B', $analysisDirectory, '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Debug', '-DYOSIC_ENABLE_ANALYSIS=ON'
    )
    Invoke-CheckedTool -ToolPath 'cmake.exe' -ToolArguments @('--build', $analysisDirectory, '--clean-first')
    Write-Host 'Quality checks passed: formatting and MSVC static analysis.'
    exit 0
} catch {
    Write-Error -Message $_.Exception.Message -ErrorAction Continue
    exit 1
}


