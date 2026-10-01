using System;

namespace AVM.UI
{
    public class ExecutionEngine
    {
        public enum AvmOpcode : byte
        {
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
            OP_JUMP_IF_FALSE = 10,
            OP_DRAW_GRAPH = 11,
            OP_EQ = 12,
            OP_GT = 13,
            OP_LT = 14,
            OP_ASSERT = 15,
            OP_ALLOC = 16,
            OP_STORE_HEAP = 17,
            OP_LOAD_HEAP = 18,
            OP_PRINT_STR = 19
        }

        private float[] _stack = new float[1024];
        private int _sp = 0;
        
        // Increased heap for dynamic arrays/pointers
        private float[] _heap = new float[65536]; 
        
        // Global variables get 0-999. Pointers allocate from 1000 upwards.
        private int _allocPointer = 1000; 

        // String table loaded from bytecode
        private string[] _stringTable = Array.Empty<string>();

        private float ReadFloatLittleEndian(byte[] bytecode, ref int pc)
        {
            if (pc + 4 > bytecode.Length) throw new Exception("Unexpected EOF while reading float.");
            byte[] bytes = new byte[4];
            bytes[0] = bytecode[pc++];
            bytes[1] = bytecode[pc++];
            bytes[2] = bytecode[pc++];
            bytes[3] = bytecode[pc++];
            if (!BitConverter.IsLittleEndian) Array.Reverse(bytes);
            return BitConverter.ToSingle(bytes, 0);
        }

        private int ReadIntLittleEndian(byte[] bytecode, ref int pc)
        {
            if (pc + 4 > bytecode.Length) throw new Exception("Unexpected EOF while reading int.");
            byte[] bytes = new byte[4];
            bytes[0] = bytecode[pc++];
            bytes[1] = bytecode[pc++];
            bytes[2] = bytecode[pc++];
            bytes[3] = bytecode[pc++];
            if (!BitConverter.IsLittleEndian) Array.Reverse(bytes);
            return BitConverter.ToInt32(bytes, 0);
        }

        private void LoadStringTable(byte[] bytecode)
        {
            // Find OP_HALT, string table starts right after it
            int pos = 0;
            while (pos < bytecode.Length) {
                if (bytecode[pos] == (byte)AvmOpcode.OP_HALT) {
                    pos++; // skip past HALT
                    break;
                }
                // Skip past opcode + operands to find HALT
                AvmOpcode op = (AvmOpcode)bytecode[pos++];
                switch (op) {
                    case AvmOpcode.OP_LOAD_CONST: pos += 4; break;
                    case AvmOpcode.OP_STORE_VAR: pos += 4; break;
                    case AvmOpcode.OP_LOAD_VAR: pos += 4; break;
                    case AvmOpcode.OP_JUMP: pos += 4; break;
                    case AvmOpcode.OP_JUMP_IF_FALSE: pos += 4; break;
                    default: break; // opcodes with no operand
                }
            }

            if (pos >= bytecode.Length) return; // no string table

            int count = ReadIntAt(bytecode, ref pos);
            _stringTable = new string[count];
            for (int i = 0; i < count; i++) {
                int len = ReadIntAt(bytecode, ref pos);
                _stringTable[i] = System.Text.Encoding.UTF8.GetString(bytecode, pos, len);
                pos += len;
            }
        }

        private int ReadIntAt(byte[] data, ref int pos)
        {
            byte[] bytes = new byte[4];
            bytes[0] = data[pos++];
            bytes[1] = data[pos++];
            bytes[2] = data[pos++];
            bytes[3] = data[pos++];
            if (!BitConverter.IsLittleEndian) Array.Reverse(bytes);
            return BitConverter.ToInt32(bytes, 0);
        }

        public void Run(byte[] bytecode, Action<string> printCallback, Action<float> graphCallback = null)
        {
            LoadStringTable(bytecode);
            int pc = 0;
            while (pc < bytecode.Length)
            {
                AvmOpcode op = (AvmOpcode)bytecode[pc++];
                switch (op)
                {
                    case AvmOpcode.OP_HALT:
                        printCallback("AVM Halted cleanly.");
                        return;
                    
                    case AvmOpcode.OP_LOAD_CONST:
                        float val = ReadFloatLittleEndian(bytecode, ref pc);
                        if (_sp >= _stack.Length) throw new Exception("FATAL: Stack Overflow!");
                        _stack[_sp++] = val;
                        break;
                    
                    case AvmOpcode.OP_STORE_VAR:
                        int varId = ReadIntLittleEndian(bytecode, ref pc);
                        if (varId < 0 || varId >= 1000) throw new Exception($"FATAL: Invalid Global Variable ID {varId}");
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_STORE_VAR!");
                        _heap[varId] = _stack[--_sp];
                        break;
                    
                    case AvmOpcode.OP_LOAD_VAR:
                        varId = ReadIntLittleEndian(bytecode, ref pc);
                        if (varId < 0 || varId >= 1000) throw new Exception($"FATAL: Invalid Global Variable ID {varId}");
                        if (_sp >= _stack.Length) throw new Exception("FATAL: Stack Overflow!");
                        _stack[_sp++] = _heap[varId];
                        break;
                        
                    case AvmOpcode.OP_ALLOC:
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_ALLOC!");
                        int size = (int)_stack[--_sp];
                        if (size <= 0) throw new Exception("FATAL: Cannot allocate zero or negative size.");
                        if (_allocPointer + size >= _heap.Length) throw new Exception("FATAL: Out of Heap Memory!");
                        
                        int ptr = _allocPointer;
                        _allocPointer += size; // Arena style bump
                        _stack[_sp++] = (float)ptr; // push pointer as float
                        break;

                    case AvmOpcode.OP_STORE_HEAP:
                        if (_sp < 3) throw new Exception("FATAL: Stack Underflow during OP_STORE_HEAP!");
                        int pointer = (int)_stack[--_sp];
                        int offset = (int)_stack[--_sp];
                        float hval = _stack[--_sp];
                        
                        if (pointer < 1000 || pointer + offset >= _heap.Length) throw new Exception($"FATAL: Segmentation Fault! Bad pointer {pointer} offset {offset}");
                        _heap[pointer + offset] = hval;
                        break;

                    case AvmOpcode.OP_LOAD_HEAP:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow during OP_LOAD_HEAP!");
                        pointer = (int)_stack[--_sp];
                        offset = (int)_stack[--_sp];
                        
                        if (pointer < 1000 || pointer + offset >= _heap.Length) throw new Exception($"FATAL: Segmentation Fault! Bad pointer {pointer} offset {offset}");
                        _stack[_sp++] = _heap[pointer + offset];
                        break;
                    
                    case AvmOpcode.OP_ADD:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow during OP_ADD!");
                        float b = _stack[--_sp];
                        float a = _stack[--_sp];
                        _stack[_sp++] = a + b;
                        break;

                    case AvmOpcode.OP_SUB:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow during OP_SUB!");
                        b = _stack[--_sp];
                        a = _stack[--_sp];
                        _stack[_sp++] = a - b;
                        break;

                    case AvmOpcode.OP_MUL:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow during OP_MUL!");
                        b = _stack[--_sp];
                        a = _stack[--_sp];
                        _stack[_sp++] = a * b;
                        break;

                    case AvmOpcode.OP_DIV:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow during OP_DIV!");
                        b = _stack[--_sp];
                        if (Math.Abs(b) < 0.0000001f) throw new Exception("FATAL: Division by Zero!");
                        a = _stack[--_sp];
                        _stack[_sp++] = a / b;
                        break;
                        
                    case AvmOpcode.OP_EQ:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow!");
                        b = _stack[--_sp]; a = _stack[--_sp];
                        _stack[_sp++] = Math.Abs(a - b) < 0.0001f ? 1.0f : 0.0f;
                        break;

                    case AvmOpcode.OP_GT:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow!");
                        b = _stack[--_sp]; a = _stack[--_sp];
                        _stack[_sp++] = (a > b) ? 1.0f : 0.0f;
                        break;

                    case AvmOpcode.OP_LT:
                        if (_sp < 2) throw new Exception("FATAL: Stack Underflow!");
                        b = _stack[--_sp]; a = _stack[--_sp];
                        _stack[_sp++] = (a < b) ? 1.0f : 0.0f;
                        break;

                    case AvmOpcode.OP_ASSERT:
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow!");
                        if (_stack[--_sp] <= 0.0001f) throw new Exception("AVM ASSERTION FAILED: Mathematical Pre/Post Condition Violated at runtime!");
                        break;

                    case AvmOpcode.OP_JUMP:
                        pc = ReadIntLittleEndian(bytecode, ref pc);
                        break;

                    case AvmOpcode.OP_JUMP_IF_FALSE:
                        int targetPc = ReadIntLittleEndian(bytecode, ref pc);
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_JUMP_IF_FALSE!");
                        float condition = _stack[--_sp];
                        if (condition <= 0.0001f) {
                            pc = targetPc;
                        }
                        break;

                    case AvmOpcode.OP_PRINT:
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_PRINT!");
                        val = _stack[--_sp];
                        printCallback(val.ToString("F4"));
                        break;

                    case AvmOpcode.OP_PRINT_STR:
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_PRINT_STR!");
                        int strIdx = (int)_stack[--_sp];
                        if (strIdx < 0 || strIdx >= _stringTable.Length)
                            throw new Exception($"FATAL: Invalid String Table Index {strIdx} (table has {_stringTable.Length} entries)");
                        printCallback(_stringTable[strIdx]);
                        break;
                        
                    case AvmOpcode.OP_DRAW_GRAPH:
                        if (_sp <= 0) throw new Exception("FATAL: Stack Underflow during OP_DRAW_GRAPH!");
                        val = _stack[--_sp];
                        graphCallback?.Invoke(val);
                        break;
                    
                    default:
                        throw new Exception($"FATAL: Unknown Opcode {(byte)op} at PC {pc - 1}");
                }
            }
        }
    }
}
