//
//  BPUIColorAdditions.h
//  CLChatDemo
//
//  Created by R_flava_Man on 2017/5/5.
//  Copyright © 2017年 R_style_Man. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIColor (BPAdditions)

/**
 The color's alpha component value.
 The value of this property is a float in the range `0.0` to `1.0`.
 */
@property (nonatomic, readonly) CGFloat alpha;

+ (nullable UIColor *)colorWithHexValue:(uint)hexValue alpha:(float)alpha;
+ (nullable UIColor *)colorWithHexString:(NSString *)hexStr alpha:(float)alpha;
+ (nullable UIColor *)colorWithHexString:(NSString *)hexStr; //支持alphpa FFFFFF-95 (95为不透明度的百分比)

/**
 Returns the color's RGB value as a hex string (lowercase).
 Such as @"0066cc-95".
 if self.alpha = 1.0, there will not has "-95" suffix
 
 It will return nil when the color space is not RGB
 
 @return The color's value as a hex string.
 */
- (nullable NSString *)hexString;

@end

NS_ASSUME_NONNULL_END
