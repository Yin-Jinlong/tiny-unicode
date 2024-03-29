#include <tiny-unicode.h>

#define VM 0b111111

#define MB_MASK(c) ((uint8_t)c)>>6
#define MBV(c) (c&VM)

#define VH  0b10
#define VHM 0b10000000

#define BETWEEN(a, b) (c>=(0x##a) && c<=(0x##b))

int tu_is_full_width_char(char32_t c) {
    // 不完全符合，还可能有预留和空白
    return
        // 韩文字母 https://www.unicode.org/charts/PDF/U1100.pdf
        BETWEEN(1100, 11FF) ||
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
        // CJK 拓展A     https://www.unicode.org/charts/PDF/U3400.pdf
        // 易经符号 https://www.unicode.org/charts/PDF/U4DC0.pdf
        // CJK https://www.unicode.org/charts/PDF/U4E00.pdf
        BETWEEN(31A0, 9FFF) ||
        // 彝文音节 https://www.unicode.org/charts/PDF/UA000.pdf
        // 彝文部首 https://www.unicode.org/charts/PDF/UA490.pdf
        BETWEEN(A000, A4C6) ||
        // 韩文拼音 https://www.unicode.org/charts/PDF/UAC00.pdf
        BETWEEN(AC00, D7AF) ||
        // CJK 兼容 https://www.unicode.org/charts/PDF/UF900.pdf
        BETWEEN(F900, FAD9) ||
        // 中文竖版标点 https://www.unicode.org/charts/PDF/UFE10.pdf
        BETWEEN(FE10, FE19) ||
        // CJK 兼容符号 https://www.unicode.org/charts/PDF/UFE30.pdf
        BETWEEN(FE30, FE4F) ||
        // 半角的全角版本 https://www.unicode.org/charts/PDF/UFF00.pdf
        BETWEEN(FF01, FF5E) ||
        // 太玄经符号 https://www.unicode.org/charts/PDF/U1D300.pdf
        BETWEEN(1D300, 1D356) ||
        // CJK 拓展-B https://www.unicode.org/charts/PDF/U20000.pdf
        BETWEEN(20000, 2A6DF) ||
        // CJK 拓展-C https://www.unicode.org/charts/PDF/U2A700.pdf
        // CJK 拓展-D https://www.unicode.org/charts/PDF/U2B740.pdf
        // CJK 拓展-E https://www.unicode.org/charts/PDF/U2B820.pdf
        BETWEEN(2A700, 2CEA1) ||
        // CJK 拓展-F https://www.unicode.org/charts/PDF/U2CEB0.pdf
        BETWEEN(2CEB0, 2EBE0) ||
        // CJK 补充   https://www.unicode.org/charts/PDF/U2F800.pdf
        BETWEEN(2F800, 2FA1D) ||
        // CJK 拓展-G https://www.unicode.org/charts/PDF/U30000.pdf
        // CJK 拓展-H https://www.unicode.org/charts/PDF/U31350.pdf
        BETWEEN(30000, 323AF);
}

int tu_is_emoji(char32_t c) {
    return BETWEEN(1F000, 1FBF9);
}

#undef BETWEEN

int tu_get_byte_count(u8_char c) {
    if (c > 0)
        return 1;
    int bc = 0;
    while (c < 0) {
        bc++;
        c <<= 1;
    }
    return bc;
}

u32_char tu_u8c_to_u32c(const u8_char c[4], tu_count *byteCount) {
    char c0 = c[0];
    if (c[0] > 0) {
        if (byteCount)
            *byteCount = 1;
        return c[0];
    }
    int bc = 0;
    while (c0 < 0) {
        bc++;
        c0 <<= 1;
    }
    if (byteCount)
        *byteCount = bc;
    switch (bc) {
        case 2:
            return MB_MASK(c[1]) == VH ?
                   ((c[0] & 0b11111) << 6) | MBV(c[1]) : 0;
        case 3:
            return (MB_MASK(c[1]) & MB_MASK(c[2])) == VH ?
                   ((c[0] & 0b1111) << 12) |
                   (MBV(c[1]) << 6) |
                   MBV(c[2]) : 0;
        case 4:
            return (MB_MASK(c[1]) & MB_MASK(c[2]) & MB_MASK(c[3])) == VH ?
                   ((c[0] & 0b111) << 18) |
                   (MBV(c[1]) << 12) |
                   (MBV(c[2]) << 6) |
                   MBV(c[3]) : 0;
        default:
            return 0;
    }
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
        int      byteCount;
        char32_t r = tu_u8c_to_u32c(&utf8[u8I], &byteCount);
        if (!r)
            return u8I;
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
    if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
        return 0;
    if (c <= 0x7F) {
        buf[0] = (u8_char) c;
        return 1;
    }
    if (c <= 0x800) {
        buf[0] = (u8_char) ((c >> 6) | 0b11000000);
        buf[1] = (u8_char) ((c & VM) | VHM);
        return 2;
    }
    if (c <= 0xFFFF) {
        buf[0] = (u8_char) ((c >> 12) | 0b11100000);
        buf[1] = (u8_char) (((c >> 6) & VM) | VHM);
        buf[2] = (u8_char) ((c & VM) | VHM);
        return 3;
    }
    buf[0] = (u8_char) ((c >> 18) | 0b11110000);
    buf[1] = (u8_char) (((c >> 12) & VM) | VHM);
    buf[2] = (u8_char) (((c >> 6) & VM) | VHM);
    buf[3] = (u8_char) ((c & VM) | VHM);
    return 4;
}

int tu_utf32_to_utf8(const u32_char *utf32, tu_count utf32Len, u8_char *utf8, tu_count *bufLen) {
    if (!utf32 || !utf8 || !bufLen)
        return INVALID_BUFFER;
    int bufSize = *bufLen;
    if (bufSize < utf32Len)
        return BUFFER_TOO_SMALL;
    int     u32I = 0, u8I = 0;
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
