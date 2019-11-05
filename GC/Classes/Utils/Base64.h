//
//  Base64.h
//  CoC
//
//  Created by qsun on 13-6-17.
//
//

#ifndef __CoC__Base64__
#define __CoC__Base64__

#include <string>
using namespace std;

class Base64
{
public:
    /*编码
     DataByte
     [in]输入的数据长度,以字节为单位
     */
    static string Encode(const unsigned char* Data, int DataByte);
    /*解码
     DataByte
     [in]输入的数据长度,以字节为单位
     OutByte
     [out]输出的数据长度,以字节为单位,请不要通过返回值计算
     输出数据的长度
     */
    static string Decode(const char* Data, int DataByte, unsigned long& OutByte);
    
    static void Encode(const unsigned char* Data, int DataByte, unsigned char* OutData, unsigned long& OutByte);
    
    static void Decode(unsigned char* InData, int DataByte, unsigned char* OutData, unsigned long& OutByte);
};

#endif /* defined(__CoC__Base64__) */
