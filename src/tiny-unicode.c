#include <tiny-unicode.h>

#define MASK(c, x) ((uint8_t)c)>>x
#define MASK_4B(c) MASK(c,3)
#define MASK_3B(c) MASK(c,4)
#define MASK_2B(c) MASK(c,5)

#define MB_MASK(c) MASK(c,6)
#define MBV(c) (c&0b111111)

#define MH2 0b110
#define MH3 0b1110
#define MH4 0b11110
#define VH  0b10

u32_char tu_u8c_to_u32c_1(u8_char c) {
    // ASCII字符范围，字节由零开始
    // 000000 - 00007F
    // 128个代码
    // 00000000 00000000 0zzzzzzz
    // 0zzzzzzz（00-7F）
    // 七个z
    return c > 0 ? c : 0;
}

u32_char u8c_to_u32c_2(u8_char c1, u8_char c2) {
    return ((c1 & 0b11111) << 6) |
           MBV(c2);
}

u32_char tu_u8c_to_u32c_2(u8_char c1, u8_char c2) {
    // 第一个字节由110开始，接着的字节由10开始
    // 000080 - 0007FF
    // 1920个代码
    // 00000000 00000yyy yyzzzzzz
    // 110yyyyy（C0-DF） 10zzzzzz（80-BF）
    // 五个y；六个z
    return MASK_2B(c1) == MH2 ? u8c_to_u32c_2(c1, c2) : 0;
}

u32_char u8c_to_u32c_3(u8_char c1, u8_char c2, u8_char c3) {
    return (MB_MASK(c2) & MB_MASK(c3)) == VH ?
           ((c1 & 0b1111) << 12) |
           (MBV(c2) << 6) |
           MBV(c3) : 0;
}

u32_char tu_u8c_to_u32c_3(u8_char c1, u8_char c2, u8_char c3) {
    // 第一个字节由1110开始，接着的字节由10开始
    // 000800 - 00D7FF
    // 00E000 - 00FFFF
    // 61440个代码
    // 00000000 xxxxyyyy yyzzzzzz
    // 1110xxxx（E0-EF） 10yyyyyy 10zzzzzz
    // 四个x；六个y；六个z
    return MASK_3B(c1) == MH3 ? u8c_to_u32c_3(c1, c2, c3) : 0;
}

u32_char u8c_to_u32c_4(u8_char c1, u8_char c2, u8_char c3, u8_char c4) {
    return (MB_MASK(c2) & MB_MASK(c3) & MB_MASK(c4)) == VH ?
           ((c1 & 0b111) << 18) |
           (MBV(c2) << 12) |
           (MBV(c3) << 6) |
           MBV(c4) : 0;
}

u32_char tu_u8c_to_u32c_4(u8_char c1, u8_char c2, u8_char c3, u8_char c4) {
    // 第一个字节由11110开始，接着的字节由10开始
    // 010000 - 10FFFF
    // 1048576个代码
    // 000wwwxx xxxxyyyy yyzzzzzz
    // 11110www（F0-F7） 10xxxxxx 10yyyyyy 10zzzzzz
    // 三个w；六个x；六个y；六个z
    return MASK_4B(c1) == MH4 ? u8c_to_u32c_4(c1, c2, c3, c4) : 0;
}

inline int tu_get_byte_count(char c1) {
    if (c1>0)
        return 1;
    int c = 0;
    while (c1 < 0) {
        c++;
        c1<<= 1;
    }
    return c;
}

tu_index tu_utf8_to_utf32(const u8_char *utf8, tu_count utf8Len, u32_char *utf32, tu_count *bufLen) {
    if (!utf8 || !utf32 || !bufLen)
        return -1;
    int bufSize = *bufLen;
    if (bufSize < utf8Len / 3)
        return -2;
    int u8I  = 0;
    int u32I = 0;
    while (u8I < utf8Len) {
        u32_char r;
        u8_char  c1        = utf8[u8I];
        int      byteCount = tu_get_byte_count(c1);
        switch (byteCount) {
            case 1:
                r = tu_u8c_to_u32c_1(c1);
                if (r)
                    break;
            case 2:
                r = tu_u8c_to_u32c_2(c1, utf8[u8I + 1]);
                if (r)
                    break;
            case 3:
                r = tu_u8c_to_u32c_3(c1, utf8[u8I + 1], utf8[u8I + 2]);
                if (r)
                    break;
            case 4:
                r = tu_u8c_to_u32c_4(c1, utf8[u8I + 1], utf8[u8I + 2], utf8[u8I + 3]);
                if (r)
                    break;
            default:
                return u8I;
        }
        utf32[u32I++] = r;
        u8I += byteCount;
        if (u32I >= bufSize)
            return -2;
    }
    if (u32I >= bufSize)
        return -2;
    utf32[u32I] = 0;
    *bufLen = u32I;
    return u8I;
}
