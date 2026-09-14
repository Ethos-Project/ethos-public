#pragma once
#include <string>

namespace axi {
namespace compiler {

    // Pure virtual interface for the Axi Compiler Engine
    class ICompiler {
    public:
        virtual ~ICompiler() = default;

        virtual bool compile(const std::string& sourcePath, const std::string& outPath) = 0;
        virtual std::string getAst(const std::string& sourcePath) = 0;
        virtual void setOptimizationLevel(int level) = 0;
    };

} // namespace compiler
} // namespace axi
