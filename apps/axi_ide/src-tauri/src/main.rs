// Prevents additional console window on Windows in release
#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

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
        // 2. Load the C++ DLL dynamically
        let lib = Library::new("../../components/axi_compiler.dll")
            .map_err(|e| format!("Could not load DLL: {}", e))?;
        
        let compile: Symbol<CompileFunc> = lib.get(b"axi_compile_file")
            .map_err(|e| format!("Could not find compiler interface: {}", e))?;
        
        // 3. Convert strings to Windows wide strings (UTF-16)
        let w_input: Vec<u16> = input_path.encode_utf16().chain(std::iter::once(0)).collect();
        let w_output: Vec<u16> = output_path.encode_utf16().chain(std::iter::once(0)).collect();
        
        // 4. Execute the Axi compilation engine
        let result = compile(w_input.as_ptr(), w_output.as_ptr());
        
        if result == 0 {
            Ok(output_path.to_string())
        } else {
            Err(format!("Compiler returned error code: {}", result))
        }
    }
}

fn main() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![compile_axi_code])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
