#ifndef AXIOM_SHA3_HPP
#define AXIOM_SHA3_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace axiom {
namespace crypto {

class SHA3_256 {
private:
    uint64_t state[25] = {0};
    uint8_t buffer[136] = {0};
    size_t buffer_len = 0;

    static uint64_t rotl64(uint64_t x, int y) {
        return (x << y) | (x >> (64 - y));
    }

    void keccak_f1600() {
        const uint64_t RC[24] = {
            0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
            0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
            0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
            0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
            0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
            0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };
        const int r[24] = {
            1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
            27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
        };
        const int piln[24] = {
            10, 7,  11, 17, 18, 3,  5,  16, 8,  21, 24, 4,
            15, 23, 19, 13, 12, 2,  20, 14, 22, 9,  6,  1
        };

        for (int round = 0; round < 24; round++) {
            uint64_t C[5], D[5];
            for (int i = 0; i < 5; i++)
                C[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
            
            for (int i = 0; i < 5; i++)
                D[i] = C[(i + 4) % 5] ^ rotl64(C[(i + 1) % 5], 1);
            
            for (int i = 0; i < 25; i++)
                state[i] ^= D[i % 5];
            
            uint64_t x = state[1], t;
            for (int i = 0; i < 24; i++) {
                int j = piln[i];
                t = state[j];
                state[j] = rotl64(x, r[i]);
                x = t;
            }
            
            for (int j = 0; j < 25; j += 5) {
                uint64_t temp[5];
                for (int i = 0; i < 5; i++) temp[i] = state[j + i];
                for (int i = 0; i < 5; i++) state[j + i] ^= (~temp[(i + 1) % 5]) & temp[(i + 2) % 5];
            }
            
            state[0] ^= RC[round];
        }
    }

    void process_block() {
        for (int i = 0; i < 136 / 8; i++) {
            uint64_t val = 0;
            for (int j = 0; j < 8; j++) {
                val |= ((uint64_t)buffer[i * 8 + j]) << (8 * j);
            }
            state[i] ^= val;
        }
        keccak_f1600();
    }

public:
    void update(const std::string& data) {
        for (char c : data) {
            buffer[buffer_len++] = (uint8_t)c;
            if (buffer_len == 136) {
                process_block();
                buffer_len = 0;
            }
        }
    }

    std::string finalize() {
        buffer[buffer_len] ^= 0x06; // SHA3 padding byte
        buffer[135] ^= 0x80;
        
        process_block();
        
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 32; i++) { // 256 bits = 32 bytes
            int state_idx = i / 8;
            int byte_idx = i % 8;
            uint8_t b = (state[state_idx] >> (8 * byte_idx)) & 0xFF;
            ss << std::setw(2) << (int)b;
        }
        return ss.str();
    }
    
    static std::string hash(const std::string& data) {
        SHA3_256 sha3;
        sha3.update(data);
        return sha3.finalize();
    }
};

} // namespace crypto
} // namespace axiom

#endif
