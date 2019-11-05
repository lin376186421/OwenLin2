//
//  UIView+Utils.m
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "UIView+Utils.h"

@implementation UIView (Utils)

- (CGFloat)centerX {
    return self.center.x;
}

- (void)setCenterX:(CGFloat)centerX {
    self.center = CGPointMake(centerX, self.center.y);
}

- (CGFloat)centerY {
    return self.center.y;
}

- (void)setCenterY:(CGFloat)centerY {
    self.center = CGPointMake(self.center.x, centerY);
}

- (void)setFixRightWidth:(CGFloat)width
{
    [self setFixRight:self.right width:width];
}

- (void)setFixRight:(CGFloat)right width:(CGFloat)width
{
    self.frame = CGRectMake(right - width, self.top, width, self.height);
}

- (void)setFixBottomHeight:(CGFloat)height
{
    [self setFixBottom:self.bottom height:height];
}

- (void)setFixBottom:(CGFloat)bottom height:(CGFloat)height
{
    self.frame = CGRectMake(self.left, self.bottom - height, self.width, height);
}

- (void)setFixCenterWidth:(CGFloat)width
{
    [self setFixCenterWidth:width height:self.height];
}

- (void)setFixCenterHeight:(CGFloat)height
{
    [self setFixCenterWidth:self.width height:height];
}

- (void)setFixCenterWidth:(CGFloat)width height:(CGFloat)height
{
    self.frame = CGRectMake(self.centerX - width/2, self.centerY - height/2, width, height);
}

- (void)setTop:(CGFloat)top andHeight:(CGFloat)height
{
    self.frame = CGRectMake(self.left, top, self.width, height);
}

- (void)setTop:(CGFloat)top andWidth:(CGFloat)width
{
    self.frame = CGRectMake(self.left, top, width, self.height);
}

- (void)setLeft:(CGFloat)left andWidth:(CGFloat)width
{
    self.frame = CGRectMake(left, self.top, width, self.height);
}

#pragma mark - actions

- (UIImage *)capturedImage {
    UIGraphicsBeginImageContextWithOptions(self.size, NO, 0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    [self.layer renderInContext:context];
    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (UIImage *)capturedImageWithRect:(CGRect)rect {
    UIGraphicsBeginImageContextWithOptions(rect.size, NO, 0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    [self.layer renderInContext:context];
    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (UIImage *)hierarchyImage
{
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, NO, 0);
    [self drawViewHierarchyInRect:self.bounds afterScreenUpdates:NO];
    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (UIImage *)hierarchyImageWithRect:(CGRect)rect
{
    UIGraphicsBeginImageContextWithOptions(rect.size, NO, 0);
    [self drawViewHierarchyInRect:self.bounds afterScreenUpdates:NO];
    UIImage *img = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return img;
}

- (void)recursiveSubviews {
    [self.subviews enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        NSLog(@"%@", obj);
        [obj recursiveSubviews];
    }];
}

- (void)removeAllSubviews {
    [self.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];
}

- (UIView *)findFirstResponderView {
    // Stop if e.g. we show a UIAlertView with a text field.
    if (UIApplication.sharedApplication.keyWindow != self.window) return nil;
    
    // Search recursively for first responder.
    for (UIView *childView in self.subviews) {
        if ([childView respondsToSelector:@selector(isFirstResponder)] && childView.isFirstResponder)
            return childView;
        UIView *result = [childView findFirstResponderView];
        if (result) return result;
    }
    return nil;
}

- (UIViewController *)findResponderViewController {
    for (UIView *next = [self superview]; next; next = next.superview) {
        UIResponder *nextResponder = [next nextResponder];
        if ([nextResponder isKindOfClass:[UIViewController class]]) {
            return (UIViewController *)nextResponder;
        }
    }
    return nil;
}

- (void)addTarget:(id)target singleTapAction:(SEL)action {
    self.userInteractionEnabled = YES;
    [self addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:target action:action]];
}

- (void)addTarget:(id)target longPressAction:(SEL)action {
    self.userInteractionEnabled = YES;
    [self addGestureRecognizer:[[UILongPressGestureRecognizer alloc] initWithTarget:target action:action]];
}

- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColor:(UIColor *)borderColor
{
    [self setLayerBorderWidth:borderWidth borderColor:borderColor cornerRadius:0.f];
}

- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColorStr:(NSString *)borderColorStr
{
    [self setLayerBorderWidth:borderWidth borderColorStr:borderColorStr cornerRadius:0.f];
}

- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColorStr:(NSString *)borderColorStr cornerRadius:(CGFloat)cornerRadius
{
    [self setLayerBorderWidth:borderWidth borderColor:[UIColor colorWithString:borderColorStr] cornerRadius:cornerRadius];
}

- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColor:(UIColor *)borderColor cornerRadius:(CGFloat)cornerRadius
{
    self.layer.borderWidth = borderWidth;
    if (borderColor) {
        self.layer.borderColor = borderColor.CGColor;
    } else {
        self.layer.borderColor = [UIColor clearColor].CGColor;
    }
    self.cornerRadius = cornerRadius;
    self.layer.masksToBounds = YES;
}

- (void)setBackgroundColorString:(NSString *)colorString
{
    if (colorString.length) {
        [self setBackgroundColor:[UIColor colorWithString:colorString]];
    }
}

+ (UIView *)clearColorView:(CGRect)frame
{
    UIView *view = [[UIView alloc] initWithFrame:frame];
    view.backgroundColor = [UIColor clearColor];
    return view;
}

//- (void)addBottomLine
//{
//    [self addBottomLineWithColorStr:@"d9d9d9"];
//}

//- (void)addBottomLineWithColorStr:(NSString *)colorStr
//{
//    BPOnePixLineView *line = [[BPOnePixLineView alloc] initWithFrame:CGRectMake(0, self.height-1, self.width, 1)];
//    line.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleTopMargin;
//    line.lineColor = [UIColor colorWithHexString:colorStr];
//    [self addSubview:line];
//}
//
//- (void)addTopLine
//{
//    [self addTopLineWithColorStr:@"d9d9d9"];
//}
//
//- (void)addTopLineWithColorStr:(NSString *)colorStr
//{
//    BPOnePixLineView *line = [[BPOnePixLineView alloc] initWithFrame:CGRectMake(0, 0, self.width, 1)];
//    line.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleBottomMargin;
//    line.direction = BPOnePixLineDirectionTopOrLeft;
//    line.lineColor = [UIColor colorWithHexString:colorStr];
//    [self addSubview:line];
//}

- (UIViewController *)ownerViewController
{
    UIResponder *next = self.nextResponder;
    do {
        if ([next isKindOfClass:[UIViewController class]]) {
            return (UIViewController *)next;
        } else if ([next isKindOfClass:[UINavigationController class]]) {
            UINavigationController *nav = (UINavigationController *)next;
            return nav.viewControllers.lastObject;
        }
        next = next.nextResponder;
    }while (next != nil);
    return nil;
}

- (UIView *)gaussView:(UIView *)view
{
    view.contentMode = UIViewContentModeScaleAspectFit;
    UIBlurEffect *blur = [UIBlurEffect effectWithStyle:UIBlurEffectStyleLight];
    UIVisualEffectView *effectview = [[UIVisualEffectView alloc] initWithEffect:blur];
    effectview.frame = view.bounds;
    [view addSubview:effectview];
    return view;
}

@end


@implementation UIView (Animate)

- (void)expandAnimated:(CGRect)rect {
    CGFloat x = CGRectGetMidX(rect);
    CGFloat y = CGRectGetMidY(rect);
    CGFloat w = x*2 > self.width ? x : self.width-x;
    CGFloat h = y*2 > self.height ? y : self.height-y;
    CGFloat r = sqrtf(pow(w, 2) + pow(h, 2));
    
    CAShapeLayer *maskLayer = [CAShapeLayer layer];
    maskLayer.path = [UIBezierPath bezierPathWithOvalInRect:CGRectInset(rect, -r, -r)].CGPath;
    self.layer.mask = maskLayer;
    
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"path"];
    animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];
    animation.fromValue = (id)([UIBezierPath bezierPathWithOvalInRect:rect].CGPath);
    animation.duration = r/800;
    [maskLayer addAnimation:animation forKey:@"path"];
}

@end

#import <objc/runtime.h>
static NSString *const FFSuppressLayoutKey = @"suppressSetNeedsLayout";

@interface FFSuppressLayoutTriggerLayer : CALayer @end
@implementation FFSuppressLayoutTriggerLayer
- (void)setNeedsLayout {
    if (![[self valueForKey:FFSuppressLayoutKey] boolValue]) {
        [super setNeedsLayout];
    }
}
@end

@implementation UIView (Hook)

- (void)performWithoutTriggeringSetNeedsLayout:(dispatch_block_t)block {
    CALayer *layer = self.layer;
    // Change layer to be our custom subclass.
    if (![layer isKindOfClass:FFSuppressLayoutTriggerLayer.class]) {
        // Check both classes to see and break if KVO is used here.
        if ([layer.class isEqual:CALayer.class] && [layer.class isEqual:object_getClass(layer)]) {
            object_setClass(self.layer, FFSuppressLayoutTriggerLayer.class);
        } else {
            // While we could use dynamic subclassing, that amount of complexity isn't needed in our case.
            // If we're a different layer type, the generic KVC store value is simply ignored, so no need to quit.
            NSLog(@"View has a custom layer - not changing.");
        }
    }
    if (![[layer valueForKey:FFSuppressLayoutKey] boolValue]) {
        [layer setValue:@YES forKey:FFSuppressLayoutKey];
        block();
        [layer setValue:@NO forKey:FFSuppressLayoutKey];
    }else {
        // No need to set flag again. Allows to be called this multiple times.
        block();
    }
}

@end


