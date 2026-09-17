#pragma once
#include <string>

namespace axi {
namespace dvcs {

    // Pure virtual interface for the YAML DAG DVCS backend
    class IAxi_dvcs {
    public:
        virtual ~IAxi_dvcs() = default;

        virtual int initWorkspace(const std::string& path) = 0;
        virtual int wrapState() = 0;
        virtual int getStatus() = 0;
        virtual int ship() = 0;
        virtual int inject() = 0;
        virtual int trackDirectories() = 0;
    };

} // namespace dvcs
} // namespace axi

