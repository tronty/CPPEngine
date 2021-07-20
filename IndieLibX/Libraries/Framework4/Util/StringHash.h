#include "../Platform.h"

#ifdef DEBUG
	#define SH_DEBUG_SAVE_STRING(str) m_String = str
#else
	#define SH_DEBUG_SAVE_STRING(str)
#endif

struct StringHash
{
	force_inline operator uint() const { return m_Hash; }


	// For run-time use
	StringHash(){}
	StringHash(const char *str, const uint length)
	{
		ComputeHash(str, length);
	}
	void ComputeHash(const char *str, const uint length)
	{
		uint32 hash = 0;
		for (uint i = 0; i < length; i++)
		{
			hash = hash * 65599 + str[i];
		}
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING("");
	}

	// For compile-time use to pre-generated and inlined hash value
	/*StringHash(const char (&str)[1])
	{
		m_Hash = 0;
	}*/
	force_inline StringHash(const char (&str)[2])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[3])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[4])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[5])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[6])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[7])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[8])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[9])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[10])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[11])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[12])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[13])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[14])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[15])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[16])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[17])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		hash = hash * 65599 + str[15];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[18])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		hash = hash * 65599 + str[15];
		hash = hash * 65599 + str[16];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[19])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		hash = hash * 65599 + str[15];
		hash = hash * 65599 + str[16];
		hash = hash * 65599 + str[17];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[20])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		hash = hash * 65599 + str[15];
		hash = hash * 65599 + str[16];
		hash = hash * 65599 + str[17];
		hash = hash * 65599 + str[18];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}
	force_inline StringHash(const char (&str)[21])
	{
		uint32 hash = 0;
		hash = hash * 65599 + str[0];
		hash = hash * 65599 + str[1];
		hash = hash * 65599 + str[2];
		hash = hash * 65599 + str[3];
		hash = hash * 65599 + str[4];
		hash = hash * 65599 + str[5];
		hash = hash * 65599 + str[6];
		hash = hash * 65599 + str[7];
		hash = hash * 65599 + str[8];
		hash = hash * 65599 + str[9];
		hash = hash * 65599 + str[10];
		hash = hash * 65599 + str[11];
		hash = hash * 65599 + str[12];
		hash = hash * 65599 + str[13];
		hash = hash * 65599 + str[14];
		hash = hash * 65599 + str[15];
		hash = hash * 65599 + str[16];
		hash = hash * 65599 + str[17];
		hash = hash * 65599 + str[18];
		hash = hash * 65599 + str[19];
		m_Hash = hash;

		SH_DEBUG_SAVE_STRING(str);
	}

	uint32 m_Hash;

#ifdef DEBUG
	const char *m_String;
	#define SH_DEBUG_SAVE_STRING(str) m_String = str
#else
	#define SH_DEBUG_SAVE_STRING(str)
#endif
};

force_inline bool operator == (const StringHash &h0, const StringHash &h1)
{
	return (h0.m_Hash == h1.m_Hash);
}

force_inline bool operator > (const StringHash &h0, const StringHash &h1)
{
	return (h0.m_Hash > h1.m_Hash);
}

force_inline bool operator < (const StringHash &h0, const StringHash &h1)
{
	return (h0.m_Hash < h1.m_Hash);
}
