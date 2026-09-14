// Prevents additional console window on Windows in release
//#![cfg_attr(
//    all(not(debug_assertions), target_os = "windows"),
//    windows_subsystem = "windows"
//)]

use tauri::command;
use std::fs;
use libloading::{Library, Symbol};

// Hook into the axi_compiler.dll C-API we built
type CompileFunc = unsafe extern "C" fn(*const u16, *const u16) -> i32;

#[command]
fn compile_axi_code(source_code: String) -> Result<String, String> {
    // 1. Write the editor contents to a temporary file
    let input_path = "temp_workspace.axi";
    let output_path = "temp_workspace.exe";
    
    fs::write(input_path, source_code).map_err(|e| e.to_string())?;

    unsafe {
        let mut dll_path = std::env::current_exe().map_err(|e| e.to_string())?;
        dll_path.pop(); // remove executable name
        dll_path.pop(); // remove 'apps' or 'release'
        // If we are in target/release, we need to go up two more levels to hit ethos-public
        if dll_path.ends_with("target") {
            dll_path.pop(); // target
            dll_path.pop(); // src-tauri
            dll_path.pop(); // axi_ide
        }
        dll_path.push("components");
        dll_path.push("axi_compiler.dll");

        let lib = Library::new(&dll_path)
            .map_err(|e| format!("Could not load DLL at {:?}: {}", dll_path, e))?;
        
        let compile: Symbol<CompileFunc> = lib.get(b"axi_compile_file")
            .map_err(|e| format!("Could not find compiler interface: {}", e))?;
        
        // 3. Set the AXI_ROOT environment variable so the compiler knows where to find its toolchains
        let mut root_path = dll_path.clone();
        root_path.pop(); // remove axi_compiler.dll
        root_path.pop(); // remove components
        root_path.push("axi_compiler");
        std::env::set_var("AXI_ROOT", root_path);

        // 4. Convert strings to Windows wide strings (UTF-16)
        let w_input: Vec<u16> = input_path.encode_utf16().chain(std::iter::once(0)).collect();
        let w_output: Vec<u16> = output_path.encode_utf16().chain(std::iter::once(0)).collect();
        
        // 4. Execute the Axi compilation engine
        let result = compile(w_input.as_ptr(), w_output.as_ptr());
        
        if result == 0 {
            Ok(output_path.to_string())
        } else {
            let error_msg = match result {
                2 => "Usage Error (AX_USAGE): Invalid compiler arguments.",
                3 => "Input Error (AX_INPUT): Invalid input file or missing C++ compiler toolchain.",
                4 => "Pipeline Error (AX_PIPELINE): Failed to parse .axi graph. Check for syntax errors or circular dependencies (e.g., self-loops like 'start -> start' are not allowed).",
                5 => "Backend Error (AX_BACKEND): A pipeline backend (like gcc or dotnet) failed to execute.",
                6 => "Artifact Error (AX_ARTIFACT): The compiler ran but failed to produce the final executable.",
                _ => "Unknown Error",
            };
            Err(format!("Compiler failed with error code {} - {}", result, error_msg))
        }
    }
}

fn main() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![compile_axi_code])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
