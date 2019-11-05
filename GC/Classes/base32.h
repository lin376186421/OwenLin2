#pragma once

typedef unsigned char BYTE;
typedef unsigned long DWORD;


#define BASE32_LOOKUP_MAX 43

/*	Function:	计算普通数据经Base编码后的的数据长度
 *	Parameters:	in/out	type		name		desc
 *				in		int			rawLength	需要的内存大小
 *	Return:		value	desc
 *				__int64	返回现有的缓冲区长度
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来，原因见文件开头处的描述
 */
int EncodeLengthBase32(int rawLength);

/*	Function:	计算Base数据解码后的的数据长度
 *	Parameters:	in/out	type		name		desc
 *				in		int			rawLength	需要的内存大小
 *	Return:		value	desc		
 *				__int64	返回现有的缓冲区长度
 *	Memo:
 *	History:	version		date		operator	action
 *				1.0.0.0		2008.2.2	YuanPeng	从CyHash.cpp文件中复制出来，原因见文件开头处的描述
 */
int DecodeLengthBase32(int base32Length);

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
char * EncodeBase32(BYTE * buffer, DWORD bufLen,char * szOutString);

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
void DecodeBase32(const char *base32Buffer, unsigned int base32BufLen, BYTE *buffer);