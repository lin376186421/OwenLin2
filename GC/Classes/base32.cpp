#include "base32.h"
#include <memory>
#include <locale>
//using namespace psl;
static BYTE base32Chars[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static BYTE base32Lookup[BASE32_LOOKUP_MAX][2] =
{
    { '0', 0xFF },
    { '1', 0xFF },
    { '2', 0x1A },
    { '3', 0x1B },
    { '4', 0x1C },
    { '5', 0x1D },
    { '6', 0x1E },
    { '7', 0x1F },
    { '8', 0xFF },
    { '9', 0xFF },
    { ':', 0xFF },
    { ';', 0xFF },
    { '<', 0xFF },
    { '=', 0xFF },
    { '>', 0xFF },
    { '?', 0xFF },
    { '@', 0xFF },
    { 'A', 0x00 },
    { 'B', 0x01 },
    { 'C', 0x02 },
    { 'D', 0x03 },
    { 'E', 0x04 },
    { 'F', 0x05 },
    { 'G', 0x06 },
    { 'H', 0x07 },
    { 'I', 0x08 },
    { 'J', 0x09 },
    { 'K', 0x0A },
    { 'L', 0x0B },
    { 'M', 0x0C },
    { 'N', 0x0D },
    { 'O', 0x0E },
    { 'P', 0x0F },
    { 'Q', 0x10 },
    { 'R', 0x11 },
    { 'S', 0x12 },
    { 'T', 0x13 },
    { 'U', 0x14 },
    { 'V', 0x15 },
    { 'W', 0x16 },
    { 'X', 0x17 },
    { 'Y', 0x18 },
    { 'Z', 0x19 }
};
/*	Function:	计算普通数据经Base编码后的的数据长度
 *	Parameters:	in/out	type		name		desc
 *				in		int			rawLength	需要的内存大小
 *	Return:		value	desc
 *				__int64	返回现有的缓冲区长度
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来，原因见文件开头处的描述
 */
int EncodeLengthBase32(int rawLength)
{
    return ((rawLength * 8) / 5) + ((rawLength % 5) != 0) + 1;
}

/*	Function:	计算Base数据解码后的的数据长度
 *	Parameters:	in/out	type		name		desc
 *				in		int			rawLength	需要的内存大小
 *	Return:		value	desc		
 *				__int64	返回现有的缓冲区长度
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来，原因见文件开头处的描述
 */
int DecodeLengthBase32(int base32Length)
{
   return ((base32Length * 5) / 8);
}

/*	Function:	给一个指定长度的数据块进行Base32编码,输出在
 *				szOutString是字符串.szOutString是给定的缓冲
 *				区,其尺寸应该大于EncodeLengthBase32(bufLne)+1
 *	Parameters:	in/out	type			name		desc
 *				in		BYTE * buffer	buffer		待Base32编码的原始数据
 *				in		DWORD			bufLen		输入数据的长度
 *				out		char *			szOutString	经Base32编码后的输出字串
 *	Return:		value	desc
 *				char *	返回经Base32编码后的输出字串指针
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来
 */
char * EncodeBase32(BYTE * buffer, DWORD bufLen,char * szOutString)
{
	char * szTmp = szOutString;
	unsigned int   i, index;
    unsigned char  word;
    for(i = 0, index = 0; i < bufLen;)
    {
        if (index > 3)
        {
            word = (buffer[i] & (0xFF >> index));
            index = (index + 5) % 8;
            word <<= index;
            if (i < bufLen - 1)
                word |= buffer[i + 1] >> (8 - index);
            i++;
        }
        else
        {
            word = (buffer[i] >> (8 - (index + 5))) & 0x1F;
            index = (index + 5) % 8;
            if (index == 0)
               i++;
        }
		*(szTmp++) = (char) base32Chars[word];
    }
	*szTmp=0;
    return szOutString;
}

/*	Function:	给一个指定长度的数据块进行Base32编码,输出在
 *				szOutString是字符串.szOutString是给定的缓冲
 *				区,其尺寸应该大于EncodeLengthBase32(bufLne)+1
 *	Parameters:	in/out	type			name			desc
 *				in		const char *	base32Buffer	待解码的Base32字串
 *				in		unsigned int	base32BufLen	输入Base32字串的长度
 *				out		BYTE *			buffer			经Base32解码后的输出数据,其长度应该是DecodeLengthBase32(base32BufLen)
 *	Return:		void
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来
 */
void DecodeBase32(const char *base32Buffer, unsigned int base32BufLen, BYTE *buffer)
{
	int            i, index, max, lookup, offset;
	unsigned char  word;

	memset(buffer, 0, DecodeLengthBase32(base32BufLen));
	max = strlen(base32Buffer);
	for(i = 0, index = 0, offset = 0; i < max; i++)
	{
		lookup = toupper(base32Buffer[i]) - '0';
		if ( lookup < 0 || lookup >= BASE32_LOOKUP_MAX)
			word = 0xFF;
		else
			word = base32Lookup[lookup][1];
		if (word == 0xFF)
			continue;

		if (index <= 3)
		{
			index = (index + 5) % 8;
			if (index == 0)
			{
				buffer[offset] |= word;
				offset++;
			}
			else
				buffer[offset] |= word << (8 - index);
		}
        else
        {
			index = (index + 5) % 8;
			buffer[offset] |= (word >> index);
			offset++;
			buffer[offset] |= word << (8 - index);
		}
	}
}
