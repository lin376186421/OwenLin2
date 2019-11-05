//
//  DESUtil.m
//  DESTest
//
//  Created by 董禹 on 14-3-27.
//  Copyright (c) 2014年 董禹. All rights reserved.
//

#import "DESUtil_DT.h"
//#import "GTMBase64.h"
//#include "hexstring.h"

#define DES_ECB_KEY @"AS5D9V"

@implementation DESUtil_DT

+(NSData *)hexToByteToNSData:(NSString *)str{
    int j=0;
    Byte bytes[[str length]/2];
    for(int i=0;i<[str length];i++)
    {
        int int_ch;  ///两位16进制数转化后的10进制数
        unichar hex_char1 = [str characterAtIndex:i]; ////两位16进制数中的第一位(高位*16)
        int int_ch1;
        if(hex_char1 >= '0' && hex_char1 <='9')
            int_ch1 = (hex_char1-48)*16;   //// 0 的Ascll - 48
        else if(hex_char1 >= 'A' && hex_char1 <='F')
            int_ch1 = (hex_char1-55)*16; //// A 的Ascll - 65
        else
            int_ch1 = (hex_char1-87)*16; //// a 的Ascll - 97
        i++;
        unichar hex_char2 = [str characterAtIndex:i]; ///两位16进制数中的第二位(低位)
        int int_ch2;
        if(hex_char2 >= '0' && hex_char2 <='9')
            int_ch2 = (hex_char2-48); //// 0 的Ascll - 48
        else if(hex_char2 >= 'A' && hex_char2 <='F')
            int_ch2 = hex_char2-55; //// A 的Ascll - 65
        else
            int_ch2 = hex_char2-87; //// a 的Ascll - 97
        int_ch = int_ch1+int_ch2;
        bytes[j] = int_ch;  ///将转化后的数放入Byte数组里
        //        if (j==[str length]/2-2) {
        //            int k=2;
        //            int_ch=bytes[0]^bytes[1];
        //            while (k
        //                int_ch=int_ch^bytes[k];
        //                k++;
        //            }
        //            bytes[j] = int_ch;
        //        }
        j++;
    }
    NSData *newData = [[NSData alloc] initWithBytes:bytes length:[str length]/2 ];
    NSLog(@"%@",newData);
    return newData;
}

+(NSString *)NSDataToByteTohex:(NSData *)data{
    Byte *bytes = (Byte *)[data bytes];
    NSString *hexStr=@"";
    for(int i=0;i<[data length];i++)
    {
        NSString *newHexStr = [NSString stringWithFormat:@"%x",bytes[i]&0xff];///16进制数
        if([newHexStr length]==1)
            hexStr = [NSString stringWithFormat:@"%@0%@",hexStr,newHexStr];
        else
            hexStr = [NSString stringWithFormat:@"%@%@",hexStr,newHexStr];
    }
    NSLog(@"hexStr:%@",hexStr);
    return hexStr;
}

//将汉字字符串转换成16进制字符串
+(NSString *)chineseToHex:(NSString*)chineseStr{
    NSStringEncoding encodingGB18030= CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
    NSData *responseData =[chineseStr dataUsingEncoding:encodingGB18030 ];
    NSString *string=[self NSDataToByteTohex:responseData];
    return string;
}
//将汉字字符串转换成UTF8字符串
+(NSString *)chineseToUTf8Str:(NSString*)chineseStr{
    NSStringEncoding encodingUTF8 = NSUTF8StringEncoding;
    NSData *responseData2 =[chineseStr dataUsingEncoding:encodingUTF8 ];
    NSString *string=[self NSDataToByteTohex:responseData2];
    return string;
}
//将十六进制字符串转换成汉字
+(NSString*)changeLanguage:(NSString*)chinese{
    NSString *strResult;
    NSLog(@"chinese:%@",chinese);
    if (chinese.length%2==0) {
        //第二次转换
        NSData *newData = [self hexToByteToNSData:chinese];
        unsigned long encode = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
        strResult = [[NSString alloc] initWithData:newData encoding:encode];
        NSLog(@"strResult:%@",strResult);
    }else{
        NSString *strResult = @"已假定是汉字的转换，所传字符串的长度必须是4的倍数!";
        NSLog(@"%@",strResult);
        return NULL;
    }
    return strResult;
}
/////////////GBK,汉字，GB2312,ASCII码，UTF8,UTF16
//UTF8字符串转换成汉字
+(NSString*)changeLanguageUTF8:(NSString*)chinese{
    NSString *strResult;
    NSData *data=[self hexToByteToNSData:chinese];
    strResult=[[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    return strResult;
}
//将十六进制数转换成十进制
-(NSString *)ToHex:(int)tmpid
{
    NSLog(@"tmpid:%d",tmpid);
    NSString *nLetterValue;
    NSString *str =@"";
    int ttmpig;
    for (int i = 0; i<9; i++) {
        ttmpig=tmpid;
        tmpid=tmpid/16;
        switch (ttmpig)
        {
            case 10:
                nLetterValue =@"A";break;
            case 11:
                nLetterValue =@"B";break;
            case 12:
                nLetterValue =@"C";break;
            case 13:
                nLetterValue =@"D";break;
            case 14:
                nLetterValue =@"E";break;
            case 15:
                nLetterValue =@"F";break;
            default:nLetterValue=[[NSString alloc]initWithFormat:@"%i",ttmpig];
        }
        str = [nLetterValue stringByAppendingString:str];
        NSLog(@"str:%@",str);
        if (tmpid == 0) {
            break;
        }
    }
    return str;
}
////Unicode转化为汉字
//+ (NSString *)replaceUnicode1:(NSString *)unicodeStr {
//    NSString *tempStr1 = [unicodeStr stringByReplacingOccurrencesOfString:@"\\u"withString:@"\\U"];
//    NSString *tempStr2 = [tempStr1 stringByReplacingOccurrencesOfString:@"""withString:@"\\""];
//    NSString *tempStr3 = [[@"" stringByAppendingString:tempStr2] stringByAppendingString:@"""];
//                          NSData *tempData = [tempStr3 dataUsingEncoding:NSUTF8StringEncoding];
//                          NSString* returnStr = [NSPropertyListSerialization propertyListFromData:tempData mutabilityOption:NSPropertyListImmutable format:NULL errorDescription:NULL];
//                          return [returnStr stringByReplacingOccurrencesOfString:@"\\r\\n"withString:@"\n"];
//                          }
//                          //iso8859－1 到 unicode编码转换
//                          + (NSString *)changeISO88591StringToUnicodeString:(NSString *)iso88591String
//    {
//        NSMutableString *srcString = [[NSMutableString alloc]initWithString:iso88591String];
//        [srcString replaceOccurrencesOfString:@"&" withString:@"&" options:NSLiteralSearch range:NSMakeRange(0, [srcString length])];
//        [srcString replaceOccurrencesOfString:@"&加行末位两个字符" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [srcString length])];//#x
//        NSMutableString *desString = [[NSMutableString alloc]init];
//        NSArray *arr = [srcString componentsSeparatedByString:@";"];
//        for(int i=0;i<[arr count]-1;i++){
//            NSString *v = [arr objectAtIndex:i];
//            char *c = malloc(3);
//            int value = [StringUtil changeHexStringToDecimal:v];//将二进制转化为十进制
//            c[1] = value  &0x00FF;
//            c[0] = value >>8 &0x00FF;
//            c[2] = '';
//            [desString appendString:[NSString stringWithCString:c encoding:NSUnicodeStringEncoding]];
//            free(c);
//        }
//        return desString;
//}

+(BOOL)hasChinese:(NSString *)str {
    for(int i=0; i< [str length];i++){
        int a = [str characterAtIndex:i];
        if( a > 0x4e00 && a < 0x9fff)
        {
            return YES;
        }
    }
    return NO;
}

+ (NSString *)doCipher:(NSString *)sTextIn key:(NSString *)sKey
               context:(CCOperation)encryptOrDecrypt {
    BOOL hasChinese = [self hasChinese:sTextIn];
    NSStringEncoding EnC = NSUTF8StringEncoding;
    NSMutableData *dTextIn;
    if (encryptOrDecrypt == kCCDecrypt) {
        dTextIn = [[self parseHexToByteArray:sTextIn] mutableCopy];
    }
    else{
        dTextIn = [[sTextIn dataUsingEncoding: EnC] mutableCopy];
    }
    NSMutableData * dKey = [[sKey dataUsingEncoding:EnC] mutableCopy];
    [dKey setLength:kCCBlockSizeDES];
    uint8_t *bufferPtr1 = NULL;
    size_t bufferPtrSize1 = 0;
    size_t movedBytes1 = 0;
    bufferPtrSize1 = ([sTextIn length] + kCCKeySizeDES) & ~(kCCKeySizeDES -1);
    if (hasChinese) {
        bufferPtrSize1 = bufferPtrSize1 * 4;//确保长度是够的
    }
    bufferPtr1 = malloc(bufferPtrSize1 * sizeof(uint8_t));
    memset((void *)bufferPtr1, 0x00, bufferPtrSize1);
    
    CCCrypt(encryptOrDecrypt, // CCOperation op
            kCCAlgorithmDES, // CCAlgorithm alg
            kCCOptionPKCS7Padding, // CCOptions options
            [dKey bytes], // const void *key
            [dKey length], // size_t keyLength //
            [dKey bytes], // const void *iv
            [dTextIn bytes], // const void *dataIn
            [dTextIn length],  // size_t dataInLength
            (void *)bufferPtr1, // void *dataOut
            bufferPtrSize1,     // size_t dataOutAvailable
            &movedBytes1);
    
    //[dTextIn release];
    //[dKey release];
    
    NSString * sResult;
    if (encryptOrDecrypt == kCCDecrypt){
        sResult = [[NSString alloc] initWithData:[NSData dataWithBytes:bufferPtr1 length:movedBytes1] encoding:EnC];
        free(bufferPtr1);
    }
    else {
        NSData *dResult = [NSData dataWithBytes:bufferPtr1 length:movedBytes1];
        free(bufferPtr1);
        sResult = [self parseByteArray2HexString:(Byte *)dResult.bytes count:movedBytes1];
    }
    return sResult;
}



/**
 字节转化为16进制数
 */
+(NSString *) parseByte2HexString:(Byte *) bytes
{
    NSMutableString *hexStr = [[NSMutableString alloc]init];
    int i = 0;
    if(bytes)
    {
        while (bytes[i] != '\0')
        {
            NSString *hexByte = [NSString stringWithFormat:@"%x",bytes[i] & 0xff];///16进制数
            if([hexByte length]==1)
                [hexStr appendFormat:@"0%@", hexByte];
            else
                [hexStr appendFormat:@"%@", hexByte];
            
            i++;
        }
    }
    
    return hexStr;
}


/**
 字节数组转化16进制数
 */
+(NSString *) parseByteArray2HexString:(Byte[]) bytes count:(NSUInteger)count
{
    NSMutableString *hexStr = [[NSMutableString alloc]init];
    //    int i = 0;
    if(bytes)
    {
        //        while (bytes[i] != '\0')
        //        {
        //            NSString *hexByte = [NSString stringWithFormat:@"%x",bytes[i] & 0xff];///16进制数
        //
        
        //
        //            if([hexByte length]==1)
        //                [hexStr appendFormat:@"0%@", hexByte];
        //            else
        //                [hexStr appendFormat:@"%@", hexByte];
        //
        //            i++;
        //        }
        for (NSUInteger i=0;i< count;i++)
        {
            NSString *hexByte = [NSString stringWithFormat:@"%x",bytes[i] & 0xff];///16进制数
            
            if([hexByte length]==1)
                [hexStr appendFormat:@"0%@", hexByte];
            else
                [hexStr appendFormat:@"%@", hexByte];
            
            //            i++;
        }
    }
    
    return [hexStr uppercaseString];
}

/*
 将16进制数据转化成NSData 数组
 */
+(NSData*) parseHexToByteArray:(NSString*) hexString
{
    int j=0;
    Byte bytes[hexString.length];
    for(int i=0;i<[hexString length];i++)
    {
        int int_ch;  /// 两位16进制数转化后的10进制数
        unichar hex_char1 = [hexString characterAtIndex:i]; ////两位16进制数中的第一位(高位*16)
        int int_ch1;
        if(hex_char1 >= '0' && hex_char1 <='9')
            int_ch1 = (hex_char1-48)*16;   //// 0 的Ascll - 48
        else if(hex_char1 >= 'A' && hex_char1 <='F')
            int_ch1 = (hex_char1-55)*16; //// A 的Ascll - 65
        else
            int_ch1 = (hex_char1-87)*16; //// a 的Ascll - 97
        i++;
        unichar hex_char2 = [hexString characterAtIndex:i]; ///两位16进制数中的第二位(低位)
        int int_ch2;
        if(hex_char2 >= '0' && hex_char2 <='9')
            int_ch2 = (hex_char2-48); //// 0 的Ascll - 48
        else if(hex_char2 >= 'A' && hex_char2 <='F')
            int_ch2 = hex_char2-55; //// A 的Ascll - 65
        else
            int_ch2 = hex_char2-87; //// a 的Ascll - 97
        
        int_ch = int_ch1+int_ch2;
        bytes[j] = int_ch;  ///将转化后的数放入Byte数组里
        j++;
    }
    
    NSData *newData = [[NSData alloc] initWithBytes:bytes length:hexString.length/2];
    
    return newData;
}

+ (NSData *)desEncOrDecData:(NSData *)data WithKey:(NSString *)key EncOrDec:(CCOperation)operation{
    NSUInteger dataLength = [data length];
    size_t bufferSize = (dataLength + kCCBlockSizeDES) & ~(kCCBlockSizeDES - 1);
    void *buffer = malloc(bufferSize);
    
    size_t numBytesEncrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(operation,
                                          kCCAlgorithmDES,
                                          kCCOptionPKCS7Padding | kCCOptionECBMode,
                                          [key UTF8String],
                                          key.length,//kCCBlockSizeDES
                                          NULL,
                                          [data bytes],
                                          dataLength,
                                          buffer,
                                          bufferSize,&numBytesEncrypted);
    NSData *result=nil;
    if (cryptStatus == kCCSuccess){
        NSLog(@"SUCCESS");
        result = [NSData dataWithBytes:(const void *)buffer length:(NSUInteger)numBytesEncrypted];
    }
    
    free(buffer);
    return result;
}


@end
