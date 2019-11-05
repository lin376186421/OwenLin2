//
//  CUrlEncode.h
//  RobinHood
//
//  Created by qsun on 13-4-24.
//
//

#ifndef __Utils__CUrlParse__
#define __Utils__CUrlParse__

string encodeURL(std::string& szToEncode)
{
	char hex[] = "0123456789ABCDEF";
	string encode;
    
	for (size_t i = 0; i < szToEncode.size(); ++i)
	{
		unsigned char cc = szToEncode[i];
		if (isalnum(cc)) {
            encode += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(szToEncode[i]);
			encode += '%';
			encode += hex[c >> 4];
			encode += hex[c % 16];
		}
	}
	return encode;
}

unsigned char toHex(const unsigned char &x)
{
    return x > 9 ? x-10+'A': x+'0';
}
unsigned char fromHex(const unsigned char &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

string decodeUrl(std::string& szToDecode)
{
    string decode;
    for( size_t i = 0; i < szToDecode.size(); i++ )
    {
        unsigned char cc = szToDecode[i];
        char ch = 0;
        if(cc == '%')
        {
            ch = (fromHex(szToDecode[i+1])<<4);
            ch |= fromHex(szToDecode[i+2]);
            i += 2;
        }
        else if(cc == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = cc;
        }
        decode += ch;
    }
    return decode;
}

#endif /* defined(__Utils__CUrlParse__) */
