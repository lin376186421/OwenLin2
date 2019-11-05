//
//  DESUtil.h
//  DESTest
//
//  Created by 董禹 on 14-3-27.
//  Copyright (c) 2014年 董禹. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonCryptor.h>


@interface DESUtil_DT : NSObject

+ (NSString *)doCipher:(NSString *)sTextIn key:(NSString *)sKey
               context:(CCOperation)encryptOrDecrypt;

//DES加密
+ (NSString *)desEncryptString:(NSString *)text WithKey:(NSString *)key;

//DES解密
+ (NSString *)desDecryptString:(NSString *)text WithKey:(NSString *)key;

@end
//NSData* data = [self parseHexToByteArray:cipherText];[self parseByteArray2HexString:(Byte *)[data bytes] count:bufferNumBytes];
