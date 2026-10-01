#include <iostream>
#include "../interfaces/IFilesystem.hpp"
#include "../interfaces/IAxi_dvcs.hpp"
#include "../interfaces/ICompiler.hpp"
#include "../interfaces/IUXPlugin.hpp"

// Simple mock implementation to verify the pure virtuals can be inherited
class MockFilesystem : public axi::core::IFilesystem {
public:
    std::string queryNode(const std::string& uri) override { return "node"; }
    std::string commitNode(const std::string& hash, const std::string& ns, const std::string& payload) override { return "commit"; }
    std::string jsonToToon(const std::string& rawJson) override { return "toon"; }
};

int main() {
    MockFilesystem fs;
    std::cout << "Interfaces compiled successfully." << std::endl;
    return 0;
}
