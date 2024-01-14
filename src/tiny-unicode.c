#include <tiny-unicode.h>

#define MASK(c, x) ((uint8_t)c)>>x
#define MASK_4B(c) MASK(c,3)
#define MASK_3B(c) MASK(c,4)
#define MASK_2B(c) MASK(c,5)

#define VM 0b111111

#define MB_MASK(c) MASK(c,6)
#define MBV(c) (c&VM)

#define MH2 0b110
#define MH3 0b1110
#define MH4 0b11110
#define VH  0b10

#define BETWEEN(a, b) (c>=(0x##a) && c<=(0x##b))

int tu_is_full_width_char(char32_t c) {
    // 不完全符合，还可能有预留和空白
    return  // 韩文字母 https://www.unicode.org/charts/PDF/U1100.pdf
            BETWEEN(1100, 11FF) ||
            // 杂项 https://www.unicode.org/charts/PDF/U2600.pdf
            // 装饰 https://www.unicode.org/charts/PDF/U2700.pdf
            BETWEEN(2600, 27BF) ||
            // 盲文符号 https://www.unicode.org/charts/PDF/U2800.pdf
            BETWEEN(2800, 28FF) ||
            // CJK 偏旁部首 https://www.unicode.org/charts/PDF/U2E80.pdf
            // 康熙部首     https://www.unicode.org/charts/PDF/U2F00.pdf
            BETWEEN(2E80, 2FDF) ||
            // 汉字结构描述 https://www.unicode.org/charts/PDF/U2FF0.pdf
            // CJK 标点   http://www.unicode.org/charts/PDF/U3000.pdf
            // 平假       https://www.unicode.org/charts/PDF/U3040.pdf
            // 片假       https://www.unicode.org/charts/PDF/U30A0.pdf
            // 汉字注音符号 https://www.unicode.org/charts/PDF/U3100.pdf
            // 韩文兼容字母 https://www.unicode.org/charts/PDF/U3130.pdf
            BETWEEN(2FF0, 318E) ||
            // 汉字注音符号扩展 https://www.unicode.org/charts/PDF/U31A0.pdf
            // CJK 笔画      http://www.unicode.org/charts/PDF/U31C0.pdf
            // 日文片假扩展    https://www.unicode.org/charts/PDF/U31F0.pdf
            // CJK 字母月份   https://www.unicode.org/charts/PDF/U3200.pdf
            // 特殊符号       https://www.unicode.org/charts/PDF/U3300.pdf
            BETWEEN(31A0, 33FF) ||
            // 易经符号 https://www.unicode.org/charts/PDF/U4DC0.pdf
            // CJK https://www.unicode.org/charts/PDF/U4E00.pdf
            BETWEEN(4DC0, 9FFF) ||
            // 彝文音节 https://www.unicode.org/charts/PDF/UA000.pdf
            // 彝文部首 https://www.unicode.org/charts/PDF/UA490.pdf
            BETWEEN(A000, A4C6) ||
            // 韩文拼音 https://www.unicode.org/charts/PDF/UAC00.pdf
            BETWEEN(AC00, D7AF) ||
            // 中文竖版标点 https://www.unicode.org/charts/PDF/UFE10.pdf
            BETWEEN(FE10, FE19) ||
            // CJK 兼容符号 https://www.unicode.org/charts/PDF/UFE30.pdf
            BETWEEN(FE30, FE4F) ||
            // 半角的全角版本 https://www.unicode.org/charts/PDF/UFF00.pdf
            BETWEEN(FF01, FF5E) ||
            // 太玄经符号 https://www.unicode.org/charts/PDF/U1D300.pdf
            BETWEEN(1D300, 1D356);
}

#undef BETWEEN

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
    if (c1 > 0)
        return 1;
    int c = 0;
    while (c1 < 0) {
        c++;
        c1 <<= 1;
    }
    return c;
}

int tu_utf8_to_utf32(const u8_char *utf8, tu_count utf8Len, u32_char *utf32, tu_count *bufLen) {
    if (!utf8 || !utf32 || !bufLen)
        return INVALID_BUFFER;
    int bufSize = *bufLen;
    if (bufSize < utf8Len / 3)
        return BUFFER_TOO_SMALL;
    int u8I  = 0;
    int u32I = 0;
    while (u8I < utf8Len) {
        u32_char r;
        u8_char  c1        = utf8[u8I];
        int      byteCount = tu_get_byte_count(c1);
        switch (byteCount) {
            case 1:
                if ((r = tu_u8c_to_u32c_1(c1)))
                    break;
            case 2:
                if ((r = tu_u8c_to_u32c_2(c1, utf8[u8I + 1])))
                    break;
            case 3:
                if ((r = tu_u8c_to_u32c_3(c1, utf8[u8I + 1], utf8[u8I + 2])))
                    break;
            case 4:
                if ((r = tu_u8c_to_u32c_4(c1, utf8[u8I + 1], utf8[u8I + 2], utf8[u8I + 3])))
                    break;
            default:
                return u8I;
        }
        utf32[u32I++] = r;
        u8I += byteCount;
        if (u32I >= bufSize)
            return BUFFER_TOO_SMALL;
    }
    if (u32I >= bufSize)
        return BUFFER_TOO_SMALL;
    utf32[u32I] = 0;
    *bufLen = u32I;
    return u8I;
}

int tu_u32c_to_u8c(u32_char c, u8_char buf[4]) {
    if (c > 0x10FFFF || c <= 0 || (c >= 0xD800 && c <= 0xDFFF))
        return 0;
    if (c <= 0x7F) {
        buf[0] = (u8_char) c;
        return 1;
    }
    if (c <= 0x800) {
        buf[0] = (u8_char) ((c >> 6) | 0b11000000);
        buf[1] = (u8_char) ((c & VM) | 0b10000000);
        return 2;
    }
    if (c <= 0xFFFF) {
        buf[0] = (u8_char) ((c >> 12) | 0b11100000);
        buf[1] = (u8_char) (((c >> 6) & VM) | 0b10000000);
        buf[2] = (u8_char) ((c & VM) | 0b10000000);
        return 3;
    }
    buf[0] = (u8_char) ((c >> 18) | 0b11110000);
    buf[1] = (u8_char) (((c >> 12) & VM) | 0b10000000);
    buf[2] = (u8_char) (((c >> 6) & VM) | 0b10000000);
    buf[3] = (u8_char) ((c & VM) | 0b10000000);
    return 4;
}

int tu_utf32_to_utf8(const u32_char *utf32, tu_count utf32Len, u8_char *utf8, tu_count *bufLen) {
    if (!utf32 || !utf8 || !bufLen)
        return INVALID_BUFFER;
    int bufSize = *bufLen;
    if (bufSize < utf32Len)
        return BUFFER_TOO_SMALL;
    int     u32I = 0;
    int     u8I  = 0;
    u8_char u32s[4];
    while (u32I < utf32Len) {
        u32_char c = utf32[u32I];
        int      l = tu_u32c_to_u8c(c, u32s);
        if (!l)
            return u32I;
        if (u8I + l > bufSize)
            return BUFFER_TOO_SMALL;
        for (int i = 0; i < l; ++i) {
            utf8[u8I + i] = u32s[i];
        }
        u8I += l;
        u32I++;
    }
    if (u8I >= bufSize)
        return BUFFER_TOO_SMALL;
    utf8[u8I] = 0;
    *bufLen = u8I;
    return u32I;
}
