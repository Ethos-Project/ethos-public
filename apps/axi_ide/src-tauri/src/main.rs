// Prevents additional console window on Windows in release
#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

use tauri::command;
use std::fs;
use std::path::Path;
use libloading::{Library, Symbol};
use serde::{Serialize, Deserialize};

// Hook into the axi_compiler.dll C-API we built
type CompileFunc = unsafe extern "C" fn(*const u16, *const u16) -> i32;

#[command]
fn compile_axi_code(source_code: String) -> Result<String, String> {
    let input_path = "temp_workspace.axi";
    let output_path = "temp_workspace.exe";
    
    fs::write(input_path, source_code).map_err(|e| e.to_string())?;

    unsafe {
        let mut dll_path = std::env::current_exe().map_err(|e| e.to_string())?;
        dll_path.pop(); // remove executable name
        dll_path.pop(); // remove 'apps' or 'release'
        if dll_path.ends_with("target") {
            dll_path.pop(); dll_path.pop(); dll_path.pop();
        }
        dll_path.push("components");
        dll_path.push("axi_compiler.dll");

        let lib = Library::new(&dll_path)
            .map_err(|e| format!("Could not load DLL at {:?}: {}", dll_path, e))?;
        
        let compile: Symbol<CompileFunc> = lib.get(b"axi_compile_file")
            .map_err(|e| format!("Could not find compiler interface: {}", e))?;
        
        let mut root_path = dll_path.clone();
        root_path.pop(); root_path.pop();
        root_path.push("axi_compiler");
        std::env::set_var("AXI_ROOT", root_path);

        let w_input: Vec<u16> = input_path.encode_utf16().chain(std::iter::once(0)).collect();
        let w_output: Vec<u16> = output_path.encode_utf16().chain(std::iter::once(0)).collect();
        
        let result = compile(w_input.as_ptr(), w_output.as_ptr());
        
        if result == 0 {
            Ok(output_path.to_string())
        } else {
            let error_msg = match result {
                2 => "Usage Error (AX_USAGE): Invalid compiler arguments.",
                3 => "Input Error (AX_INPUT): Invalid input file or missing C++ compiler toolchain.",
                4 => "Pipeline Error (AX_PIPELINE): Failed to parse .axi graph. Check for syntax errors or circular dependencies.",
                5 => "Backend Error (AX_BACKEND): A pipeline backend (like gcc or dotnet) failed to execute.",
                6 => "Artifact Error (AX_ARTIFACT): The compiler ran but failed to produce the final executable.",
                _ => "Unknown Error",
            };
            Err(format!("Compiler failed with error code {} - {}", result, error_msg))
        }
    }
}

// --------------------------------------------------------
// NEW OS BINDINGS
// --------------------------------------------------------

#[derive(Serialize)]
struct FileNode {
    name: String,
    path: String,
    is_dir: bool,
}

#[command]
fn read_dir(path: String) -> Result<Vec<FileNode>, String> {
    let mut nodes = Vec::new();
    let entries = fs::read_dir(&path).map_err(|e| e.to_string())?;
    for entry in entries {
        if let Ok(entry) = entry {
            let path_buf = entry.path();
            let is_dir = path_buf.is_dir();
            nodes.push(FileNode {
                name: entry.file_name().to_string_lossy().to_string(),
                path: path_buf.to_string_lossy().to_string(),
                is_dir,
            });
        }
    }
    nodes.sort_by(|a, b| {
        if a.is_dir == b.is_dir {
            a.name.cmp(&b.name)
        } else if a.is_dir {
            std::cmp::Ordering::Less
        } else {
            std::cmp::Ordering::Greater
        }
    });
    Ok(nodes)
}

#[command]
fn read_file(path: String) -> Result<String, String> {
    fs::read_to_string(path).map_err(|e| e.to_string())
}

#[command]
fn write_file(path: String, contents: String) -> Result<(), String> {
    fs::write(path, contents).map_err(|e| e.to_string())
}

#[command]
fn execute_program(path: String) -> Result<String, String> {
    let output = std::process::Command::new(&path)
        .output()
        .map_err(|e| format!("Failed to execute {}: {}", path, e))?;
    
    let mut result = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();
    if !stderr.is_empty() {
        result.push_str("\n[STDERR]\n");
        result.push_str(&stderr);
    }
    Ok(result)
}

fn main() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![
            compile_axi_code,
            read_dir,
            read_file,
            write_file,
            execute_program
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
