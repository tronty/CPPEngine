/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\String.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "String.h"
#include <stdarg.h>
#include <stdlib.h>

String::String(){
	length = 0;
	str = NULL;
}

String::String(const char *string){
	length = strlen(string);
	str = (char *) malloc(length + 1);
	strncpy(str, string, length + 1);
}

String::String(const char *string, const size_t len){
	length = len;
	str = (char *) malloc(len + 1);
	strncpy(str, string, len);
	str[len] = '\0';
}

String::String(const char *string0, const char *string1){
	size_t len0 = strlen(string0);
	size_t len1 = strlen(string1);
	length = len0 + len1;
	str = (char *) malloc(length + 1);
	strncpy(str, string0, len0);
	strncpy(str + len0, string1, len1);
	str[length] = '\0';
}

String::String(const String &string){
	length = string.length;
	str = (char *) malloc(length + 1);
	strncpy(str, string.str, length + 1);
}

String::String(const int count, ...){
	length = 0;
	str = NULL;

	va_list marker;
	va_start(marker, count);

	// Loop through all strings passed and append them
	for (int i = 0; i < count; i++){
		char *string = va_arg(marker, char *);
		append(string, strlen(string));
	}

	va_end(marker);
}

String::~String(){
	free(str);
}

void String::append(const char *string, const size_t len){
	str = (char *) realloc(str, length + len + 1);
	strncpy(str + length, string, len);
	length += len;
	str[length] = '\0';
}

void String::appendInt(const int aint){
	char s[16];
	append(s, sprintf(s, "%d", aint));
}

void String::appendInt64(const int64 aint64){
	char s[32];
	append(s, sprintf(s, "%Ld", aint64));
}

void String::appendIntNatural(const int aint){
	char s[32];
	s[31] = '\0';
	int p = 31;
	int k = 0;

	int n = abs(aint);
	do {
		if (k == 3){
			s[--p] = ',';
			k = 0;
		}
		k++;

		int digit = (n % 10);
		n /= 10;
		s[--p] = '0' + digit;
	} while (n);

	if (aint < 0) s[--p] = '-';

	append(s + p, 31 - p);
}

void String::appendFloat(const float afloat, const int prec){
	// Make a format string
	char format[5] = "%.0f";
	format[2] = '0' + prec;

	char s[16];
	append(s, sprintf(s, format, afloat));
}

void String::operator = (const char *string){
	length = strlen(string);
	str = (char *) realloc(str, length + 1);
	strncpy(str, string, length + 1);
}

void String::operator = (const String &string){
	length = string.length;
	str = (char *) realloc(str, length + 1);
	strncpy(str, string.str, length + 1);
}

void String::operator += (const char *string){
	append(string, strlen(string));
}

void String::operator += (const String &string){
	append(string.str, string.length);
}

void String::terminateAtLast(const char terminator){
	for (size_t i = length - 1; i >= 0; i--){
		if (str[i] == terminator){
			str[i] = '\0';
            return;
		}
	}
}

void String::replace(const char oldChar, const char newChar){
	for (uint i = 0; i < length; i++){
		if (str[i] == oldChar){
			str[i] = newChar;
		}
	}
}

void String::clear(){
	delete str;
	str = NULL;
	length = 0;
}

String operator + (const String &string0, const String &string1){
	return String(string0, string1);
}

String operator + (const String &string0, const char *string1){
	return String(string0, string1);
}

String operator + (const char *string0, const String &string1){
	return String(string0, string1);
}

bool operator == (const char   *string0, const String &string1){ return (strcmp(string0, string1) == 0); }
bool operator == (const String &string0, const char   *string1){ return (strcmp(string0, string1) == 0); }
bool operator == (const String &string0, const String &string1){ return (strcmp(string0, string1) == 0); }
bool operator != (const char   *string0, const String &string1){ return (strcmp(string0, string1) != 0); }
bool operator != (const String &string0, const char   *string1){ return (strcmp(string0, string1) != 0); }
bool operator != (const String &string0, const String &string1){ return (strcmp(string0, string1) != 0); }
bool operator  < (const char   *string0, const String &string1){ return (strcmp(string0, string1)  < 0); }
bool operator  < (const String &string0, const char   *string1){ return (strcmp(string0, string1)  < 0); }
bool operator  < (const String &string0, const String &string1){ return (strcmp(string0, string1)  < 0); }
bool operator  > (const char   *string0, const String &string1){ return (strcmp(string0, string1)  > 0); }
bool operator  > (const String &string0, const char   *string1){ return (strcmp(string0, string1)  > 0); }
bool operator  > (const String &string0, const String &string1){ return (strcmp(string0, string1)  > 0); }

