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
fn compile_axi_code(file_path: String) -> Result<String, String> {
    let input_path = std::path::Path::new(&file_path);
    let output_path = "temp_workspace.exe";
    let ext = input_path.extension().and_then(|e| e.to_str()).unwrap_or("");
    
    unsafe {
        let mut dll_dir = std::env::current_exe().map_err(|e| e.to_string())?;
        dll_dir.pop(); 
        dll_dir.pop(); 
        if dll_dir.ends_with("target") {
            dll_dir.pop(); dll_dir.pop(); dll_dir.pop();
        }
        dll_dir.push("components");
        
        let mut root_path = dll_dir.clone();
        root_path.pop(); root_path.pop();
        root_path.push("axi_compiler");
        std::env::set_var("AXI_ROOT", root_path);

        let is_transpile = ext == "py" || ext == "cs";
        let mut final_compile_path = file_path.clone();
        
        if is_transpile {
            let mut transpile_dll = dll_dir.clone();
            transpile_dll.push("axi_compiler.dll");
            
            let lib = Library::new(&transpile_dll)
                .map_err(|e| format!("Could not load Transpiler DLL at {:?}: {}", transpile_dll, e))?;
            
            let transpile: Symbol<CompileFunc> = lib.get(b"axi_transpile_file")
                .map_err(|e| format!("Could not find transpiler interface: {}", e))?;
                
            final_compile_path = "temp_workspace.axi".to_string();
            let w_input: Vec<u16> = file_path.encode_utf16().chain(std::iter::once(0)).collect();
            let w_output: Vec<u16> = final_compile_path.encode_utf16().chain(std::iter::once(0)).collect();
            
            let result = transpile(w_input.as_ptr(), w_output.as_ptr());
            if result != 0 {
                return Err(format!("Transpiler failed with error code {}", result));
            }
        }
        
        // Native Compilation using axi.dll
        let mut lang_dll = dll_dir.clone();
        lang_dll.push("axi.dll");
        
        let lib = Library::new(&lang_dll)
            .map_err(|e| format!("Could not load Language DLL at {:?}: {}", lang_dll, e))?;
            
        let compile: Symbol<CompileFunc> = lib.get(b"axi_compile_file")
            .map_err(|e| format!("Could not find compiler interface: {}", e))?;
            
        let w_input: Vec<u16> = final_compile_path.encode_utf16().chain(std::iter::once(0)).collect();
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

#[command]
fn run_shell(cmd: String, cwd: String) -> Result<String, String> {
    let current_dir = if cwd.is_empty() { ".".to_string() } else { cwd };
    
    let mut path = std::env::var("PATH").unwrap_or_default();
    path.push_str(";C:\\Ethos\\ethos-public\\axi_dvcs;C:\\Ethos\\ethos-public\\axi_compiler;C:\\Ethos\\bin");
    
    let output = std::process::Command::new("powershell")
        .args(&["-NoProfile", "-NonInteractive", "-Command", &cmd])
        .current_dir(&current_dir)
        .env("PATH", path)
        .output()
        .map_err(|e| format!("Shell error: {}", e))?;
    
    let mut result = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();
    if !stderr.is_empty() {
        if !result.is_empty() { result.push('\n'); }
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
            execute_program,
            run_shell
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

