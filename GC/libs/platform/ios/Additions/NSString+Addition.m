//
//  NSString+MD5Addition.m
//  UIDeviceAddition
//
//  Created by Georg Kitz on 20.08.11.
//  Copyright 2011 Aurora Apps. All rights reserved.
//

#import "NSString+Addition.h"
#import <CommonCrypto/CommonDigest.h>

@implementation NSString(Addition)

- (NSString *) stringFromMD5{
    
    if(self == nil || [self length] == 0)
        return nil;
    
    const char *value = [self UTF8String];
    
    unsigned char outputBuffer[CC_MD5_DIGEST_LENGTH];
    CC_MD5(value, strlen(value), outputBuffer);
    
    NSMutableString *outputString = [[NSMutableString alloc] initWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for(NSInteger count = 0; count < CC_MD5_DIGEST_LENGTH; count++){
        [outputString appendFormat:@"%02x",outputBuffer[count]];
    }
    
    return [outputString autorelease];
}

- (BOOL)isValidEmail{ 
    
    NSString *emailRegex = @"^\\w+((\\-\\w+)|(\\.\\w+))*@[A-Za-z0-9]+((\\.|\\-)[A-Za-z0-9]+)*.[A-Za-z0-9]+$"; 
    //@"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}"; 
    // @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}"; 
    //@"[A-Z0-9a-z][A-Z0-9a-z._%+-]*@[A-Za-z0-9][A-Za-z0-9.-]*\\.[A-Za-z]{2,6}"; 
    // @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";
    
    NSPredicate *regExPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", emailRegex]; 
    
    return [regExPredicate evaluateWithObject:self]; 
} 

-(BOOL)isPhoneNumber{
//    NSString *regex = @"^\\([0-9\\(\\)\\/\\+\\-]*)$"; 
//    
//    NSPredicate *regExPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex]; 
//    
//    return [regExPredicate evaluateWithObject:self]; 
    
    if ([self length]<11) {
        return NO;
    }
    
    NSRange range = NSMakeRange([self length]-11, 11);
    NSString *subString = [self substringWithRange:range];
    
    NSScanner* scan = [NSScanner scannerWithString:subString]; 
    int val; 
    return [scan scanInt:&val] && [scan isAtEnd]; 
}

-(BOOL)isNumber{
    NSScanner* scan = [NSScanner scannerWithString:self]; 
    int val; 
    return [scan scanInt:&val] && [scan isAtEnd]; 
}

@end
