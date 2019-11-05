//
//  DrawView.m
//  PodTest
//
//  Created by 林 on 16/12/1.
//  Copyright © 2016年 林. All rights reserved.
//

#import "DrawView.h"

@interface DrawView()
{
    NSMutableArray *rects;
}
@end

@implementation DrawView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor blackColor];
        [self cutViewFrame];
//        [self setNeedsDisplay];//强制调用drawRect：
    }
    return self;
}

- (void)cutViewFrame
{
    rects = [NSMutableArray array];
    CGSize size = [UIScreen mainScreen].bounds.size;
    CGFloat vw = 60, vh = 60;
    NSUInteger h = size.width/vw,v = size.height/vh;
    for (int i = 0; i < h ; i ++) {
        for (int j = 0; j < v; j++) {
            CGRect rect;
            rect.origin.x = (i*vw);
            rect.origin.y = (j*vh);
            rect.size = CGSizeMake(vw, vh);
            NSString *rectStr = NSStringFromCGRect(rect);
            [rects addObject:rectStr];
        }
    }
}


- (CGRect)getRectWithIndex:(NSUInteger *)index
{
    NSString *rectStr = [rects objectAtIndex:*index];
    *index += 1;
    return CGRectFromString(rectStr);
}

- (void)drawTestRect:(CGRect)rect
{
    //两种方式
    NSUInteger rand = arc4random() % 100;
    if (rand < 50) {
        UIColor *color = [UIColor whiteColor];
        [color set];
        UIBezierPath *path = [UIBezierPath bezierPathWithRect:rect];
        path.lineWidth = 1;//宽度
        [path stroke];
        color = [UIColor colorWithWhite:1 alpha:0.2];
        [color set];
        [path fill];//填充颜色
    } else {
        
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGContextSetLineWidth(context, 1.f);
        CGContextSetStrokeColorWithColor(context, [UIColor whiteColor].CGColor);
        CGContextStrokeRect(context, rect);
        CGContextDrawPath(context, kCGPathStroke);
        
        CGContextSetFillColorWithColor(context, [UIColor colorWithWhite:1 alpha:0.2].CGColor);
        CGContextFillRect(context, rect);
        CGContextDrawPath(context, kCGPathFill);//填充颜色
    }
//    NSUInteger rand = arc4random() % 100;
//    if (rand < 50) {
//
//    } else {
//
//    }
    
}

- (void)drawText:(NSString *)text rect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [UIColor whiteColor].CGColor);
    UIFont *font = [UIFont systemFontOfSize:18];
    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    style.alignment = NSTextAlignmentCenter;
    [text drawInRect:rect withAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor],NSFontAttributeName:font,
                                           NSParagraphStyleAttributeName:style}];
}

- (void)drawCircle:(CGRect)rect
{
    //纯色的圆形image
//    CGRect rect = CGRectMake(0.0f, 0.0f, 4.0f, 4.0f);
//    UIGraphicsBeginImageContextWithOptions(rect.size, NO, 0);
//    CGContextRef context = UIGraphicsGetCurrentContext();
//    CGContextSetFillColorWithColor(context, [[UIColor colorWithHexString:@"d9d9d9"] CGColor]);
//    CGContextAddArc(context, rect.origin.x+rect.size.width/2, rect.origin.y+rect.size.height/2, rect.size.width/2, 0, 2*M_PI, YES);
//    CGContextFillPath(context);
//    //    CGContextFillRect(context, rect);
//    //    cgcon
//    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
//    UIGraphicsEndImageContext();

    
    NSUInteger rand = arc4random() % 100;
    rand = 51;
    if (rand < 50) {
        UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:rect cornerRadius:rect.size.width/2];
        path.lineWidth = 1;
        UIColor *color = [UIColor redColor];
        [color set];
        [path stroke];
        color = [UIColor yellowColor];
        [color set];
        [path fill];
    } else {
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
        CGContextSetFillColorWithColor(context, [UIColor yellowColor].CGColor);
        CGContextAddArc(context, rect.origin.x+rect.size.width/2, rect.origin.y+rect.size.height/2, rect.size.width/2, 0, 2*M_PI, YES);
        CGContextSetLineWidth(context, 5);
        CGContextDrawPath(context, kCGPathFillStroke);
    }
}

//四边形
- (void)draw4Shape:(CGRect)targetRect
{
    //画四边形
    NSUInteger rand = arc4random() % 100 + 60;
    if (rand < 50) {
        UIBezierPath *path = [UIBezierPath bezierPath];
        [path moveToPoint:CGPointMake(targetRect.size.width/2 + targetRect.origin.x, targetRect.origin.y)];
        [path addLineToPoint:CGPointMake(targetRect.size.width + targetRect.origin.x, targetRect.origin.y + targetRect.size.height/2)];
        [path addLineToPoint:CGPointMake(targetRect.size.width/2 + targetRect.origin.x, targetRect.origin.y + targetRect.size.height)];
        [path addLineToPoint:CGPointMake(targetRect.origin.x, targetRect.origin.y + targetRect.size.height/2)];
        [path closePath];//最后一条线自动连接
        path.lineCapStyle = kCGLineCapRound; //线条拐角
        path.lineJoinStyle = kCGLineJoinRound;//终点处理
        path.lineWidth = 1;//宽度 也可以理解为描边的宽度
        UIColor *color = [UIColor redColor];
        [color set];
        [path stroke];//路径绘制
        color = [UIColor blueColor];
        [color set];
        [path fill];//路径内部绘制
        [self drawText:@"bez" rect:targetRect];

    } else {
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGContextSetLineWidth(context, 1);
        CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
        CGPoint pos[4];
        pos[0] = CGPointMake(targetRect.size.width/2 + targetRect.origin.x, targetRect.origin.y);
        pos[1] = CGPointMake(targetRect.size.width + targetRect.origin.x, targetRect.origin.y + targetRect.size.height/2);
        pos[2] = CGPointMake(targetRect.size.width/2 + targetRect.origin.x, targetRect.origin.y + targetRect.size.height);
        pos[3] = CGPointMake(targetRect.origin.x, targetRect.origin.y + targetRect.size.height/2);
        CGContextAddLines(context, pos, 4);
        CGContextClosePath(context);//最后一条线自动连接
        CGContextSetFillColorWithColor(context, [UIColor blueColor].CGColor);
        CGContextDrawPath(context, kCGPathFillStroke);//填充和描边
        [self drawText:@"con" rect:targetRect];
    }
}

//圆角四边形-----====`````111
- (void)drawCircleShape:(CGRect)rect
{
    CGFloat radius = 10;
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextMoveToPoint(context, rect.origin.x, 10 + rect.origin.y);
    CGContextAddArcToPoint(context, rect.origin.x + 0, rect.origin.y + 0, rect.origin.x + 10, rect.origin.y, radius);
    CGContextAddArcToPoint(context, rect.origin.x + rect.size.width, rect.origin.y, rect.origin.x + rect.size.width, rect.origin.y+10, radius);
    CGContextAddArcToPoint(context, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height, rect.origin.x + rect.size.width - 10, rect.origin.y + rect.size.height , radius);
    CGContextAddArcToPoint(context, rect.origin.x , rect.origin.y + rect.size.height, rect.origin.x + 0, rect.origin.y + rect.size.height - 10, radius);
    CGContextClosePath(context);
    CGContextSetFillColorWithColor(context, [UIColor redColor].CGColor);
    CGContextSetStrokeColorWithColor(context, [UIColor whiteColor].CGColor);
    CGContextDrawPath(context, kCGPathFillStroke);
}

- (void)drawRect:(CGRect)rect {
    
    [super drawRect:rect];
    
    NSUInteger index = 0;
    CGRect targetRect = [self getRectWithIndex:&index];
    [self drawTestRect:targetRect];
    [self draw4Shape:targetRect];
    
    targetRect = [self getRectWithIndex:&index];
    [self drawTestRect:targetRect];
    [self drawCircle:targetRect];
    
    targetRect = [self getRectWithIndex:&index];
    [self drawTestRect:targetRect];
    [self drawCircleShape:targetRect];

}


@end
