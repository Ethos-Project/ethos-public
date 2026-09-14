#pragma once
#include <string>

namespace axi {
namespace core {

    // Pure virtual interface for the DAG Filesystem
    class IFilesystem {
    public:
        virtual ~IFilesystem() = default;
        
        virtual std::string queryNode(const std::string& uri) = 0;
        virtual std::string commitNode(const std::string& hash, const std::string& ns, const std::string& payload) = 0;
        virtual std::string jsonToToon(const std::string& rawJson) = 0;
    };

} // namespace core
} // namespace axi
