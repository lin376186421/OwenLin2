//
//  FFPixelLine.m
//  FFStory
//
//  Created by PageZhang on 15/6/17.
//  Copyright (c) 2015年 PageZhang. All rights reserved.
//

#import "FFPixelLine.h"

@implementation FFPixelLine
- (void)awakeFromNib {
    [super awakeFromNib];
    self.backgroundColor = [UIColor clearColor];
}
- (void)drawRect:(CGRect)rect {
    self.backgroundColor = [UIColor clearColor];
    CGFloat linewidth = 1.0/[UIScreen mainScreen].scale;
    CGContextRef context = UIGraphicsGetCurrentContext();
    UIColor *strokeColor = _stokeColor ?: WHITE_255(230);
    CGContextSetStrokeColorWithColor(context, strokeColor.CGColor);
    CGContextSetLineWidth(context, linewidth);
    switch (_position) {
        case FFLinePositionTop: {
            CGContextMoveToPoint(context, 0, linewidth*0.5);
            CGContextAddLineToPoint(context, CGRectGetMaxX(rect), linewidth*0.5);
        } break;
        case FFLinePositionLeft: {
            CGContextMoveToPoint(context, linewidth*0.5, 0);
            CGContextAddLineToPoint(context, linewidth*0.5, CGRectGetMaxY(rect));
        } break;
        case FFLinePositionBottom: {
            CGContextMoveToPoint(context, 0, CGRectGetMaxY(rect)-linewidth*0.5);
            CGContextAddLineToPoint(context, CGRectGetMaxX(rect), CGRectGetMaxY(rect)-linewidth*0.5);
        } break;
        case FFLinePositionRight: {
            CGContextMoveToPoint(context, CGRectGetMaxX(rect)-linewidth*0.5, 0);
            CGContextAddLineToPoint(context, CGRectGetMaxX(rect)-linewidth*0.5, CGRectGetMaxY(rect));
        } break;
        case FFLinePositionTLBR: {
            CGContextMoveToPoint(context, 0, 0);
            CGContextAddLineToPoint(context, CGRectGetMaxX(rect), CGRectGetMaxY(rect));
        } break;
        case FFLinePositionBLTR: {
            CGContextMoveToPoint(context, 0, CGRectGetMaxY(rect));
            CGContextAddLineToPoint(context, CGRectGetMaxX(rect), 0);
        } break;
        default: break;
    }
    CGContextStrokePath(context);
}
@end
