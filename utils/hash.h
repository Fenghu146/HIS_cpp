#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
using namespace std;

// 自实现 SHA256，不依赖 OpenSSL
// 参考 RFC 6234，输出 64 字符小写十六进制哈希串

static const uint32_t SHA256_K[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static inline uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z)  { return (x & y) ^ (~x & z); }
static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
static inline uint32_t sig0(uint32_t x) { return rotr(x,2)  ^ rotr(x,13) ^ rotr(x,22); }
static inline uint32_t sig1(uint32_t x) { return rotr(x,6)  ^ rotr(x,11) ^ rotr(x,25); }
static inline uint32_t gam0(uint32_t x) { return rotr(x,7)  ^ rotr(x,18) ^ (x >> 3); }
static inline uint32_t gam1(uint32_t x) { return rotr(x,17) ^ rotr(x,19) ^ (x >> 10); }

static void sha256(const uint8_t* data, size_t len, uint8_t out[32]) {
    // 初始哈希值
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // 消息填充：补 1 字节 0x80 + 若干 0x00 + 8 字节长度（位）
    size_t orig = len;
    size_t padded = ((len + 9 + 63) / 64) * 64; // 总填充后长度（64 的倍数）
    std::vector<uint8_t> buf(padded, 0); // 按填充后长度动态分配，长消息不再越界
    memcpy(buf.data(), data, len);
    buf[len] = 0x80;
    // 末尾 8 字节存原始位长度（大端）
    uint64_t bitlen = (uint64_t)orig * 8;
    for (int i = 0; i < 8; i++) {
        buf[padded - 1 - i] = (uint8_t)(bitlen >> (i * 8));
    }

    // 逐块处理
    for (size_t offset = 0; offset < padded; offset += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)buf[offset + i*4] << 24) |
                    ((uint32_t)buf[offset + i*4 + 1] << 16) |
                    ((uint32_t)buf[offset + i*4 + 2] << 8) |
                    ((uint32_t)buf[offset + i*4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            w[i] = gam1(w[i-2]) + w[i-7] + gam0(w[i-15]) + w[i-16];
        }

        uint32_t a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], h7=h[7];
        for (int i = 0; i < 64; i++) {
            uint32_t t1 = h7 + sig1(e) + ch(e,f,g) + SHA256_K[i] + w[i];
            uint32_t t2 = sig0(a) + maj(a,b,c);
            h7=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=h7;
    }

    // 输出 32 字节
    for (int i = 0; i < 8; i++) {
        out[i*4]   = (uint8_t)(h[i] >> 24);
        out[i*4+1] = (uint8_t)(h[i] >> 16);
        out[i*4+2] = (uint8_t)(h[i] >> 8);
        out[i*4+3] = (uint8_t)(h[i]);
    }
}

// 对外接口：输入字符串，返回 64 字符十六进制哈希
static string sha256(const string& input) {
    uint8_t hash[32];
    sha256((const uint8_t*)input.data(), input.size(), hash);

    const char* hex = "0123456789abcdef";
    string result;
    result.reserve(64);
    for (int i = 0; i < 32; i++) {
        result.push_back(hex[(hash[i] >> 4) & 0xF]);
        result.push_back(hex[hash[i] & 0xF]);
    }
    return result;
}
