//
//  UIButton+Utils.m
//  DrivingTest
//
//  Created by cheng on 16/9/13.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import "UIButton+Utils.h"

@implementation UIButton (Utils)

- (void)setTitle:(NSString *)title
{
    [self setTitle:title forState:UIControlStateNormal];
}

- (void)setTitleFont:(UIFont *)font
{
    self.titleLabel.font = font;
}

- (void)setTitleFontSize:(CGFloat)fontSize
{
    [self setTitleFont:[UIFont systemFontOfSize:fontSize]];
}

- (void)setTitleColor:(UIColor *)color
{
    [self setTitleColor:color forState:UIControlStateNormal];
}

- (void)setTitleColorString:(NSString *)colorString
{
    [self setTitleColor:[UIColor colorWithHexString:colorString]];
}

- (void)setTitleFont:(UIFont *)font color:(UIColor *)color
{
    [self setTitleFont:font];
    [self setTitleColor:color forState:UIControlStateNormal];
}

- (void)setTitleFontSize:(CGFloat)fontSize color:(UIColor *)color
{
    [self setTitleFont:[UIFont systemFontOfSize:fontSize] color:color];
}

- (void)setTitleFontSize:(CGFloat)fontSize colorString:(NSString *)colorString
{
    [self setTitleFontSize:fontSize color:[UIColor colorWithHexString:colorString]];
}

- (void)setTitle:(NSString *)title fontSize:(CGFloat)fontSize colorString:(NSString *)colorString
{
    [self setTitle:title forState:UIControlStateNormal];
    [self setTitleFontSize:fontSize colorString:colorString];
}

- (void)setTitle:(NSString *)title image:(UIImage *)image
{
    [self setTitle:title forState:UIControlStateNormal];
    [self setImage:image forState:UIControlStateNormal];
}

- (void)setTitle:(NSString *)title image:(UIImage *)image selImage:(UIImage *)selImage
{
    [self setTitle:title forState:UIControlStateNormal];
    [self setImage:image forState:UIControlStateNormal];
    [self setImage:selImage forState:UIControlStateSelected];
}

- (void)setTitle:(NSString *)title imageName:(NSString *)imageName
{
    [self setTitle:title image:(imageName.length>0?[UIImage imageNamed:imageName]:nil)];
}

- (void)setTitle:(NSString *)title imageName:(NSString *)imageName selImageName:(NSString *)selImageName
{
    [self setTitle:title image:(imageName.length>0?[UIImage imageNamed:imageName]:nil) selImage:(selImageName.length>0?[UIImage imageNamed:selImageName]:nil)];
}

- (void)setImageWithImageName:(NSString *)imageName
{
    if (imageName) {
        [self setImage:[UIImage imageNamed:imageName] forState:UIControlStateNormal];
    } else {
        [self setImage:nil forState:UIControlStateNormal];
    }
}

//- (void)setBackgroundImageAndHightlightWithColorHex:(NSString *)colorHex
//{
//    [self setBackgroundImage:[DTPubUtil imageWithColorString:colorHex] forState:UIControlStateNormal];
//    [self setBackgroundImage:[DTPubUtil imageWithColorString:FFHighlightColorStr(colorHex)] forState:UIControlStateHighlighted];
//}
//
//- (void)setBackgroundImageAndHightlightWithColorHex:(NSString *)colorHex cornerRadius:(CGFloat)cornerRadius
//{
//    if (cornerRadius <= 0.f) {
//        [self setBackgroundImageAndHightlightWithColorHex:colorHex];
//        return;
//    }
//    [self setBackgroundImage:[DTPubUtil imageWithColorString:colorHex cornerRadius:cornerRadius] forState:UIControlStateNormal];
//    [self setBackgroundImage:[DTPubUtil imageWithColorString:FFHighlightColorStr(colorHex) cornerRadius:cornerRadius] forState:UIControlStateHighlighted];
//}

- (void)addTarget:(id)target action:(SEL)action
{
    [self addTarget:target action:action forControlEvents:UIControlEventTouchUpInside];
}

//- (void)setTitleEdgeInsetsForIphoneX
//{
//    if ([DTPubUtil isIPhoneX]) {
//        UIEdgeInsets inset = UIEdgeInsetsMake(SAFE_BOTTOM_TITLE_EDGE_TOP_HEIGHT, 0, 0, 0);
//        [self setTitleEdgeInsets:inset];
//    }
//}
//
//- (void)setTitleEdgeInsetsForIphoneXWithEdgeInset:(UIEdgeInsets)fixEdgeInset
//{
//    if ([DTPubUtil isIPhoneX]) {
//        [self setTitleEdgeInsets:fixEdgeInset];
//    }
//}
//
//- (void)setImageEdgeInsetsForIphoneX
//{
//    if ([DTPubUtil isIPhoneX]) {
//        UIEdgeInsets inset = UIEdgeInsetsMake(SAFE_BOTTOM_TITLE_EDGE_TOP_HEIGHT, 0, 0, 0);
//        [self setImageEdgeInsets:inset];
//    }
//}
//
//- (void)setImageEdgeInsetsForIphoneXWithEdgeInset:(UIEdgeInsets)fixEdgeInset
//{
//    if ([DTPubUtil isIPhoneX]) {
//        [self setImageEdgeInsets:fixEdgeInset];
//    }
//}

@end
