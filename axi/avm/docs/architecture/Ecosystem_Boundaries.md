# Ethos Ecosystem Boundaries & Architecture

This document defines the strict, unified separation of concerns across the Ethos language ecosystem. By enforcing these boundaries, we prevent tooling overlaps and ensure the DVCS, the execution engine, and the package manager operate without conflict.

---

## 1. The Declarative Orchestrator: `Axi`
* **File Extension:** `.axi`
* **CLI Tool:** `axi.exe` (The DVCS)
* **Role:** Project architecture, routing, and version control.
* **Boundary:** Axi is purely **declarative**. It maps the Directed Acyclic Graph (DAG), tracks states, manages `.axiignore`, and defines which nodes connect to which (e.g., `init -> main_loop`). 
* **Rule:** Axi **never** executes imperative logic, loops, or math. It only routes data and manages the repository state.

## 2. The Execution Engine: `Axos` & `Allos`
* **File Extensions:** `.axos` (FOSS) and `.allos` (Enterprise Superset)
* **CLI Tool:** `avm.exe` (The Axi Virtual Machine Headless Console)
* **Role:** Computation and imperative logic.
* **Boundary:** The AVM is purely a **runtime engine**. It receives an `.axos` or `.allos` file, transpiles it, and executes the C_Native logic, algorithms, and `while` loops inside the node. 
* **Rule:** The AVM **never** tracks repository states, branches, or orchestration graphs. It is blind to the larger project structure; it only executes the logic handed to it.

## 3. The Universal Command Layer: `Allforge`
* **CLI Tool:** `allforge` (The Package Manager)
* **Role:** The user-facing glue and orchestrator.
* **Boundary:** Allforge sits above Axi and the AVM. 
* **Pipeline Flow:**
  1. The user runs `allforge run`.
  2. Allforge reads the `.axi` file to understand the project DAG and dependencies.
  3. Allforge resolves any package requirements in `.allos/`.
  4. Allforge uses `axi.exe` to take snapshots of the current state.
  5. Allforge passes the specific `.axos` or `.allos` computation nodes to the AVM (`avm.exe`) to execute.
  6. The AVM returns the output to Allforge, which passes the state back to the `.axi` DAG for the next operation.

By adhering to this pipeline, the DVCS is decoupled from the runtime, creating a modular, lightning-fast architecture.
