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

static int fc = 0;

void test_u8_to_u32() {
    ASSERT_EQ(tu_u8c_to_u32c_1('a'), 'a');
    char u2[2] = "¥";
    ASSERT_EQ(tu_u8c_to_u32c_2(u2[0], u2[1]), 0xA5);
    char u3[3] = "爱";
    ASSERT_EQ(tu_u8c_to_u32c_3(u3[0], u3[1], u3[2]), 0x7231);
    char u4[4] = "\U0001D320";
    ASSERT_EQ(tu_u8c_to_u32c_4(u4[0], u4[1], u4[2], u4[3]), 0x1D320);

    char     ss[13] = "V©中国\U0001D356";
    u32_char u32[6];
    int      len    = 6;
    tu_index ei     = tu_utf8_to_utf32(ss, 13, u32, &len);
    ASSERT_EQ(ei, 13);
    ASSERT_EQ(len, 5);
    ASSERT_EQ(u32[0], 'V');
    ASSERT_EQ(u32[1], 0xA9);
    ASSERT_EQ(u32[2], 0x4E2D);
    ASSERT_EQ(u32[3], 0x56FD);
    ASSERT_EQ(u32[4], 0x1D356);
}

void test_u32_to_u8() {
    u8_char buf[4];
    int     len = tu_u32c_to_u8c('a', buf);
    ASSERT_EQ(len, 1);
    ASSERT_EQ(buf[0], 'a');

    char u2[2] = "¥";
    len = tu_u32c_to_u8c(0xA5, buf);
    ASSERT_EQ(len, 2);
    ASSERT_EQ(buf[0], u2[0]);
    ASSERT_EQ(buf[1], u2[1]);

    char u3[3] = "爱";
    len = tu_u32c_to_u8c(0x7231, buf);
    ASSERT_EQ(len, 3);
    for (int i = 0; i < 3; ++i) {
        ASSERT_EQ(buf[i], u3[i]);
    }

    char u4[4] = "\U0001D320";
    len = tu_u32c_to_u8c(0x1D320, buf);
    ASSERT_EQ(len, 4);
    for (int i = 0; i < 4; ++i) {
        ASSERT_EQ(buf[i], u4[i]);
    }

    char     ss[13] = "V©中国\U0001D356";
    u32_char u32[5] = {'V', 0xA9, 0x4E2D, 0x56FD, 0x1D356};
    u8_char  u8[50];
    len = 50;
    tu_index ei = tu_utf32_to_utf8(u32, 5, u8, &len);
    ASSERT_EQ(ei, 5);
    ASSERT_EQ(len, 13);
    for (int i = 0; i < 13; ++i) {
        ASSERT_EQ(u8[i], ss[i]);
    }

}

int main() {
    test_u8_to_u32();
    test_u32_to_u8();
    return fc;
}
