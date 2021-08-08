/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\String.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _STRING_H_
#define _STRING_H_

/** \file
	String utlity definition
*/

#include <stdio.h>
#include <string.h>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>

/** A string container */
class String {
public:
	String();
	String(const char *string);
	String(const char *string, const size_t len);
	String(const char *string0, const char *string1);
	String(const int count, ...);
	String(const String &string);
	~String();

	size_t getLength() const { return length; }
	bool isEmpty() const { return length == 0; }
	operator char *() const { return str; }
	operator const char *() const { return str; }

	void append(const char *string, const size_t len);
	void appendInt(const int aint);
	void appendInt64(const int64 aint64);
	void appendIntNatural(const int aint);
	void appendFloat(const float afloat, const int prec = 9);

	void operator = (const char *string);
	void operator = (const String &string);
	void operator += (const char *string);
	void operator += (const String &string);

	void terminateAtLast(const char terminator);
	void replace(const char oldChar, const char newChar);

	void clear();

private:
	char *str;
	size_t length;
};

String operator + (const String &string0, const String &string1);
String operator + (const String &string0, const char   *string1);
String operator + (const char   *string0, const String &string1);

bool operator == (const char   *string0, const String &string1);
bool operator == (const String &string0, const char   *string1);
bool operator == (const String &string0, const String &string1);
bool operator != (const char   *string0, const String &string1);
bool operator != (const String &string0, const char   *string1);
bool operator != (const String &string0, const String &string1);
bool operator  < (const char   *string0, const String &string1);
bool operator  < (const String &string0, const char   *string1);
bool operator  < (const String &string0, const String &string1);
bool operator  > (const char   *string0, const String &string1);
bool operator  > (const String &string0, const char   *string1);
bool operator  > (const String &string0, const String &string1);

#endif // _STRING_H_
