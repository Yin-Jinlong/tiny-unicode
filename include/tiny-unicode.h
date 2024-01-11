#ifndef TINY_UNICODE_TINY_UNICODE_H
#define TINY_UNICODE_TINY_UNICODE_H

#include <stdint.h>
#include <uchar.h>

#define TINY_UNICODE_DLL_OUT __declspec(dllexport)
#define TU_ARG_OUT
#define TU_ARG_IN
#define TU_ARG_IN_OUT

typedef int      tu_index;
typedef int32_t  tu_count;
typedef char     u8_char;
typedef char16_t u16_char;
typedef char32_t u32_char;

/**
 * utf8单字节字符转utf32字符
 * @param c utf8字符
 * @return utf32字符，0转换错误
 */
TINY_UNICODE_DLL_OUT u32_char tu_u8c_to_u32c_1(TU_ARG_IN u8_char c);

/**
 * utf8双字节字符转utf32字符
 * @param c1 utf8字符1
 * @param c2 utf8字符2
 * @return utf32字符，0转换错误
 */
TINY_UNICODE_DLL_OUT u32_char tu_u8c_to_u32c_2(TU_ARG_IN u8_char c1, TU_ARG_IN u8_char c2);

/**
 * utf8三字节字符转utf32字符
 * @param c1 utf8字符1
 * @param c2 utf8字符2
 * @param c3 utf8字符3
 * @return utf32字符，0转换错误
 */
TINY_UNICODE_DLL_OUT u32_char tu_u8c_to_u32c_3(
        TU_ARG_IN u8_char c1,
        TU_ARG_IN u8_char c2,
        TU_ARG_IN u8_char c3
);

/**
 * utf8四字节字符转utf32字符
 * @param c1 utf8字符1
 * @param c2 utf8字符2
 * @param c3 utf8字符3
 * @param c4 utf8字符4
 * @return utf32字符，0转换错误
 */
TINY_UNICODE_DLL_OUT u32_char tu_u8c_to_u32c_4(
        TU_ARG_IN u8_char c1,
        TU_ARG_IN u8_char c2,
        TU_ARG_IN u8_char c3,
        TU_ARG_IN u8_char c4
);

/**
 * utf8 to utf32，不包含结尾\0
 *
 * @param [in]     utf8    utf8字符串
 * @param [in]     utf8Len utf8字符串长度
 * @param [out]    utf32   utf32字符串
 * @param [in,out] bufLen  utf32缓冲区长度
 *
 * @return 已转换字符数，成功返回字符串长度，缓冲区NULL返回-1，缓冲区太小返回-2
 */
TINY_UNICODE_DLL_OUT tu_index tu_utf8_to_utf32(
        TU_ARG_IN     const u8_char *utf8,
        TU_ARG_IN           tu_count utf8Len,
        TU_ARG_OUT          u32_char *utf32,
        TU_ARG_IN_OUT       tu_count *bufLen
);

#endif //TINY_UNICODE_TINY_UNICODE_H
