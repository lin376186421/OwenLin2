//
//  BPCoreUtil.h
//  VGEUtil
//
//  Created by Hunter Huang on 11/24/11.
//  Copyright (c) 2011 vge design. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "BPCommon.h"

@interface BPCoreUtil : NSObject

+ (id)convertNSNullToNil:(id)source;
/**
 * @param source source is a string, just return; if source is a number, convert it to a string; otherwise, return nil
 */
+ (NSString *)convertToString:(id)source;
+ (float)convertToFloat:(id)source;
+ (double)convertToDouble:(id)source;
+ (int)convertToInt:(id)source;
+ (bool)convertToBool:(id)source;

/**
 with notch: 44.0 on iPhone X, XS, XS Max, XR.
 without notch: 20.0 on iPhone 8 on iOS 12+.
 */
+ (BOOL)hasTopNotch;

/// with home indicator: 34.0 on iPhone X, XS, XS Max, XR.
+ (BOOL)hasBottomSafeAreaInsets;

/// 是否是iPhone X以后的设备
+ (BOOL)isIPhoneXDevice;

/// 获取一设备像素
+ (CGFloat)screenOnePixel;

///压缩时Buffer需要的额外内存空间
+ (NSInteger)compressBufferExtra;

//去掉头尾空格
+ (NSString *)trimmingWhiteSpace:(NSString *)source;

//去掉字符串内的所有空格
+ (NSString *)removeAllWhiteSpace:(NSString *)source;

#pragma mark - DEPRECATED

+ (BOOL)isNilOrNull:(id)obj DEPRECATED_MSG_ATTRIBUTE("Use -checkIsEmpty:");
+ (BOOL)isStringEmpty:(NSString *)str DEPRECATED_MSG_ATTRIBUTE("Use -checkIsEmpty:");

+ (UIColor *)colorWithHexValue:(uint)hexValue alpha:(float)alpha DEPRECATED_MSG_ATTRIBUTE("Use UIColor colorWithHexValue:alpha:");
+ (UIColor *)colorWithHexString:(NSString*) hexStr alpha: (float) alpha DEPRECATED_MSG_ATTRIBUTE("Use UIColor colorWithHexString:alpha:");
+ (UIColor *)colorWithHexString:(NSString*) hexStr DEPRECATED_MSG_ATTRIBUTE("Use UIColor colorWithHexString:"); //支持alphpa FFFFFF-95 (95为不透明度的百分比)
+ (NSString *)createUUID DEPRECATED_MSG_ATTRIBUTE("Use NSString createUUID:");

+ (NSString*)devicePlatform DEPRECATED_MSG_ATTRIBUTE("Use [UIDevice currentDevice].machineModel");

@end
