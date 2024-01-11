#include <tiny-unicode.h>
#include <stdio.h>

#define ASSERT_EQ(a, b)  fc += assert_eq(a,b)

int assert_eq(u32_char a, u32_char b) {
    int r = (a == b);
    if (r)
        return 0;
    printf("assert_eq failed: %x!= %x\n", a, b);
    return 1;
}


int main() {
    int fc = 0;
    ASSERT_EQ(tu_u8c_to_u32c_1('a'), 'a');
    char u2[2] = "¥";
    ASSERT_EQ(tu_u8c_to_u32c_2(u2[0], u2[1]), 0xA5);
    char u3[3] = "爱";
    ASSERT_EQ(tu_u8c_to_u32c_3(u3[0], u3[1], u3[2]), 0x7231);
    char u4[4] = "\U0001D320";
    ASSERT_EQ(tu_u8c_to_u32c_4(u4[0], u4[1], u4[2],u4[3]), 0x1D320);

    char     ss[13] = "V©中国\U0001D356";
    u32_char u32[5];
    int len=6;
    tu_index ei     = tu_utf8_to_utf32(ss, 13, u32, &len);
    ASSERT_EQ(ei, 13);
    ASSERT_EQ(len, 5);
    ASSERT_EQ(u32[0], 'V');
    ASSERT_EQ(u32[1], 0xA9);
    ASSERT_EQ(u32[2], 0x4E2D);
    ASSERT_EQ(u32[3], 0x56FD);
    ASSERT_EQ(u32[4], 0x1D356);
    return fc;
}
