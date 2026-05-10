#include "string.h"

// counts characters until null terminator
int myStrLen(const char* s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

// copies src into dst safely
void myStrCopy(char* dst, const char* src, int maxLen)
{
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0')
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

// true if both char arrays are identical
bool myStrEq(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

// lowercase a single character manually
char myToLower(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}

// case-insensitive compare
bool myStrEqCI(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (myToLower(a[i]) != myToLower(b[i]))
            return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

// appends src onto dst up to maxLen total
void myStrCat(char* dst, const char* src, int maxLen)
{
    int dLen = myStrLen(dst);
    int i = 0;
    while (src[i] != '\0' && dLen + i < maxLen - 1)
    {
        dst[dLen + i] = src[i];
        i++;
    }
    dst[dLen + i] = '\0';
}

// converts integer n into a char array (buf must be big enough, 12 is safe)
void intToCharArr(int n, char* buf)
{
    if (n == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    // handle negative
    int start = 0;
    if (n < 0)
    {
        buf[0] = '-';
        start = 1;
        n = -n;
    }
    // count digits
    int temp = n, len = 0;
    while (temp > 0) { len++; temp /= 10; }
    buf[start + len] = '\0';
    for (int i = start + len - 1; i >= start; i--)
    {
        buf[i] = (n % 10) + '0';
        n /= 10;
    }
}

// converts float to string with 2 decimal places (buf must be ~20 chars)
void floatToCharArr(float f, char* buf)
{
    // handle negative
    int pos = 0;
    if (f < 0.0f)
    {
        buf[pos++] = '-';
        f = -f;
    }
    int intPart = (int)f;
    // round 2 decimal places
    int fracPart = (int)((f - intPart) * 100.0f + 0.5f);
    if (fracPart >= 100) { intPart++; fracPart = 0; }

    char tmp[16];
    intToCharArr(intPart, tmp);
    int i = 0;
    while (tmp[i] != '\0') buf[pos++] = tmp[i++];

    buf[pos++] = '.';
    buf[pos++] = '0' + (fracPart / 10);
    buf[pos++] = '0' + (fracPart % 10);
    buf[pos] = '\0';
}