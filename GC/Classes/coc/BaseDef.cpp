#include "BaseDef.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <ctype.h>

#include "../coc/LogicInfo.h"

NS_COC_BEGIN

float countRadian(float bx, float by, float ex, float ey)
{
	return atan2(ey - by, ex - bx);

	float radian = 0.0f; // 夹角

	CCPoint p1;
	p1.x = bx;
	p1.y = by + 100;

	CCPoint p2;
	p2.x = ex;
	p2.y = ey;

	CCPoint pSrc;
	pSrc.x = bx;
	pSrc.y = by;
      
    // 向量Vector a的(x, y)坐标  
    float va_x = p1.x - pSrc.x;
    float va_y = p1.y - pSrc.y;  
      
    // 向量b的(x, y)坐标  
    float vb_x = p2.x - pSrc.x;  
    float vb_y = p2.y - pSrc.y;  
      
    float productValue = (va_x * vb_x) + (va_y * vb_y);  // 向量的乘积  
    float va_val = sqrt(va_x * va_x + va_y * va_y);  // 向量a的模  
    float vb_val = sqrt(vb_x * vb_x + vb_y * vb_y);  // 向量b的模  
    float cosValue = productValue / (va_val * vb_val);      // 余弦公式  
      
    // acos的输入参数范围必须在[-1, 1]之间，否则会"domain error"  
    // 对输入参数作校验和处理  
    if(cosValue < -1 && cosValue > -2)  
        cosValue = -1;  
    else if(cosValue > 1 && cosValue < 2)  
        cosValue = 1;  

	radian = acos(cosValue);
      
    // acos返回的是弧度值，转换为角度值  
    //angle = acos(cosValue) * 180 / M_PI;  
      
    return radian;  
}

std::string UrlEncode(const char* szToEncode)
{
    std::string src = szToEncode;
    char hex[] = "0123456789ABCDEF";
    std::string dst;
    for (size_t i = 0; i < src.size(); ++i)
    {
        unsigned char cc = src[i];
        if((cc >= '0' && cc <= '9') || (cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z'))
        {
            if (cc == ' ')
            {
                dst += "%20";
            }
            else
                dst += cc;
        }
        else
        {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}

std::string UrlDecode(const char* str)
{
    std::string szToDecode = str;
    std::string result;
    int hex = 0;
    for (size_t i = 0; i < szToDecode.length(); ++i)
    {
        switch (szToDecode[i])
        {
            case '+':
                result += ' ';
                break;
            case '%':
                if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
                {
                    std::string hexStr = szToDecode.substr(i + 1, 2);
                    hex = strtol(hexStr.c_str(), 0, 16);
                    ////字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
                    //  //可以不经过编码直接用于URL
                    //if (!((hex >= 48 && hex <= 57) || //0-9
                    // (hex >=97 && hex <= 122) ||   //a-z
                    // (hex >=65 && hex <= 90) ||    //A-Z
                    // //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
                    // hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
                    //|| hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
                    //|| hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
                    //))
                    // {
                    result += char(hex);
                    i += 2;
                    //}
                    //else result += '%';
                    //}else {
                    //  result += '%';
                    // }
                }
                break;
            default:
                result += szToDecode[i];
                break;
        }
    }
    return result;
}

int getAsInt(const Json::Value& val)
{
	if(val.isString())
		return atoi(val.asString().c_str());
	else if(val.isInt())
		return val.asInt();

	return 0;
}

const char* getServiceKey()
{
	return "djirtu304sdakjflh34tiuytbndbfvhj239p8ty3ogdmnasbvy3ury280o5tg";
}

bool isUserMask(int mask, int m)
{
    return (mask & m);
}

NS_COC_END
