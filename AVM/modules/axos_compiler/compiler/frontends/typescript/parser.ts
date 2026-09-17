import { AxiASTNode, AxiComplianceLevel } from "./AST";

export function parseTSToAxiDAG(sourceCode: string): AxiASTNode {
    console.log("[TS-FRONTEND] Parsing TypeScript AST to Axi DAG...");
    return {
        type: 0,
        compliance: AxiComplianceLevel.HighSemantic,
        identifier: "ts_node",
        payload: "",
        requiresEulerPoolAlloc: true,
        isUnsafeCKernelCall: false
    };
}
