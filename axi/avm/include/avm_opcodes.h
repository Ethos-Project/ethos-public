#ifndef AVM_OPCODES_H
#define AVM_OPCODES_H

typedef enum {
    OP_HALT = 0,
    OP_LOAD_CONST = 1,
    OP_STORE_VAR = 2,
    OP_LOAD_VAR = 3,
    OP_ADD = 4,
    OP_SUB = 5,
    OP_MUL = 6,
    OP_DIV = 7,
    OP_PRINT = 8,
    OP_JUMP = 9,
    OP_JUMP_IF_FALSE = 10
} AvmOpcode;

#endif // AVM_OPCODES_H
