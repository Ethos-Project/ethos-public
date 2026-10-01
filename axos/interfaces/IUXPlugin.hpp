#pragma once

namespace axi {
namespace ux {

    // Pure virtual interface for extensible UX and IDE Plugins
    class IUXPlugin {
    public:
        virtual ~IUXPlugin() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void renderUI() = 0;
    };

} // namespace ux
} // namespace axi
