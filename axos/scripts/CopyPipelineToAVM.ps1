$srcDir = 'C:\Ethos\allos\allos-php\tools\proprietary\Transpiler_Engine'
$dstDir = 'C:\Ethos\allos\avm\kernel\pipeline'
if (-not (Test-Path $dstDir)) { 
    New-Item -ItemType Directory -Path $dstDir -Force | Out-Null
}

$sourceFiles = @(
    'allos_lexer.h', 'allos_lexer.cpp',
    'allos_parser.h', 'allos_parser.cpp',
    'allos_analyzer.h', 'allos_analyzer.cpp',
    'allos_emitter.h', 'allos_emitter.cpp',
    'allos_AST.h', 'allos_AST.cpp',
    'allos_compiler.c',
    'AST.h', 'AST.cpp',
    'lexer.h', 'lexer.cpp',
    'parser.h', 'parser.cpp',
    'analyzer.h', 'analyzer.cpp',
    'emitter.h', 'emitter.cpp',
    'c_api.h', 'c_api.cpp',
    'dvcs.h', 'dvcs.cpp',
    'eros_exponent_core.hpp',
    'sha3.hpp',
    'ast.hpp'
)

$copiedCount = 0
foreach ($fileName in $sourceFiles) {
    $srcPath = Join-Path $srcDir $fileName
    if (Test-Path $srcPath) {
        Copy-Item -Path $srcPath -Destination $dstDir -Force
        $copiedCount++
    }
}
Write-Host "Copied $copiedCount pipeline source files into $dstDir"
