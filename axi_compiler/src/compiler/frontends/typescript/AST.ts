export enum AxiComplianceLevel {
    HighSemantic = 0,
    LowBareMetal = 1,
    BoundaryBridge = 2
}

export interface AxiASTNode {
    type: number;
    compliance: AxiComplianceLevel;
    identifier: string;
    payload: string;
    requiresEulerPoolAlloc: boolean;
    isUnsafeCKernelCall: boolean;
    leftChild?: AxiASTNode;
    rightChild?: AxiASTNode;
}
