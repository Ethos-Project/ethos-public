package axi_frontend

import "C"

type AxiComplianceLevel int

const (
    HighSemantic AxiComplianceLevel = 0
    LowBareMetal AxiComplianceLevel = 1
    BoundaryBridge AxiComplianceLevel = 2
)

type AxiASTNode struct {
    Type int
    Compliance AxiComplianceLevel
    Identifier *C.char
    Payload *C.char
    RequiresEulerPoolAlloc bool
    IsUnsafeCKernelCall bool
    LeftChild *C.void
    RightChild *C.void
}
