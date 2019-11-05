//
//  UILabel+Label.h
//  soshow.org
//
//  Created by Users on 13-9-2.
//  Copyright (c) 2013年 sos. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UILabel (Label)
+ (UILabel *)frame:(CGRect)frame WhiteName:(NSString *)name size:(CGFloat)fontSize;
+ (UILabel *)frame:(CGRect)frame BlackName:(NSString *)name size:(CGFloat)fontSize;
+ (UILabel *)frame:(CGRect)frame name:(NSString *)name size:(CGFloat)fontSize andTextColor:(UIColor *)color;
+ (UILabel *)frame:(CGRect)frame name:(NSString *)name size:(CGFloat)fontSize andTextColor:(UIColor *)color bgColor:(UIColor *)bgColor tag:(int)tag;

+ (NSMutableAttributedString *)getAttibutedStringWithColor:(UIColor *)color withString:(NSString *)string;
@end

