//	String.cpp

#include "Core/Types.h"
#include "Core/String.hpp"

static u32 crc_table[256];

static void make_crc_table(void)
{
	u32	c;
	int n, k; 
	
	for (n = 0; n < 256; n++)
	{
		c = (unsigned long) n;
		for (k = 0; k < 8; k++)
		{
            if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
            else
				c = c >> 1;
		}
		crc_table[n] = c;
	}
}

u32 prehash(const u8 *chars, int len)
{
	u32 hash=0;

	if (chars[0]=='#' && len==9)	// # and eight chars
	{
		for (int i=1; i<9; i++)
		{
			u8 ch=chars[i];
			if (!((ch>='0' && ch<='9') || (ch>='A' && ch<='F')))
			{
				//	Uh-oh, wasn't a hex code - abort and do "properly"!
				
				break;
			}

			hash<<=4;
			hash|=(ch>'9') ? (ch-'A'+10) : (ch-'0');
			ch++;
		}
	}
	return hash;
}


void InitHash()
{
	if (!crc_table[1])
	{
		make_crc_table();
	}
}

void ShutdownHash()
{
}

u32 StringHash(const String& str)
{
	InitHash();

	u32 hash = prehash((u8*)str.c_str(), str.length());

	if (!hash)
	{
		u32 c = 0;
		for (auto i=str.begin(); i!= str.end(); ++i)
			c = crc_table[(c ^ tolower(*i)) & 0xff] ^ (c >> 8);

		hash = c ^ 0xffffffffL;
	}

	return hash;
}

