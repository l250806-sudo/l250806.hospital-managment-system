#ifndef MYSTRING_H
#define MYSTRING_H

// returns length of a null-terminated char array (like strlen but mine)
int myStrLen(const char* s);

// copies src into dst, stops at maxLen-1 chars, always null-terminates
void myStrCopy(char* dst, const char* src, int maxLen);

// compares two char arrays, returns true if equal (like strcmp==0 but mine)
bool myStrEq(const char* a, const char* b);

// converts lowercase version of c (manual tolower)
char myToLower(char c);

// case-insensitive compare, returns true if equal
bool myStrEqCI(const char* a, const char* b);

// concatenates src onto end of dst (dst must have enough space)
void myStrCat(char* dst, const char* src, int maxLen);

// converts integer to char array, writes into buf (must be big enough)
void intToCharArr(int n, char* buf);

// converts float to char array with 2 decimal places
void floatToCharArr(float f, char* buf);

#endif