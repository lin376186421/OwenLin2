//
//  UIButton+Utils.h
//  DrivingTest
//
//  Created by cheng on 16/9/13.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIButton (Utils)

- (void)setTitle:(NSString *)title;

- (void)setTitleFont:(UIFont *)font;
- (void)setTitleFontSize:(CGFloat)fontSize;

- (void)setTitleColor:(UIColor *)color;
- (void)setTitleColorString:(NSString *)colorString;

- (void)setTitleFont:(UIFont *)font color:(UIColor *)color;
- (void)setTitleFontSize:(CGFloat)fontSize color:(UIColor *)color;
- (void)setTitleFontSize:(CGFloat)fontSize colorString:(NSString *)colorString;

- (void)setTitle:(NSString *)title fontSize:(CGFloat)fontSize colorString:(NSString *)colorString;

- (void)setTitle:(NSString *)title image:(UIImage *)image;
- (void)setTitle:(NSString *)title image:(UIImage *)image selImage:(UIImage *)selImage;
- (void)setTitle:(NSString *)title imageName:(NSString *)imageName;
- (void)setTitle:(NSString *)title imageName:(NSString *)imageName selImageName:(NSString *)selImageName;

- (void)setImageWithImageName:(NSString *)imageName;;

//- (void)setBackgroundImageAndHightlightWithColorHex:(NSString *)colorHex;
//- (void)setBackgroundImageAndHightlightWithColorHex:(NSString *)colorHex cornerRadius:(CGFloat)cornerRadius;

- (void)addTarget:(id)target action:(SEL)action;

//- (void)setTitleEdgeInsetsForIphoneX;
//- (void)setImageEdgeInsetsForIphoneX;
//- (void)setTitleEdgeInsetsForIphoneXWithEdgeInset:(UIEdgeInsets)fixEdgeInset;

@end
