/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <stdarg.h>
#include <stddef.h>

#define STXEXTERN
#define STXDECLSPEC
#define STXCALL
#define STXAPI
#define STXAPIENTRY

STXEXTERN STXDECLSPEC char *STXCALL stx_ltoa(int value, char *str, int radix);
STXEXTERN STXDECLSPEC char *STXCALL stx_ultoa(unsigned int value, char *str, int radix);

#ifdef __cplusplus
#define stx_reinterpret_cast(type, expression) reinterpret_cast<type>(expression)
#define stx_static_cast(type, expression) static_cast<type>(expression)
#define stx_const_cast(type, expression) const_cast<type>(expression)
#else
#define stx_reinterpret_cast(type, expression) ((type)(expression))
#define stx_static_cast(type, expression) ((type)(expression))
#define stx_const_cast(type, expression) ((type)(expression))
#endif

#define stx_isupperhex(X)   (((X) >= 'A') && ((X) <= 'F'))
#define stx_islowerhex(X)   (((X) >= 'a') && ((X) <= 'f'))

#define UTF8_IsLeadByte(c) ((c) >= 0xC0 && (c) <= 0xF4)
#define UTF8_IsTrailingByte(c) ((c) >= 0x80 && (c) <= 0xBF)

#define stx_zero(x) stx_memset(&(x), 0, sizeof((x)))

#ifdef HAVE_CTYPE_H
int stx_isdigit(int x) { return isdigit(x); }
int stx_isspace(int x) { return isspace(x); }
int stx_toupper(int x) { return toupper(x); }
int stx_tolower(int x) { return tolower(x); }
#else
int stx_isdigit(int x) { return ((x) >= '0') && ((x) <= '9'); }
int stx_isspace(int x) { return ((x) == ' ') || ((x) == '\t') || ((x) == '\r') || ((x) == '\n') || ((x) == '\f') || ((x) == '\v'); }
int stx_toupper(int x) { return ((x) >= 'a') && ((x) <= 'z') ? ('A'+((x)-'a')) : (x); }
int stx_tolower(int x) { return ((x) >= 'A') && ((x) <= 'Z') ? ('a'+((x)-'A')) : (x); }
#endif

static int UTF8_TrailingBytes(unsigned char c)
{
    if (c >= 0xC0 && c <= 0xDF)
        { return 1; }
    else if (c >= 0xE0 && c <= 0xEF)
        { return 2; }
    else if (c >= 0xF0 && c <= 0xF4)
        { return 3; }
    else
        { return 0; }
}

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOL)
static size_t
stx_ScanLong(const char *text, int radix, int *valuep)
{
    const char *textstart = text;
    int value = 0;
    stx_bool negative = stx_FALSE;

    if (*text == '-') {
        negative = stx_TRUE;
        ++text;
    }
    if (radix == 16 && stx_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (stx_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && stx_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && stx_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOUL) || !defined(HAVE_STRTOD)
static size_t
stx_ScanUnsignedLong(const char *text, int radix, unsigned int *valuep)
{
    const char *textstart = text;
    unsigned int value = 0;

    if (radix == 16 && stx_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (stx_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && stx_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && stx_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif

#ifndef HAVE_VSSCANF
static size_t
stx_ScanUintPtrT(const char *text, int radix, uintptr_t * valuep)
{
    const char *textstart = text;
    uintptr_t value = 0;

    if (radix == 16 && stx_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (stx_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && stx_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && stx_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif
#if HAVE_64
#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOLL)
static size_t
stx_ScanLongLong(const char *text, int radix, Sint64 * valuep)
{
    const char *textstart = text;
    Sint64 value = 0;
    stx_bool negative = stx_FALSE;

    if (*text == '-') {
        negative = stx_TRUE;
        ++text;
    }
    if (radix == 16 && stx_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (stx_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && stx_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && stx_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOULL)
static size_t
stx_ScanUnsignedLongLong(const char *text, int radix, Uint64 * valuep)
{
    const char *textstart = text;
    Uint64 value = 0;

    if (radix == 16 && stx_strncmp(text, "0x", 2) == 0) {
        text += 2;
    }
    for (;;) {
        int v;
        if (stx_isdigit((unsigned char) *text)) {
            v = *text - '0';
        } else if (radix == 16 && stx_isupperhex(*text)) {
            v = 10 + (*text - 'A');
        } else if (radix == 16 && stx_islowerhex(*text)) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if (valuep) {
        *valuep = value;
    }
    return (text - textstart);
}
#endif
#endif

#if !defined(HAVE_VSSCANF) || !defined(HAVE_STRTOD)
static size_t
stx_ScanFloat(const char *text, double *valuep)
{
    const char *textstart = text;
    unsigned int lvalue = 0;
    double value = 0.0;
    stx_bool negative = stx_FALSE;

    if (*text == '-') {
        negative = stx_TRUE;
        ++text;
    }
    text += stx_ScanUnsignedLong(text, 10, &lvalue);
    value += lvalue;
    if (*text == '.') {
        int mult = 10;
        ++text;
        while (stx_isdigit((unsigned char) *text)) {
            lvalue = *text - '0';
            value += (double) lvalue / mult;
            mult *= 10;
            ++text;
        }
    }
    if (valuep) {
        if (negative && value) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}
#endif
#if 0
void *
stx_memset(void *dst, int c, size_t len)
{
#if  defined(HAVE_MEMSET)
    return stx_memset(dst, c, len);
#else
    size_t left = (len % 4);
    Uint32 *dstp4;
    Uint8 *dstp1;
    Uint32 value4 = (c | (c << 8) | (c << 16) | (c << 24));
    Uint8 value1 = (Uint8) c;

    dstp4 = (Uint32 *) dst;
    len /= 4;
    while (len--) {
        *dstp4++ = value4;
    }

    dstp1 = (Uint8 *) dstp4;
    switch (left) {
    case 3:
        *dstp1++ = value1;
    case 2:
        *dstp1++ = value1;
    case 1:
        *dstp1++ = value1;
    }

    return dst;
#endif /* HAVE_MEMSET */
}

void *
stx_memcpy(void *dst, const void *src, size_t len)
{
#ifdef __GNUC__
    /* Presumably this is well tuned for speed.
       On my machine this is twice as fast as the C code below.
     */
    return __builtin_memcpy(dst, src, len);
#elif defined(HAVE_MEMCPY)
    return stx_memcpy(dst, src, len);
#elif defined(HAVE_BCOPY)
    bcopy(src, dst, len);
    return dst;
#else
    /* GCC 4.9.0 with -O3 will generate movaps instructions with the loop
       using Uint32* pointers, so we need to make sure the pointers are
       aligned before we loop using them.
     */
    if (((intptr_t)src & 0x3) || ((intptr_t)dst & 0x3)) {
        /* Do an unaligned byte copy */
        Uint8 *srcp1 = (Uint8 *)src;
        Uint8 *dstp1 = (Uint8 *)dst;

        while (len--) {
            *dstp1++ = *srcp1++;
        }
    } else {
        size_t left = (len % 4);
        Uint32 *srcp4, *dstp4;
        Uint8 *srcp1, *dstp1;

        srcp4 = (Uint32 *) src;
        dstp4 = (Uint32 *) dst;
        len /= 4;
        while (len--) {
            *dstp4++ = *srcp4++;
        }

        srcp1 = (Uint8 *) srcp4;
        dstp1 = (Uint8 *) dstp4;
        switch (left) {
        case 3:
            *dstp1++ = *srcp1++;
        case 2:
            *dstp1++ = *srcp1++;
        case 1:
            *dstp1++ = *srcp1++;
        }
    }
    return dst;
#endif /* __GNUC__ */
}

void *
stx_memmove(void *dst, const void *src, size_t len)
{
#if  defined(HAVE_MEMMOVE)
    return stx_memmove(dst, src, len);
#else
    char *srcp = (char *) src;
    char *dstp = (char *) dst;

    if (src < dst) {
        srcp += len - 1;
        dstp += len - 1;
        while (len--) {
            *dstp-- = *srcp--;
        }
    } else {
        while (len--) {
            *dstp++ = *srcp++;
        }
    }
    return dst;
#endif /* HAVE_MEMMOVE */
}

int
stx_memcmp(const void *s1, const void *s2, size_t len)
{
#if  defined(HAVE_MEMCMP)
    return stx_memcmp(s1, s2, len);
#else
    char *s1p = (char *) s1;
    char *s2p = (char *) s2;
    while (len--) {
        if (*s1p != *s2p) {
            return (*s1p - *s2p);
        }
        ++s1p;
        ++s2p;
    }
    return 0;
#endif /* HAVE_MEMCMP */
}
#endif
size_t
stx_strlen(const char *string)
{
#if  defined(HAVE_STRLEN)
    return strlen(string);
#else
    size_t len = 0;
    while (*string++) {
        ++len;
    }
    return len;
#endif /* HAVE_STRLEN */
}

size_t
stx_wcslen(const wchar_t * string)
{
#if  defined(HAVE_WCSLEN)
    return wcslen(string);
#else
    size_t len = 0;
    while (*string++) {
        ++len;
    }
    return len;
#endif /* HAVE_WCSLEN */
}

size_t
stx_wcslcpy(wchar_t *dst, const wchar_t *src, size_t maxlen)
{
#if  defined(HAVE_WCSLCPY)
    return wcslcpy(dst, src, maxlen);
#else
    size_t srclen = stx_wcslen(src);
    if (maxlen > 0) {
        size_t len = stx_min(srclen, maxlen - 1);
        stx_memcpy(dst, src, len * sizeof(wchar_t));
        dst[len] = '\0';
    }
    return srclen;
#endif /* HAVE_WCSLCPY */
}

size_t
stx_wcslcat(wchar_t *dst, const wchar_t *src, size_t maxlen)
{
#if  defined(HAVE_WCSLCAT)
    return wcslcat(dst, src, maxlen);
#else
    size_t dstlen = stx_wcslen(dst);
    size_t srclen = stx_wcslen(src);
    if (dstlen < maxlen) {
        stx_wcslcpy(dst + dstlen, src, maxlen - dstlen);
    }
    return dstlen + srclen;
#endif /* HAVE_WCSLCAT */
}

size_t
stx_strlcpy(char *dst, const char *src, size_t maxlen)
{
#if  defined(HAVE_STRLCPY)
    return strlcpy(dst, src, maxlen);
#else
    size_t srclen = stx_strlen(src);
    if (maxlen > 0) {
        size_t len = stx_min(srclen, maxlen - 1);
        stx_memcpy(dst, src, len);
        dst[len] = '\0';
    }
    return srclen;
#endif /* HAVE_STRLCPY */
}

size_t stx_utf8strlcpy(char *dst, const char *src, size_t dst_bytes)
{
    size_t src_bytes = stx_strlen(src);
    size_t bytes = stx_min(src_bytes, dst_bytes - 1);
    size_t i = 0;
    char trailing_bytes = 0;
    if (bytes)
    {
        unsigned char c = (unsigned char)src[bytes - 1];
        if (UTF8_IsLeadByte(c))
            --bytes;
        else if (UTF8_IsTrailingByte(c))
        {
            for (i = bytes - 1; i != 0; --i)
            {
                c = (unsigned char)src[i];
                trailing_bytes = UTF8_TrailingBytes(c);
                if (trailing_bytes)
                {
                    if (bytes - i != trailing_bytes + 1)
                        bytes = i;

                    break;
                }
            }
        }
        stx_memcpy(dst, src, bytes);
    }
    dst[bytes] = '\0';
    return bytes;
}

size_t
stx_strlcat(char *dst, const char *src, size_t maxlen)
{
#if  defined(HAVE_STRLCAT)
    return strlcat(dst, src, maxlen);
#else
    size_t dstlen = stx_strlen(dst);
    size_t srclen = stx_strlen(src);
    if (dstlen < maxlen) {
        stx_strlcpy(dst + dstlen, src, maxlen - dstlen);
    }
    return dstlen + srclen;
#endif /* HAVE_STRLCAT */
}

char *
stx_strdup(const char *string)
{
#if  defined(HAVE_STRDUP)
    return strdup(string);
#else
    size_t len = stx_strlen(string) + 1;
    char *newstr = (char *)stx_malloc(len);
    if (newstr) {
        stx_strlcpy(newstr, string, len);
    }
    return newstr;
#endif /* HAVE_STRDUP */
}

char *
stx_strrev(char *string)
{
#if  defined(HAVE__STRREV)
    return _strrev(string);
#else
    size_t len = stx_strlen(string);
    char *a = &string[0];
    char *b = &string[len - 1];
    len /= 2;
    while (len--) {
        char c = *a;
        *a++ = *b;
        *b-- = c;
    }
    return string;
#endif /* HAVE__STRREV */
}

char *
stx_strupr(char *string)
{
#if  defined(HAVE__STRUPR)
    return _strupr(string);
#else
    char *bufp = string;
    while (*bufp) {
        *bufp = stx_toupper((unsigned char) *bufp);
        ++bufp;
    }
    return string;
#endif /* HAVE__STRUPR */
}

char *
stx_strlwr(char *string)
{
#if  defined(HAVE__STRLWR)
    return _strlwr(string);
#else
    char *bufp = string;
    while (*bufp) {
        *bufp = stx_tolower((unsigned char) *bufp);
        ++bufp;
    }
    return string;
#endif /* HAVE__STRLWR */
}

char *
stx_strchr(const char *string, int c)
{
#ifdef HAVE_STRCHR
    return stx_const_cast(char*,strchr(string, c));
#elif defined(HAVE_INDEX)
    return stx_const_cast(char*,index(string, c));
#else
    while (*string) {
        if (*string == c) {
            return (char *) string;
        }
        ++string;
    }
    return NULL;
#endif /* HAVE_STRCHR */
}

char *
stx_strrchr(const char *string, int c)
{
#ifdef HAVE_STRRCHR
    return stx_const_cast(char*,strrchr(string, c));
#elif defined(HAVE_RINDEX)
    return stx_const_cast(char*,rindex(string, c));
#else
    const char *bufp = string + stx_strlen(string) - 1;
    while (bufp >= string) {
        if (*bufp == c) {
            return (char *) bufp;
        }
        --bufp;
    }
    return NULL;
#endif /* HAVE_STRRCHR */
}

char *
stx_strstr(const char *haystack, const char *needle)
{
#if  defined(HAVE_STRSTR)
    return stx_const_cast(char*,strstr(haystack, needle));
#else
    size_t length = stx_strlen(needle);
    while (*haystack) {
        if (stx_strncmp(haystack, needle, length) == 0) {
            return (char *) haystack;
        }
        ++haystack;
    }
    return NULL;
#endif /* HAVE_STRSTR */
}

#if !defined(HAVE__LTOA) || !defined(HAVE__I64TOA) || \
    !defined(HAVE__ULTOA) || !defined(HAVE__UI64TOA)
static const char ntoa_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};
#endif /* ntoa() conversion table */

char *
stx_itoa(int value, char *string, int radix)
{
#ifdef HAVE_ITOA
    return itoa(value, string, radix);
#else
    return stx_ltoa((int)value, string, radix);
#endif /* HAVE_ITOA */
}

char *
stx_uitoa(unsigned int value, char *string, int radix)
{
#ifdef HAVE__UITOA
    return _uitoa(value, string, radix);
#else
    return stx_ultoa((unsigned int)value, string, radix);
#endif /* HAVE__UITOA */
}

char *
stx_ltoa(int value, char *string, int radix)
{
#if  defined(HAVE__LTOA)
    return _ltoa(value, string, radix);
#else
    char *bufp = string;

    if (value < 0) {
        *bufp++ = '-';
        stx_ultoa(-value, bufp, radix);
    } else {
        stx_ultoa(value, bufp, radix);
    }

    return string;
#endif /* HAVE__LTOA */
}

char *
stx_ultoa(unsigned int value, char *string, int radix)
{
#if  defined(HAVE__ULTOA)
    return _ultoa(value, string, radix);
#else
    char *bufp = string;

    if (value) {
        while (value > 0) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    stx_strrev(string);

    return string;
#endif /* HAVE__ULTOA */
}
#if HAVE_64
char *
stx_lltoa(Sint64 value, char *string, int radix)
{
#if  defined(HAVE__I64TOA)
    return _i64toa(value, string, radix);
#else
    char *bufp = string;

    if (value < 0) {
        *bufp++ = '-';
        stx_ulltoa(-value, bufp, radix);
    } else {
        stx_ulltoa(value, bufp, radix);
    }

    return string;
#endif /* HAVE__I64TOA */
}

char *
stx_ulltoa(Uint64 value, char *string, int radix)
{
#if  defined(HAVE__UI64TOA)
    return _ui64toa(value, string, radix);
#else
    char *bufp = string;

    if (value) {
        while (value > 0) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    stx_strrev(string);

    return string;
#endif /* HAVE__UI64TOA */
}
#endif

int stx_atoi(const char *string)
{
#ifdef HAVE_ATOI
    return atoi(string);
#else
    return stx_strtol(string, NULL, 0);
#endif /* HAVE_ATOI */
}

double stx_atof(const char *string)
{
#ifdef HAVE_ATOF
    return (double) atof(string);
#else
    return stx_strtod(string, NULL);
#endif /* HAVE_ATOF */
}

int
stx_strtol(const char *string, char **endp, int base)
{
#if  defined(HAVE_STRTOL)
    return strtol(string, endp, base);
#else
    size_t len;
    int value;

    if (!base) {
        if ((stx_strlen(string) > 2) && (stx_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = stx_ScanLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOL */
}

unsigned int
stx_strtoul(const char *string, char **endp, int base)
{
#if  defined(HAVE_STRTOUL)
    return strtoul(string, endp, base);
#else
    size_t len;
    unsigned int value;

    if (!base) {
        if ((stx_strlen(string) > 2) && (stx_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = stx_ScanUnsignedLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOUL */
}
#if HAVE_64
Sint64
stx_strtoll(const char *string, char **endp, int base)
{
#if  defined(HAVE_STRTOLL)
    return strtoll(string, endp, base);
#else
    size_t len;
    Sint64 value;

    if (!base) {
        if ((stx_strlen(string) > 2) && (stx_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = stx_ScanLongLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOLL */
}

Uint64
stx_strtoull(const char *string, char **endp, int base)
{
#if  defined(HAVE_STRTOULL)
    return strtoull(string, endp, base);
#else
    size_t len;
    Uint64 value;

    if (!base) {
        if ((stx_strlen(string) > 2) && (stx_strncmp(string, "0x", 2) == 0)) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = stx_ScanUnsignedLongLong(string, base, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOULL */
}
#endif

double
stx_strtod(const char *string, char **endp)
{
#if  defined(HAVE_STRTOD)
    return strtod(string, endp);
#else
    size_t len;
    double value;

    len = stx_ScanFloat(string, &value);
    if (endp) {
        *endp = (char *) string + len;
    }
    return value;
#endif /* HAVE_STRTOD */
}

int
stx_strcmp(const char *str1, const char *str2)
{
#if  defined(HAVE_STRCMP)
    return strcmp(str1, str2);
#else
    while (*str1 && *str2) {
        if (*str1 != *str2)
            break;
        ++str1;
        ++str2;
    }
    return (int) ((unsigned char) *str1 - (unsigned char) *str2);
#endif /* HAVE_STRCMP */
}

int
stx_strncmp(const char *str1, const char *str2, size_t maxlen)
{
#if  defined(HAVE_STRNCMP)
    return strncmp(str1, str2, maxlen);
#else
    while (*str1 && *str2 && maxlen) {
        if (*str1 != *str2)
            break;
        ++str1;
        ++str2;
        --maxlen;
    }
    if (!maxlen) {
        return 0;
    }
    return (int) ((unsigned char) *str1 - (unsigned char) *str2);
#endif /* HAVE_STRNCMP */
}

int
stx_strcasecmp(const char *str1, const char *str2)
{
#ifdef HAVE_STRCASECMP
    return strcasecmp(str1, str2);
#elif defined(HAVE__STRICMP)
    return _stricmp(str1, str2);
#else
    char a = 0;
    char b = 0;
    while (*str1 && *str2) {
        a = stx_toupper((unsigned char) *str1);
        b = stx_toupper((unsigned char) *str2);
        if (a != b)
            break;
        ++str1;
        ++str2;
    }
    a = stx_toupper(*str1);
    b = stx_toupper(*str2);
    return (int) ((unsigned char) a - (unsigned char) b);
#endif /* HAVE_STRCASECMP */
}

int
stx_strncasecmp(const char *str1, const char *str2, size_t maxlen)
{
#ifdef HAVE_STRNCASECMP
    return strncasecmp(str1, str2, maxlen);
#elif defined(HAVE__STRNICMP)
    return _strnicmp(str1, str2, maxlen);
#else
    char a = 0;
    char b = 0;
    while (*str1 && *str2 && maxlen) {
        a = stx_tolower((unsigned char) *str1);
        b = stx_tolower((unsigned char) *str2);
        if (a != b)
            break;
        ++str1;
        ++str2;
        --maxlen;
    }
    if (maxlen == 0) {
        return 0;
    } else {
        a = stx_tolower((unsigned char) *str1);
        b = stx_tolower((unsigned char) *str2);
        return (int) ((unsigned char) a - (unsigned char) b);
    }
#endif /* HAVE_STRNCASECMP */
}

int
stx_sscanf(const char *text, const char *fmt, ...)
{
    int rc;
    va_list ap;
    va_start(ap, fmt);
    rc = stx_vsscanf(text, fmt, ap);
    va_end(ap);
    return rc;
}

#ifdef HAVE_VSSCANF
int
stx_vsscanf(const char *text, const char *fmt, va_list ap)
{
    return vsscanf(text, fmt, ap);
}
#else
int
stx_vsscanf(const char *text, const char *fmt, va_list ap)
{
    int retval = 0;

    while (*fmt) {
        if (*fmt == ' ') {
            while (stx_isspace((unsigned char) *text)) {
                ++text;
            }
            ++fmt;
            continue;
        }
        if (*fmt == '%') {
            stx_bool done = stx_FALSE;
            int count = 0;
            int radix = 10;
            enum
            {
                DO_SHORT,
                DO_INT,
                DO_LONG,
                //DO_LONGLONG
            } inttype = DO_INT;
            stx_bool suppress = stx_FALSE;

            ++fmt;
            if (*fmt == '%') {
                if (*text == '%') {
                    ++text;
                    ++fmt;
                    continue;
                }
                break;
            }
            if (*fmt == '*') {
                suppress = stx_TRUE;
                ++fmt;
            }
            fmt += stx_ScanLong(fmt, 10, &count);

            if (*fmt == 'c') {
                if (!count) {
                    count = 1;
                }
                if (suppress) {
                    while (count--) {
                        ++text;
                    }
                } else {
                    char *valuep = va_arg(ap, char *);
                    while (count--) {
                        *valuep++ = *text++;
                    }
                    ++retval;
                }
                continue;
            }

            while (stx_isspace((unsigned char) *text)) {
                ++text;
            }

            /* FIXME: implement more of the format specifiers */
            while (!done) {
                switch (*fmt) {
                case '*':
                    suppress = stx_TRUE;
                    break;
                case 'h':
                    if (inttype > DO_SHORT) {
                        //++inttype;
			switch (inttype) {
                            case DO_SHORT:
                                inttype=DO_INT;
                                break;
                            case DO_INT:
                                inttype=DO_LONG;
                                break;
                            case DO_LONG:
                                inttype=DO_SHORT;
                                break;
                       }
                    }
                    break;
                case 'l':
                    if (inttype < DO_LONG) {//DO_LONGLONG) {
                        //++inttype;
			switch (inttype) {
                            case DO_SHORT:
                                inttype=DO_INT;
                                break;
                            case DO_INT:
                                inttype=DO_LONG;
                                break;
                            case DO_LONG:
                                inttype=DO_SHORT;
                                break;
                       }
                    }
                    break;
                /*case 'I':
                    if (stx_strncmp(fmt, "I64", 3) == 0) {
                        fmt += 2;
                        inttype = DO_LONGLONG;
                    }
                    break;*/
                case 'i':
                    {
                        int index = 0;
                        if (text[index] == '-') {
                            ++index;
                        }
                        if (text[index] == '0') {
                            if (stx_tolower((unsigned char) text[index + 1]) == 'x') {
                                radix = 16;
                            } else {
                                radix = 8;
                            }
                        }
                    }
                    /* Fall through to %d handling */
                case 'd':
                    /* if (inttype == DO_LONGLONG) {
                        Sint64 value;
                        text += stx_ScanLongLong(text, radix, &value);
                        if (!suppress) {
                            Sint64 *valuep = va_arg(ap, Sint64 *);
                            *valuep = value;
                            ++retval;
                        }
                    } else */ {
                        int value;
                        text += stx_ScanLong(text, radix, &value);
                        if (!suppress) {
                            switch (inttype) {
                            case DO_SHORT:
                                {
                                    short *valuep = va_arg(ap, short *);
                                    *valuep = (short) value;
                                }
                                break;
                            case DO_INT:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = (int) value;
                                }
                                break;
                            case DO_LONG:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = value;
                                }
                                break;
                            //case DO_LONGLONG:
                                /* Handled above */
                               // break;
                            }
                            ++retval;
                        }
                    }
                    done = stx_TRUE;
                    break;
                case 'o':
                    if (radix == 10) {
                        radix = 8;
                    }
                    /* Fall through to unsigned handling */
                case 'x':
                case 'X':
                    if (radix == 10) {
                        radix = 16;
                    }
                    /* Fall through to unsigned handling */
                case 'u':
                    /* if (inttype == DO_LONGLONG) {
                        Uint64 value;
                        text += stx_ScanUnsignedLongLong(text, radix, &value);
                        if (!suppress) {
                            Uint64 *valuep = va_arg(ap, Uint64 *);
                            *valuep = value;
                            ++retval;
                        }
                    } else */ {
                        unsigned int value;
                        text += stx_ScanUnsignedLong(text, radix, &value);
                        if (!suppress) {
                            switch (inttype) {
                            case DO_SHORT:
                                {
                                    short *valuep = va_arg(ap, short *);
                                    *valuep = (short) value;
                                }
                                break;
                            case DO_INT:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = (int) value;
                                }
                                break;
                            case DO_LONG:
                                {
                                    int *valuep = va_arg(ap, int *);
                                    *valuep = value;
                                }
                                break;
                            //case DO_LONGLONG:
                                /* Handled above */
                               // break;
                            }
                            ++retval;
                        }
                    }
                    done = stx_TRUE;
                    break;
                case 'p':
                    {
                        uintptr_t value;
                        text += stx_ScanUintPtrT(text, 16, &value);
                        if (!suppress) {
                            void **valuep = va_arg(ap, void **);
                            *valuep = (void *) value;
                            ++retval;
                        }
                    }
                    done = stx_TRUE;
                    break;
                case 'f':
                    {
                        double value;
                        text += stx_ScanFloat(text, &value);
                        if (!suppress) {
                            float *valuep = va_arg(ap, float *);
                            *valuep = (float) value;
                            ++retval;
                        }
                    }
                    done = stx_TRUE;
                    break;
                case 's':
                    if (suppress) {
                        while (!stx_isspace((unsigned char) *text)) {
                            ++text;
                            if (count) {
                                if (--count == 0) {
                                    break;
                                }
                            }
                        }
                    } else {
                        char *valuep = va_arg(ap, char *);
                        while (!stx_isspace((unsigned char) *text)) {
                            *valuep++ = *text++;
                            if (count) {
                                if (--count == 0) {
                                    break;
                                }
                            }
                        }
                        *valuep = '\0';
                        ++retval;
                    }
                    done = stx_TRUE;
                    break;
                default:
                    done = stx_TRUE;
                    break;
                }
                ++fmt;
            }
            continue;
        }
        if (*text == *fmt) {
            ++text;
            ++fmt;
            continue;
        }
        /* Text didn't match format specifier */
        break;
    }

    return retval;
}
#endif /* HAVE_VSSCANF */

int
stx_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = stx_vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);

    return retval;
}

#ifdef HAVE_VSNPRINTF
int stx_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    return vsnprintf(text, maxlen, fmt, ap);
}
#else
 /* FIXME: implement more of the format specifiers */
typedef enum
{
    stx_CASE_NOCHANGE,
    stx_CASE_LOWER,
    stx_CASE_UPPER
} stx_letter_case;

typedef struct
{
    stx_bool left_justify;
    stx_bool force_sign;
    stx_bool force_type;
    stx_bool pad_zeroes;
    stx_letter_case force_case;
    int width;
    int radix;
    int precision;
} stx_FormatInfo;

static size_t
stx_PrintString(char *text, size_t maxlen, stx_FormatInfo *info, const char *string)
{
    size_t length = 0;

    if (info && info->width && (size_t)info->width > stx_strlen(string)) {
        char fill = info->pad_zeroes ? '0' : ' ';
        size_t width = info->width - stx_strlen(string);
        while (width-- > 0 && maxlen > 0) {
            *text++ = fill;
            ++length;
            --maxlen;
        }
    }

    length += stx_strlcpy(text, string, maxlen);

    if (info) {
        if (info->force_case == stx_CASE_LOWER) {
            stx_strlwr(text);
        } else if (info->force_case == stx_CASE_UPPER) {
            stx_strupr(text);
        }
    }
    return length;
}

static size_t
stx_PrintLong(char *text, size_t maxlen, stx_FormatInfo *info, int value)
{
    char num[130];

    stx_ltoa(value, num, info ? info->radix : 10);
    return stx_PrintString(text, maxlen, info, num);
}

static size_t
stx_PrintUnsignedLong(char *text, size_t maxlen, stx_FormatInfo *info, unsigned int value)
{
    char num[130];

    stx_ultoa(value, num, info ? info->radix : 10);
    return stx_PrintString(text, maxlen, info, num);
}
#if HAVE_64
static size_t
stx_PrintLongLong(char *text, size_t maxlen, stx_FormatInfo *info, Sint64 value)
{
    char num[130];

    stx_lltoa(value, num, info ? info->radix : 10);
    return stx_PrintString(text, maxlen, info, num);
}

static size_t
stx_PrintUnsignedLongLong(char *text, size_t maxlen, stx_FormatInfo *info, Uint64 value)
{
    char num[130];

    stx_ulltoa(value, num, info ? info->radix : 10);
    return stx_PrintString(text, maxlen, info, num);
}
#endif
static size_t
stx_PrintFloat(char *text, size_t maxlen, stx_FormatInfo *info, double arg)
{
    int width;
    size_t len;
    size_t left = maxlen;
    char *textstart = text;

    if (arg) {
        /* This isn't especially accurate, but hey, it's easy. :) */
        unsigned int value;

        if (arg < 0) {
            if (left > 1) {
                *text = '-';
                --left;
            }
            ++text;
            arg = -arg;
        } else if (info->force_sign) {
            if (left > 1) {
                *text = '+';
                --left;
            }
            ++text;
        }
        value = (unsigned int) arg;
        len = stx_PrintUnsignedLong(text, left, NULL, value);
        text += len;
        if (len >= left) {
            left = stx_min(left, 1);
        } else {
            left -= len;
        }
        arg -= value;
        if (info->precision < 0) {
            info->precision = 6;
        }
        if (info->force_type || info->precision > 0) {
            int mult = 10;
            if (left > 1) {
                *text = '.';
                --left;
            }
            ++text;
            while (info->precision-- > 0) {
                value = (unsigned int) (arg * mult);
                len = stx_PrintUnsignedLong(text, left, NULL, value);
                text += len;
                if (len >= left) {
                    left = stx_min(left, 1);
                } else {
                    left -= len;
                }
                arg -= (double) value / mult;
                mult *= 10;
            }
        }
    } else {
        if (left > 1) {
            *text = '0';
            --left;
        }
        ++text;
        if (info->force_type) {
            if (left > 1) {
                *text = '.';
                --left;
            }
            ++text;
        }
    }

    width = info->width - (int)(text - textstart);
    if (width > 0) {
        char fill = info->pad_zeroes ? '0' : ' ';
        char *end = text+left-1;
        len = (text - textstart);
        for (len = (text - textstart); len--; ) {
            if ((textstart+len+width) < end) {
                *(textstart+len+width) = *(textstart+len);
            }
        }
        len = (size_t)width;
        text += len;
        if (len >= left) {
            left = stx_min(left, 1);
        } else {
            left -= len;
        }
        while (len--) {
            if (textstart+len < end) {
                textstart[len] = fill;
            }
        }
    }

    return (text - textstart);
}

int
stx_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    size_t left = maxlen;
    char *textstart = text;

    while (*fmt) {
        if (*fmt == '%') {
            stx_bool done = stx_FALSE;
            size_t len = 0;
            stx_bool check_flag;
            stx_FormatInfo info;
            enum
            {
                DO_INT,
                DO_LONG//,
                //DO_LONGLONG
            } inttype = DO_INT;

            stx_zero(info);
            info.radix = 10;
            info.precision = -1;

            check_flag = stx_TRUE;
            while (check_flag) {
                ++fmt;
                switch (*fmt) {
                case '-':
                    info.left_justify = stx_TRUE;
                    break;
                case '+':
                    info.force_sign = stx_TRUE;
                    break;
                case '#':
                    info.force_type = stx_TRUE;
                    break;
                case '0':
                    info.pad_zeroes = stx_TRUE;
                    break;
                default:
                    check_flag = stx_FALSE;
                    break;
                }
            }

            if (*fmt >= '0' && *fmt <= '9') {
                info.width = stx_strtol(fmt, (char **)&fmt, 0);
            }

            if (*fmt == '.') {
                ++fmt;
                if (*fmt >= '0' && *fmt <= '9') {
                    info.precision = stx_strtol(fmt, (char **)&fmt, 0);
                } else {
                    info.precision = 0;
                }
            }

            while (!done) {
                switch (*fmt) {
                case '%':
                    if (left > 1) {
                        *text = '%';
                    }
                    len = 1;
                    done = stx_TRUE;
                    break;
                case 'c':
                    /* char is promoted to int when passed through (...) */
                    if (left > 1) {
                        *text = (char) va_arg(ap, int);
                    }
                    len = 1;
                    done = stx_TRUE;
                    break;
                case 'h':
                    /* short is promoted to int when passed through (...) */
                    break;
                case 'l':
                    if (inttype < DO_LONG) {//DO_LONGLONG) {
                        //++inttype;
			switch (inttype) {
                            case DO_INT:
                                inttype=DO_LONG;
                                break;
                            case DO_LONG:
                                inttype=DO_INT;
                                break;
                       }
                    }
                    break;
                /* case 'I':
                    if (stx_strncmp(fmt, "I64", 3) == 0) {
                        fmt += 2;
                        inttype = DO_LONGLONG;
                    }
                    break; */
                case 'i':
                case 'd':
                    switch (inttype) {
                    case DO_INT:
                        len = stx_PrintLong(text, left, &info,
                                            (int) va_arg(ap, int));
                        break;
                    case DO_LONG:
                        len = stx_PrintLong(text, left, &info,
                                            va_arg(ap, int));
                        break;
                    /* case DO_LONGLONG:
                        len = stx_PrintLongLong(text, left, &info,
                                                va_arg(ap, Sint64));
                        break; */
                    }
                    done = stx_TRUE;
                    break;
                case 'p':
                case 'x':
                    info.force_case = stx_CASE_LOWER;
                    /* Fall through to 'X' handling */
                case 'X':
                    if (info.force_case == stx_CASE_NOCHANGE) {
                        info.force_case = stx_CASE_UPPER;
                    }
                    if (info.radix == 10) {
                        info.radix = 16;
                    }
                    if (*fmt == 'p') {
                        inttype = DO_LONG;
                    }
                    /* Fall through to unsigned handling */
                case 'o':
                    if (info.radix == 10) {
                        info.radix = 8;
                    }
                    /* Fall through to unsigned handling */
                case 'u':
                    info.pad_zeroes = stx_TRUE;
                    switch (inttype) {
                    case DO_INT:
                        len = stx_PrintUnsignedLong(text, left, &info,
                                                    (unsigned int)
                                                    va_arg(ap, unsigned int));
                        break;
                    case DO_LONG:
                        len = stx_PrintUnsignedLong(text, left, &info,
                                                    va_arg(ap, unsigned int));
                        break;
                    /* case DO_LONGLONG:
                        len = stx_PrintUnsignedLongLong(text, left, &info,
                                                        va_arg(ap, Uint64));
                        break; */
                    }
                    done = stx_TRUE;
                    break;
                case 'f':
                    len = stx_PrintFloat(text, left, &info, va_arg(ap, double));
                    done = stx_TRUE;
                    break;
                case 's':
                    len = stx_PrintString(text, left, &info, va_arg(ap, char *));
                    done = stx_TRUE;
                    break;
                default:
                    done = stx_TRUE;
                    break;
                }
                ++fmt;
            }
            text += len;
            if (len >= left) {
                left = stx_min(left, 1);
            } else {
                left -= len;
            }
        } else {
            if (left > 1) {
                *text = *fmt;
                --left;
            }
            ++fmt;
            ++text;
        }
    }
    if (left > 0) {
        *text = '\0';
    }
    return (int)(text - textstart);
}
#endif /* HAVE_VSNPRINTF */

/* vi: set ts=4 sw=4 expandtab: */

int stx_abs(int x)
{
    return abs(x);
}

