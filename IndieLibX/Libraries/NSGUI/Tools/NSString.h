#ifndef STRING_H

#define STRING_H
#ifdef USE_TINYXML
#include <string>
#define NSString std::string
#else
#ifdef _WIN32
#pragma warning( disable : 4996)
#endif

#include <cstring>

#include <cstdio>

#include "MathUtils.h"


class NSString

{

  public:
const char* c_str() const;

    NSString(const char* string = 0);

    NSString(const NSString &copy);

   ~NSString();



    operator const char*()    const ;

    const    char *getBytes() const;



    NSString &operator  =(const NSString &copy  );

    NSString &operator  =(const char   *string);



    NSString &operator +=(const NSString &copy);

    NSString  operator + (const NSString &copy);



    NSString &operator +=(const char character);

    NSString  operator + (const char character);



    NSString  operator + (const int    numberi);

    NSString  operator + (const float  numberf);



    NSString &operator +=(const int    numberi);

    NSString &operator +=(const float  numberf);



    NSString &addFloat(const float numberf, int precision);



    void set(const char* string, int count);

    void clear();

    //const unsigned int getLength() const;

    const unsigned int length() const {return m_length;}

public:
    unsigned int   m_length;

private:
    char *data;
};



bool    operator  < (const char   *arg1, const NSString &arg2);

bool    operator  < (const NSString &arg1, const char   *arg2);

bool    operator  < (const NSString &arg1, const NSString &arg2);

bool    operator  > (const char   *arg1, const NSString &arg2);

bool    operator  > (const NSString &arg1, const char   *arg2);

bool    operator  > (const NSString &arg1, const NSString &arg2);



bool   operator  ==(const NSString &arg1 , const NSString &arg2);

bool   operator  ==(const NSString &arg  , const char   *bytes);

bool   operator  ==(const char   *bytes, const NSString &arg);



bool   operator  !=(const NSString &arg1 , const NSString &arg2);

bool   operator  !=(const NSString &arg  , const char   *bytes);

bool   operator  !=(const char   *bytes, const NSString &arg);
#endif
#endif

