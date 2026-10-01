#!/bin/bash
g++ -shared -o dev_forge.dll lexer.cpp AST.cpp parser.cpp c_api.cpp ../../eros_flat_core/euler_pool.cpp -static -static-libgcc -static-libstdc++
