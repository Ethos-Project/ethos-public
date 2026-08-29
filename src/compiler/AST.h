#ifndef AST_DAG_H
#define AST_DAG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../shm_bus.h"
#include "../euler_pool.h"

namespace Axiom {

enum class WireStyle {
    SOLID,
    BRAIDED,
    PRISMATIC,
    ASYNC
};

class Node;

struct Wire {
    std::shared_ptr<Node> sourceNode;
    std::shared_ptr<Node> targetNode;
    std::string dataType;
    WireStyle style;

    float memory_weight;
    bool is_euler_circuit;
};

struct Circuit {
    std::vector<std::shared_ptr<Wire>> path;
    uint64_t heap_offset; // Map to the FFI_SharedMemory
};

struct Pin {
    std::string name;
    std::string type;
    uint64_t data_offset; // Zero-copy memory reference to FFI_SharedMemory
};

class Node {
public:
    std::string nodeID;
    std::string title;
    float ui_x, ui_y;

    bool is_c_native_block;
    std::string raw_source_code;

    std::vector<Pin> input_pins;
    std::vector<Pin> output_pins;

    bool data_ready() const { return true; }
    void execute() {}
};

class DAG {
public:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Wire>> edges;
    std::vector<Circuit> active_circuits;

    FFI_SharedMemory* shm_bus;

    bool performTopologicalSort() { return true; }

    void verifyEulerianCircuits();
};

}

#endif
