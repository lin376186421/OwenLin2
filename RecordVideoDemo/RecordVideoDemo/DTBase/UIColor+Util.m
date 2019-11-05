//
//  UIColor+Util.m
//  DrivingTest
//
//  Created by cheng on 16/9/21.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import "UIColor+Util.h"
#import "BPUIColorAdditions.h"
@implementation UIColor (Util)

+ (UIColor *)colorWithHexValue:(uint)hexValue;
{
    return [UIColor colorWithHexValue:hexValue alpha:1.0];
}

+ (UIColor *)colorWithString:(NSString *)hexString alpha:(float)alpha {
    if (hexString == nil || (id)hexString == [NSNull null]) {
        return nil;
    }
    UIColor *col;
    if (![hexString hasPrefix:@"#"]) {
        hexString = [NSString stringWithFormat:@"#%@", hexString];
    }
    hexString = [hexString stringByReplacingOccurrencesOfString:@"#"
                                                     withString:@"0x"];
    uint hexValue;
    if ([[NSScanner scannerWithString:hexString] scanHexInt:&hexValue]) {
        col = [self colorWithHexValue:hexValue alpha:alpha];
    } else {
        // invalid hex string
        col = [UIColor clearColor];
    }
    return col;
}

+ (UIColor *)colorWithString:(NSString *)hexStr
{
    return [self colorWithHexString:hexStr alpha:1.0f];
}

+ (UIColor *)colorWithAlphaString:(NSString *)hexStr
{
    NSArray *hexArray = [hexStr componentsSeparatedByString:@"-"];
    if (hexArray.count==2) {
        return [self colorWithHexString:hexArray[0] alpha:[hexArray[1] floatValue]/100.0f];
    } else {
        return [self colorWithHexString:hexStr alpha:1.f];
    }
}
@end
