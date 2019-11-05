//
//  UILabel+Label.m
//  soshow.org
//
//  Created by Users on 13-9-2.
//  Copyright (c) 2013年 sos. All rights reserved.
//

#import "UILabel+Label.h"

@implementation UILabel (Label)

+ (UILabel *)frame:(CGRect)frame BlackName:(NSString *)name size:(CGFloat)fontSize
{
    UILabel * label = [[UILabel alloc] initWithFrame:frame];
    [label setBackgroundColor:[UIColor clearColor]];
    [label setTextColor:[UIColor blackColor]];
    [label setFont:[UIFont fontWithName:@"Helvetica-Bold" size:fontSize]];
    [label setTextAlignment:1];
    [label setText:name];
    return label;
}
+ (UILabel *)frame:(CGRect)frame WhiteName:(NSString *)name size:(CGFloat)fontSize
{
    UILabel * label = [[UILabel alloc] initWithFrame:frame];
    [label setFont:[UIFont fontWithName:@"Helvetica-Bold" size:fontSize]];
    [label setBackgroundColor:[UIColor clearColor]];
    [label setTextColor:[UIColor whiteColor]];
    [label setTextAlignment:1];
    [label setText:name];
    return label;
}
+ (UILabel *)frame:(CGRect)frame name:(NSString *)name size:(CGFloat)fontSize andTextColor:(UIColor *)color
{
    UILabel * label = [[UILabel alloc] initWithFrame:frame];
    
    [label setBackgroundColor:[UIColor clearColor]];
    label.textColor = color;
    //[label setFont:[UIFont fontWithName:@"Helvetica-Bold" size:fontSize]];
    [label setFont:[UIFont systemFontOfSize:fontSize]];
    [label setTextAlignment:1];
    [label setText:name];
    return label;
}

+ (UILabel *)frame:(CGRect)frame name:(NSString *)name size:(CGFloat)fontSize andTextColor:(UIColor *)color bgColor:(UIColor *)bgColor tag:(int)tag
{
    UILabel * label = [[UILabel alloc] initWithFrame:frame];
    
    [label setBackgroundColor:bgColor];
    label.textColor = color;
    [label setFont:[UIFont systemFontOfSize:fontSize]];
    [label setTextAlignment:0];
    [label setText:name];
    [label setTag:tag];
    return label;
}

+ (NSMutableAttributedString *)getAttibutedStringWithColor:(UIColor *)color withString:(NSString *)string
{
    NSMutableAttributedString *str = [[NSMutableAttributedString alloc] initWithString:string];
    [str addAttribute:NSForegroundColorAttributeName value:color range:NSMakeRange(0,string.length - 1)];
    return str;
}

@end

