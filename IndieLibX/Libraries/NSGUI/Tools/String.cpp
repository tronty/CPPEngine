#include <STX/STX.h>
#ifndef USE_TINYXML
#include "NSString.h"

const char* NSString::c_str()
{
	static char* buf2=0;
	delete[] buf2;
	buf2=new char[m_length+1];
	stx_memcpy(buf2,data,m_length);
	buf2[m_length]='\0';
	return buf2;
}

NSString::NSString(const char* string)

{

  m_length = 0;

  data   = 0;

  operator=(string);

}



NSString::NSString(const NSString &copy)

{

  data   = 0;

  m_length = 0;

  operator=(copy);

}



NSString &NSString::operator  =(const NSString &copy)

{

  if(this != &copy)

  {

    clear();

    if(copy.data && copy.m_length)

    {

      data   = new char[copy.m_length + 1];

      m_length = copy.m_length;

      stx_memcpy(data, copy.data, copy.m_length);

      data[m_length] = 0;

    }

  }

  return *this;

}



void NSString::set(const char* string, int slen)

{

  clear();



  if(string && slen)

  {

    data   = new char[slen + 1];

    m_length = slen;

    stx_memcpy(data, string, slen);

    data[m_length] = 0;

  }

}



NSString &NSString::operator  =(const char *string)

{

  clear();



  unsigned int slen = 0;

  if(string && (slen = stx_strlen(string)))

  {

    data   = new char[slen + 1];

    m_length = slen;

    stx_memcpy(data, string, slen);

    data[m_length] = 0;

  }

  return *this;

}



NSString &NSString::operator +=(const char character)

{

  if(character != 0)

  {

    if(data && m_length)

    {

      int   newLength = m_length + 1;

      char *newBuffer = new char[newLength + 1];



      stx_memcpy(newBuffer, data, m_length);

      newBuffer[m_length    ] = character;

      newBuffer[m_length + 1] = 0;

      clear();

      m_length = newLength;

      data   = newBuffer;

    }

    else

    {

      data    = new char[2];

      data[0] = character;

      data[1] =  0;

      m_length  = 1;

    }

  }

  return *this;

}



NSString  NSString::operator + (const char character)

{

  NSString returnString(*this);

  returnString += character;

  return returnString;

}



NSString &NSString::operator +=(const NSString &copy)

{

  if(copy.data && copy.m_length)

  {

    char *newBuffer = new char[m_length + copy.m_length + 1];

    int   newLength = m_length + copy.m_length;



    if(!data)

    {

      data   = newBuffer;

      m_length = newLength;



      stx_memcpy(data, copy.data, m_length);

      data[m_length] = 0;

    }

    else

    {

      stx_memcpy(newBuffer         , data     , m_length     );

      stx_memcpy(newBuffer + m_length, copy.data, copy.m_length);

      newBuffer[newLength] = 0;

      clear();

      data   = newBuffer;

      m_length = newLength;

    }

  }

  return *this;

}



NSString  NSString::operator + (const NSString &copy)

{

  NSString returnString(*this);

  returnString += copy;

  return returnString;

}



NSString NSString::operator + (const int numberi)

{

  NSString  returnString;

  char   *numberBuffer = new char[12 + m_length];



  if(data && m_length)

    stx_snprintf(numberBuffer, 12 + m_length, "%s%d", data, numberi);

  else

    stx_snprintf(numberBuffer, 12 + m_length, "%d", numberi);



  returnString.data   = numberBuffer;

  returnString.m_length = stx_strlen(numberBuffer);

  return returnString;

}



NSString NSString::operator + (const float numberf)

{

  NSString  returnString;

  char   *numberBuffer = new char[16 + m_length];



  if(data && m_length)

    stx_snprintf(numberBuffer,16 + m_length, "%s%.2f", data, numberf);

  else

    stx_snprintf(numberBuffer,16 + m_length, "%.2f", numberf);



  returnString.data   = numberBuffer;

  returnString.m_length = stx_strlen(numberBuffer);

  return returnString;

}



NSString &NSString::operator +=(const int    numberi)

{

  char   *numberBuffer = new char[12 + m_length];



  if(data && m_length)

    stx_snprintf(numberBuffer, 12 + m_length, "%s%d", data, numberi);

  else

    stx_snprintf(numberBuffer, 12 + m_length, "%d", numberi);



  clear();



  data   = numberBuffer;

  m_length = stx_strlen(numberBuffer);



  return *this;

}



NSString &NSString::operator +=(const float  numberf)

{

  char   *numberBuffer = new char[16 + m_length];



  if(data && m_length)

    stx_snprintf(numberBuffer,16 + m_length, "%s%.2f", data, numberf);

  else

    stx_snprintf(numberBuffer,16 + m_length, "%.2f", numberf);



  clear();



  data   = numberBuffer;

  m_length = stx_strlen(numberBuffer);



  return *this;

}



bool   operator  < (const char   *arg1, const NSString &arg2)

{

  return (!arg2.length() || !arg1) ? false :

         (strcmp(arg1, arg2)  < 0);

}

bool   operator  < (const NSString &arg1, const char   *arg2)

{

  return (!arg1.length() || !arg2) ? false :

         (strcmp(arg1, arg2)  < 0);

}



bool   operator  < (const NSString &arg1, const NSString &arg2)

{

  return (!arg1.length() || !arg2.length()) ? false :

         (strcmp(arg1, arg2)  < 0);

}



bool   operator  > (const char   *arg1, const NSString &arg2)

{

  return (!arg2.length() || !arg1) ? false :

         (strcmp(arg1, arg2)  > 0);

}



bool   operator  > (const NSString &arg1, const char   *arg2)

{

  return (!arg1.length() || !arg2) ? false :

         (strcmp(arg1, arg2)  > 0);

}



bool   operator  > (const NSString &arg1, const NSString &arg2)

{

  return (!arg1.length() || !arg2.length()) ? false :

         (strcmp(arg1, arg2)  > 0);

}



bool   operator  ==(const NSString &arg1, const NSString &arg2)

{

  return (arg1.length() == arg2.length()) &&

         (arg1.length() !=                0) &&

         !strcmp(arg1, arg2);

}



bool   operator  ==(const char   *bytes, const NSString &arg)

{

  return (bytes           != 0) &&

         (arg.length() !=    0) &&

         !strcmp(arg, bytes);

}



bool   operator  ==(const NSString &arg , const char   *bytes)

{

  return (bytes           != 0) &&

         (arg.length() !=    0) &&

         !strcmp(arg, bytes);

}



bool   operator  !=(const NSString &arg1, const NSString &arg2)

{

  return !((arg1.length() == arg2.length()) &&

           (arg1.length() !=                0) &&

           !strcmp(arg1, arg2));

}



bool   operator  !=(const char   *bytes, const NSString &arg)

{

  return !((bytes           != 0) &&

           (arg.length() !=    0) &&

           !strcmp(arg, bytes));

}



bool   operator  !=(const NSString &arg , const char   *bytes)

{

  return !((bytes           != 0) &&

           (arg.length() !=    0) &&

           !strcmp(arg, bytes));

}



NSString::operator const char* () const

{

  return data;

}



const    char *NSString::getBytes() const

{

  return data;

}


/*
const unsigned int NSString::length() const

{

  return m_length;

}
*/


void NSString::clear()

{

  if(data)

  {

    delete [] data;

    data = 0;

  }

  m_length = 0;

}



NSString::~NSString()

{

  clear();

}



NSString &NSString::addFloat(const float numberf, int precision)

{

  precision          =  precision < 1 ? 1 : precision > 6 ? 6 : precision;

  const char *format = (precision == 1) ? "%s%.1f" :

                       (precision == 2) ? "%s%.2f" :

                       (precision == 3) ? "%s%.3f" :

                       (precision == 4) ? "%s%.4f" :

                       (precision == 5) ? "%s%.5f" : "%s%.6f";



  char *numberBuffer = new char[16 + m_length];



  if(data && m_length)

    stx_snprintf(numberBuffer, 16 + m_length, format, data, numberf);

  else

    stx_snprintf(numberBuffer, 16 + m_length, "%s", data);



  clear();



  data   = numberBuffer;

  m_length = (unsigned int)(stx_strlen(numberBuffer));



  return *this;

}
#endif

