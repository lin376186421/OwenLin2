//
//  UILabel+Additions.h
//  DrivingTest
//
//  Created by cheng on 15/12/31.
//  Copyright © 2015年 eclicks. All rights reserved.
//

#import <UIKit/UIKit.h>
//#import "TTTAttributedLabel.h"
//#import "NSAttributedString+Utils.h"

//extern CTFontRef CTFontCreateFromUIFont(UIFont *font);

@interface UILabel (Additions)

//- (CGSize)measureSize;

+ (id)labelWithFrame:(CGRect)frame font:(UIFont *)font color:(UIColor *)color;

+ (id)labelWithFrame:(CGRect)frame font:(UIFont *)font colorString:(NSString *)colorString;

+ (id)labelWithFrame:(CGRect)frame fontSize:(CGFloat)fontSize color:(UIColor *)color;

+ (id)labelWithFrame:(CGRect)frame fontSize:(CGFloat)fontSize colorString:(NSString *)colorString;

- (void)setText:(NSString *)text highLightText:(NSString *)string highLightTextColor:(UIColor *)color;

- (void)setLineBreakByTruncatingLastTailIndent:(CGFloat)tailIndent; //最后一行设置缩进 无返回值
- (BOOL)lastLineShouldTailIndent:(CGFloat)tailIndent; //最后一行设置缩进 有返回值

- (void)textAlignmentLeftAndRightWith:(CGFloat)labelWidth;

- (CGSize)measureSize;
@end

//@interface UILabel (Attributed)
//
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withFont:(UIFont *)font;
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color;
//- (void)setText:(NSString *)text highLightText:(NSString *)highLightText withColor:(UIColor *)color withFont:(UIFont*)font;
//
////高亮数组
//- (void)setText:(NSString *)text highLightTextArray:(NSArray *)highLightTextArray withColor:(UIColor *)color;
//
//@end

//@interface TTTAttributedLabel (HighLightText)
//
//- (void)setText:(NSString *)text highLightRange:(NSRange)range withColor:(UIColor *)color;
////配置链接 颜色， 无下划线
//- (void)setLinkTextColor:(UIColor *)color highLightColor:(UIColor *)hColor;
//- (void)setLinkTextColorStr:(NSString *)colorString highLightColorStr:(NSString *)hColorString;
//
//@end

