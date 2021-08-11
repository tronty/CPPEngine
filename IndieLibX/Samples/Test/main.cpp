/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <STX/Matrix.h>

int ApplicationLogic()
{
	LOG_FNLN;
	LOG_PRINT("sizeof(BYTE)=%d\n", sizeof(BYTE));
	LOG_PRINT("sizeof(half)=%d\n", sizeof(half));
	LOG_PRINT("sizeof(unsigned short)=%d\n", sizeof(unsigned short));
	LOG_PRINT("sizeof(int)=%d\n", sizeof(int));
	LOG_PRINT("sizeof(__WORD__)=%d\n", sizeof(__WORD__));
	LOG_PRINT("sizeof(uint)=%d\n", sizeof(uint));
	LOG_PRINT("sizeof(unsigned int)=%d\n", sizeof(unsigned int));
	LOG_PRINT("sizeof(::uint32_t)=%d\n", sizeof(::uint32_t));
	LOG_PRINT("sizeof(__DWORD__)=%d\n", sizeof(__DWORD__));

	std::string f=std::string("/");
	std::string file=stx_convertpath(f);

	LOG_PRINT("iosDocumentsFolder(/)=%s\n", file.c_str());
	return 0;
}

