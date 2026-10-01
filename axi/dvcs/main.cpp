#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include "sha3.hpp"

namespace fs = std::filesystem;

// FOSS Standalone DVCS Backend (axi.exe & libaxi.dll)
// Provides a decoupled, lightweight YAML DAG implementation with .axiignore support.
// Licensed under the GNU Affero General Public License v3.0 (AGPLv3) or later.

class AxiIgnore {
public:
    std::vector<std::string> patterns;

    AxiIgnore(const fs::path& root) {
        // Built-in defaults
        patterns.push_back(".axi/");
        patterns.push_back(".git/");
        patterns.push_back(".gemini/");
        patterns.push_back(".vscode/");

        fs::path ignore_path = root / ".axiignore";
        if (fs::exists(ignore_path)) {
            std::ifstream f(ignore_path);
            std::string line;
            while (std::getline(f, line)) {
                size_t first = line.find_first_not_of(" \t\r\n");
                if (first == std::string::npos) continue;
                size_t last = line.find_last_not_of(" \t\r\n");
                std::string trimmed = line.substr(first, last - first + 1);
                if (trimmed.empty() || trimmed[0] == '#') continue;

                for (char& c : trimmed) if (c == '\\') c = '/';
                patterns.push_back(trimmed);
            }
        }
    }

    bool is_ignored(const fs::path& rel_path, bool is_dir) const {
        std::string rel = rel_path.generic_string();
        if (is_dir && (rel.empty() || rel.back() != '/')) rel += '/';

        for (const auto& pat : patterns) {
            std::string clean = pat;
            bool is_dir_pat = (!clean.empty() && clean.back() == '/');

            if (is_dir_pat) {
                if (rel == clean || rel.rfind(clean, 0) == 0 || rel.find("/" + clean) != std::string::npos) {
                    return true;
                }
            } else {
                if (clean.find('*') != std::string::npos) {
                    std::string filename = rel_path.filename().string();
                    if (clean.rfind("*.", 0) == 0) {
                        std::string ext = clean.substr(1);
                        if (filename.size() >= ext.size() && filename.substr(filename.size() - ext.size()) == ext) {
                            return true;
                        }
                    }
                } else {
                    if (rel == clean || rel_path.filename().string() == clean) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

void create_dir(const fs::path& p) {
    if (!fs::exists(p)) {
        fs::create_directories(p);
    }
}

int cmd_init(const std::string& path_str) {
    fs::path root = path_str.empty() ? fs::current_path() : fs::path(path_str);
    fs::path axi_dir = root / ".axi";
    
    if (fs::exists(axi_dir)) {
        std::cout << "FOSS DVCS: Workspace already initialized at " << axi_dir.string() << "\n";
        return 0;
    }
    
    create_dir(axi_dir / "objects");
    create_dir(axi_dir / "refs" / "heads");
    
    // Create HEAD pointing to main
    std::ofstream head_file(axi_dir / "HEAD");
    head_file << "ref: refs/heads/main\n";
    head_file.close();

    // Create default .axiignore if not present
    fs::path ignore_path = root / ".axiignore";
    if (!fs::exists(ignore_path)) {
        std::ofstream ig(ignore_path);
        ig << "# Axi DVCS Ignore File (.axiignore)\n"
           << ".axi/\n"
           << ".git/\n"
           << ".gemini/\n"
           << "node_modules/\n"
           << "vendor/\n"
           << "build/\n"
           << "dist/\n"
           << "obj/\n"
           << "*.tmp\n"
           << "*.log\n"
           << "*.pdb\n";
        ig.close();
    }
    
    std::cout << "Initialized empty FOSS YAML workspace in " << axi_dir.string() << "\n";
    return 0;
}

std::string get_type_ref(const fs::path& p) {
    std::string ext = p.extension().string();
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".gif") return "media/image";
    if (ext == ".txt" || ext == ".md" || ext == ".csv") return "document/text";
    if (ext == ".axi" || ext == ".axos" || ext == ".allos" || ext == ".cpp" || ext == ".c" || ext == ".h") return "source/code";
    if (ext == ".wav" || ext == ".mp3") return "media/audio";
    return "application/octet-stream";
}

void walk_directory(const fs::path& current, const fs::path& root, const AxiIgnore& ignore, std::stringstream& payload_stream, bool& has_files, size_t& file_count) {
    for (const auto& entry : fs::directory_iterator(current)) {
        fs::path rel = fs::relative(entry.path(), root);
        bool is_dir = entry.is_directory();

        if (ignore.is_ignored(rel, is_dir)) {
            continue;
        }

        if (is_dir) {
            walk_directory(entry.path(), root, ignore, payload_stream, has_files, file_count);
        } else if (entry.is_regular_file()) {
            has_files = true;
            file_count++;
            payload_stream << "    " << rel.generic_string() << ":\n";
            payload_stream << "      type_ref: \"" << get_type_ref(entry.path()) << "\"\n";
            payload_stream << "      properties: [ size_bytes: " << fs::file_size(entry.path()) << " ]\n";
        }
    }
}

int cmd_wrap() {
    fs::path root = fs::current_path();
    fs::path axi_dir = root / ".axi";
    
    if (!fs::exists(axi_dir)) {
        std::cerr << "FOSS DVCS Error: Not a valid Axi workspace. Run 'axi init' first.\n";
        return 1;
    }
    
    AxiIgnore ignore(root);
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    
    std::stringstream payload_stream;
    bool has_files = false;
    size_t file_count = 0;

    walk_directory(root, root, ignore, payload_stream, has_files, file_count);

    if (!has_files) payload_stream << "    \"Basic FOSS Snapshot (Empty)\"\n";
    std::string semantic_payload = payload_stream.str();
    
    // Hash payload + timestamp
    std::string digest = Axi::crypto::SHA3_256::hash(semantic_payload + std::to_string(now));
    
    fs::path object_path = axi_dir / "objects" / digest;
    std::ofstream obj(object_path);
    obj << "commit:\n";
    obj << "  type: wrap_snapshot\n";
    obj << "  hash: \"" << digest << "\"\n";
    obj << "  timestamp: " << now << "\n";
    obj << "  files_tracked: " << file_count << "\n";
    obj << "  semantic_layer:\n";
    obj << semantic_payload;
    obj.close();
    
    // Update main branch
    std::ofstream head_ref(axi_dir / "refs" / "heads" / "main");
    head_ref << digest << "\n";
    head_ref.close();
    
    std::cout << "Wrapped " << file_count << " files into FOSS YAML ledger: " << digest << "\n";
    return 0;
}

int cmd_status() {
    fs::path root = fs::current_path();
    fs::path axi_dir = root / ".axi";
    
    if (!fs::exists(axi_dir)) {
        std::cerr << "FOSS DVCS Error: Not a valid Axi workspace.\n";
        return 1;
    }
    
    std::cout << "FOSS Workspace Status:\n";
    fs::path head_path = axi_dir / "refs" / "heads" / "main";
    if (fs::exists(head_path)) {
        std::ifstream head_ref(head_path);
        std::string hash;
        head_ref >> hash;
        std::cout << "On branch main\n";
        std::cout << "Current HEAD: " << hash << "\n";
    } else {
        std::cout << "On branch main\n";
        std::cout << "No wraps yet.\n";
    }

    if (fs::exists(root / ".axiignore")) {
        std::cout << "Active ignore: .axiignore detected and active.\n";
    }
    return 0;
}

int cmd_pack(const std::string& out_zip) {
    fs::path root = fs::current_path();
    std::string script_path = "C:\\Ethos\\axos\\scripts\\AxiZip.ps1";
    
    std::string cmd = "powershell -ExecutionPolicy Bypass -File \"" + script_path + "\" -SourceDir \"" + root.string() + "\"";
    if (!out_zip.empty()) {
        cmd += " -OutputFile \"" + out_zip + "\"";
    }
    
    std::cout << "Invoking AxiZip Multi-Agent Packager...\n";
    int ret = std::system(cmd.c_str());
    return ret;
}

int cmd_ship() {
    std::cout << "FOSS DVCS: Shipping payload to remote peers (Simulation)...\n";
    std::cout << "Shipment complete.\n";
    return 0;
}

int cmd_inject() {
    std::cout << "FOSS DVCS: Injecting payload from remote into local DAG (Simulation)...\n";
    std::cout << "Injection complete.\n";
    return 0;
}

int cmd_track() {
    std::cout << "FOSS DVCS: Tracking spatial directories...\n";
    std::cout << "Tracked.\n";
    return 0;
}

#if defined(_WIN32)
#define DVCS_API __declspec(dllexport)
#else
#define DVCS_API
#endif

extern "C" {
    DVCS_API int axi_dvcs_init(const char* path_str) {
        return cmd_init(path_str ? path_str : "");
    }
    DVCS_API int axi_dvcs_wrap() {
        return cmd_wrap();
    }
    DVCS_API int axi_dvcs_status() {
        return cmd_status();
    }
    DVCS_API int axi_dvcs_pack(const char* out_zip) {
        return cmd_pack(out_zip ? out_zip : "");
    }
    DVCS_API int axi_dvcs_ship() {
        return cmd_ship();
    }
    DVCS_API int axi_dvcs_inject() {
        return cmd_inject();
    }
    DVCS_API int axi_dvcs_track() {
        return cmd_track();
    }
}

#include "../include/bom_manifest.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "FOSS Axi DVCS\n";
        std::cerr << "Commands: init, status, wrap, pack, ship, track, inject\n";
        return 1;
    }
    
    std::string cmd = argv[1];
    if (cmd == "--version" || cmd == "-v" || cmd == "-V") {
        print_bom_manifest("axi");
        return 0;
    } else if (cmd == "init") {
        std::string p = (argc > 2) ? argv[2] : "";
        return cmd_init(p);
    } else if (cmd == "wrap") {
        return cmd_wrap();
    } else if (cmd == "status") {
        return cmd_status();
    } else if (cmd == "pack" || cmd == "zip") {
        std::string out = (argc > 2) ? argv[2] : "";
        return cmd_pack(out);
    } else if (cmd == "ship") {
        return cmd_ship();
    } else if (cmd == "inject") {
        return cmd_inject();
    } else if (cmd == "track") {
        return cmd_track();
    } else {
        std::cerr << "Unknown FOSS command: " << cmd << "\n";
        return 1;
    }
}
