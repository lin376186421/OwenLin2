//
//  BPCoreUtil.m
//  VGEUtil
//
//  Created by Hunter Huang on 11/24/11.
//  Copyright (c) 2011 vge design. All rights reserved.
//

#import "BPCoreUtil.h"

@implementation BPCoreUtil

+ (id)convertNSNullToNil:(id) source
{
    if (source == [NSNull null]) {
        return nil;
    }
    return source;
}

+ (bool)convertToBool:(id)source
{
    if (source == [NSNull null]) {
        return NO;
    } 
    return [source boolValue];
}

+ (NSString *)convertToString:(id)source
{
    if ([source isKindOfClass:[NSString class]]) {
        return source;
    } else if ([source isKindOfClass:[NSNumber class]]) {
        return [NSString stringWithFormat:@"%@", source];
    } else {
        return nil;
    }
}

+ (float)convertToFloat:(id)source
{
    if ([source isKindOfClass:[NSString class]]) {
        return [source floatValue];
    } else if ([source isKindOfClass:[NSNumber class]]) {
        return [source floatValue];
    } else {
        return 0;
    }
}

+ (double)convertToDouble:(id)source
{
    if ([source isKindOfClass:[NSString class]]) {
        return [(NSString *)source doubleValue];
    } else if ([source isKindOfClass:[NSNumber class]]) {
        return [(NSNumber *)source doubleValue];
    } else {
        return 0;
    }
}

+ (int)convertToInt:(id)source
{
    if ([source isKindOfClass:[NSString class]]) {
        return [source intValue];
    } else if ([source isKindOfClass:[NSNumber class]]) {
        return [source intValue];
    } else {
        return 0;
    }
}

+ (BOOL)hasTopNotch
{
#ifdef __IPHONE_11_0
    if (@available(iOS 11.0, *)) {
        // with notch: 44.0 on iPhone X, XS, XS Max, XR.
        // without notch: 20.0 on iPhone 8 on iOS 12+.
        return [UIApplication sharedApplication].keyWindow.safeAreaInsets.top > 20.f;
    } else {
        return NO;
    }
#else
    return NO;
#endif
}

+ (BOOL)hasBottomSafeAreaInsets
{
#ifdef __IPHONE_11_0
    if (@available(iOS 11.0, *)) {
        // with home indicator: 34.0 on iPhone X, XS, XS Max, XR.
        return [UIApplication sharedApplication].keyWindow.safeAreaInsets.bottom > 0.f;
    } else {
        return NO;
    }
#else
    return NO;
#endif
}

//+ (BOOL)isIPhoneXDevice
//{
//#ifdef __IPHONE_11_0
//    if (@available(iOS 11.0, *)) {
//        NSString *modelName = [UIDevice currentDevice].machineModelName;
//        if ([modelName hasPrefix:@"iPhone X"]) {
//            return YES;
//        } else {
//            return [UIApplication sharedApplication].keyWindow.safeAreaInsets.bottom > 0.f;
//        }
//    } else {
//        return NO;
//    }
//#else
//    return NO;
//#endif
//}

+ (CGFloat)screenOnePixel
{
	if ([UIScreen mainScreen].scale == 3) {
		return 0.385f;
	} else if ([UIScreen mainScreen].scale == 2) {
		return .5f;
	} else {
		return 1.f;
	}
}

+ (NSInteger)compressBufferExtra
{
    return 100;
}

+ (NSString *)trimmingWhiteSpace:(NSString *)source
{
    return [source stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
}

+ (NSString *)removeAllWhiteSpace:(NSString *)source
{
    NSString *string = [source stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    NSArray *components = [string componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    components = [components filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self <> ''"]];
    string = [components componentsJoinedByString:@""];
    return string;
}

#pragma mark - DEPRECATED

//+ (BOOL)isNilOrNull:(id)obj
//{
//    return [NSObject checkIsEmpty:obj];
//}
//
//+ (BOOL)isStringEmpty:(NSString *)str
//{
//    return [NSObject checkIsEmpty:str];
//}
//
//+ (UIColor *)colorWithHexValue:(uint)hexValue alpha:(float)alpha
//{
//    return [UIColor colorWithHexValue:hexValue alpha:alpha];
//}
//
//+ (UIColor *)colorWithHexString:(NSString*)hexString alpha:(float)alpha
//{
//    return [UIColor colorWithHexString:hexString alpha:alpha];
//}
//
//+ (UIColor *) colorWithHexString: (NSString*)hexStr
//{
//    return [UIColor colorWithHexString:hexStr];
//}
//
//+ (NSString *)createUUID
//{
//    return [NSString createUUID];
//}
//
//+ (NSString*)devicePlatform
//{
//    return [UIDevice currentDevice].machineModel;
//}

@end
