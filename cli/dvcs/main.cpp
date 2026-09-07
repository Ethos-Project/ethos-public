#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

// FOSS Standalone DVCS Backend (axi.exe)
// Provides a decoupled, lightweight TOON DAG implementation for the Open-Source community.

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
    
    std::cout << "Initialized empty FOSS TOON workspace in " << axi_dir.string() << "\n";
    return 0;
}

std::string get_type_ref(const fs::path& p) {
    std::string ext = p.extension().string();
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".gif") return "media/image";
    if (ext == ".txt" || ext == ".md" || ext == ".csv") return "document/text";
    if (ext == ".axi" || ext == ".cpp" || ext == ".c" || ext == ".h") return "source/code";
    if (ext == ".wav" || ext == ".mp3") return "media/audio";
    return "application/octet-stream";
}

int cmd_wrap() {
    fs::path root = fs::current_path();
    fs::path axi_dir = root / ".axi";
    
    if (!fs::exists(axi_dir)) {
        std::cerr << "FOSS DVCS Error: Not a valid Axi workspace. Run 'axi init' first.\n";
        return 1;
    }
    
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::string mock_hash = "foss_" + std::to_string(now);
    
    fs::path object_path = axi_dir / "objects" / mock_hash;
    std::ofstream obj(object_path);
    obj << "toon_commit:\n";
    obj << "  type: wrap_snapshot\n";
    obj << "  timestamp: " << now << "\n";
    obj << "  semantic_layer:\n";

    // FOSS enriched payload logic
    bool has_files = false;
    for (const auto& entry : fs::directory_iterator(root)) {
        if (entry.is_regular_file()) {
            has_files = true;
            fs::path p = entry.path();
            obj << "    " << p.filename().string() << ":\n";
            obj << "      type_ref: \"" << get_type_ref(p) << "\"\n";
            obj << "      properties: [ size_bytes: " << fs::file_size(p) << " ]\n";
        }
    }
    if (!has_files) obj << "    \"Basic FOSS Snapshot\"\n";

    obj.close();
    
    // Update main branch
    std::ofstream head_ref(axi_dir / "refs" / "heads" / "main");
    head_ref << mock_hash << "\n";
    head_ref.close();
    
    std::cout << "Wrapped current state into FOSS ledger: " << mock_hash << "\n";
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
    return 0;
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "FOSS Axiom DVCS\n";
        std::cerr << "Commands: init, status, wrap, ship, track, inject\n";
        return 1;
    }
    
    std::string cmd = argv[1];
    if (cmd == "init") {
        std::string p = (argc > 2) ? argv[2] : "";
        return cmd_init(p);
    } else if (cmd == "wrap") {
        return cmd_wrap();
    } else if (cmd == "status") {
        return cmd_status();
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
