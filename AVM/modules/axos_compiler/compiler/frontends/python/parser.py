# =====================================================================
# MODULE: frontends.python.parser
# CLASSIFICATION: TIER 3 (RESTRICTED / INTERNAL ONLY)
# DESCRIPTION: Python parser that routes native AST via FFI to Axi core.
# =====================================================================

import ast
from AST import create_python_high_node, AxiASTNode

class AxiPythonBridge(ast.NodeVisitor):
    def __init__(self):
        self.axi_nodes = []

    def visit_FunctionDef(self, node):
        # When we find a Python 'def', we map it to a HIGH semantic Axi node
        print(f"[PY-FRONTEND] Parsing Python function: {node.name}")
        axi_node = create_python_high_node(node.name)
        self.axi_nodes.append(axi_node)
        self.generic_visit(node)

def parse_python_to_axi_dag(source_code: str):
    print("[PY-FRONTEND] Initiating Python -> Axi DAG mapping...")
    tree = ast.parse(source_code)
    bridge = AxiPythonBridge()
    bridge.visit(tree)
    # The CTypes array would then be passed to the C-kernel's emit_axi_c
    return bridge.axi_nodes
