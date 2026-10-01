$itemsToRemove = @(
    'C:\Ethos\allos\avm\kernel\frontends',
    'C:\Ethos\axos\avm\modules\axos_compiler\compiler\frontends',
    'C:\Ethos\axos\avm\modules\axos_compiler\compiler\backends',
    'C:\Ethos\axos\avm\bin\src\compiler',
    'C:\Ethos\axos\avm\modules\axos_compiler\compiler\main.cpp',
    'C:\Ethos\axos\avm\modules\axos_compiler\pratt.axos',
    'C:\Ethos\allos\avm\kernel\windows_native\sys_audio.hpp',
    'C:\Ethos\allos\avm\kernel\windows_native\sys_graphics_3d.hpp',
    'C:\Ethos\allos\avm\kernel\windows_native\sys_graphics.hpp',
    'C:\Ethos\allos\avm\kernel\windows_native\sys_network.hpp',
    'C:\Ethos\allos\avm\kernel\windows_native\sys_speech.hpp',
    'C:\Ethos\allos\avm\kernel\pipelines\python_to_c.exe',
    'C:\Ethos\allos\avm\kernel\pipelines\python_to_c.exe.c',
    'C:\Ethos\allos\avm\kernel\pipelines\tests\python_form_axi_test.exe',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\cloudrun_mcp.axos',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\cloudrun_mcp.axos.exe',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\cloudrun_mcp.axos.exe.c',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\genkit_mcp.axos',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\genkit_mcp.axos.exe',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\genkit_mcp.axos.exe.c',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\sequential_thinking_mcp.axos',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\sequential_thinking_mcp.axos.exe',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\sequential_thinking_mcp.axos.exe.c',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\webtools_mcp.axos',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\webtools_mcp.axos.exe',
    'C:\Ethos\axos\avm\modules\axos_compiler\ipc\webtools_mcp.axos.exe.c',
    'C:\Ethos\axos\avm\tests\test_hardware_backends.axos',
    'C:\Ethos\axos\avm\tests\test_universal_transpiler.axos',
    'C:\Ethos\axos\avm\tests\test_frontends.py',
    'C:\Ethos\axos\avm\tests\test_ast_boundary_bin.exe',
    'C:\Ethos\axos\avm\tests\test_ast_boundary_bin.exe.c',
    'C:\Ethos\axos\avm\tests\test_universal_transpiler_v3.0.0_bin.exe.c'
)

$deletedCount = 0
foreach ($path in $itemsToRemove) {
    if (Test-Path $path) {
        Remove-Item -Path $path -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "Deleted: $path" -ForegroundColor Yellow
        $deletedCount++
    }
}
Write-Host "Total items removed: $deletedCount" -ForegroundColor Green
