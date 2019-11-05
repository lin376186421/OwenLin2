//
//  UIView+Utils.m
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "UIView+Utils.h"

@implementation UIView (Utils)

- (CGFloat)top {
    return CGRectGetMinY(self.frame);
}
- (void)setTop:(CGFloat)top {
    CGRect frame = self.frame;
    frame.origin.y = top;
    self.frame = frame;
}

- (CGFloat)left {
    return CGRectGetMinX(self.frame);
}
- (void)setLeft:(CGFloat)left {
    CGRect frame = self.frame;
    frame.origin.x = left;
    self.frame = frame;
}

- (CGFloat)bottom {
    return CGRectGetMaxY(self.frame);
}
- (void)setBottom:(CGFloat)bottom {
    CGRect frame = self.frame;
    frame.origin.y = bottom - self.height;
    self.frame = frame;
}

- (CGFloat)right {
    return CGRectGetMaxX(self.frame);
}
- (void)setRight:(CGFloat)right {
    CGRect frame = self.frame;
    frame.origin.x = right - self.width;
    self.frame = frame;
}

- (CGFloat)width {
    return CGRectGetWidth(self.frame);
}
- (void)setWidth:(CGFloat)width {
    CGRect frame = self.frame;
    frame.size.width = width;
    self.frame = frame;
}

- (CGFloat)height {
    return CGRectGetHeight(self.frame);
}
- (void)setHeight:(CGFloat)height {
    CGRect frame = self.frame;
    frame.size.height = height;
    self.frame = frame;
}

- (CGSize)size {
    return self.frame.size;
}
- (void)setSize:(CGSize)size {
    CGRect frame = self.frame;
    frame.size = size;
    self.frame = frame;
}

- (CGPoint)origin {
    return self.frame.origin;
}
- (void)setOrigin:(CGPoint)origin {
    CGRect frame = self.frame;
    frame.origin = origin;
    self.frame = frame;
}

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

#pragma mark - actions

- (UIImage *)capturedImage {
    UIGraphicsBeginImageContextWithOptions(self.size, NO, 0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    [self.layer renderInContext:context];
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

- (void)startTransformAni
{
    CAKeyframeAnimation *theAnimation = [CAKeyframeAnimation animation];
    theAnimation.values = [NSArray arrayWithObjects:
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, -15, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, 0, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, -15, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, 0, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, 0, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, 0, 0)],
                           [NSValue valueWithCATransform3D:CATransform3DMakeTranslation(0, 0, 0)],
                           nil];
    theAnimation.cumulative = YES;
    theAnimation.duration = 2.f;
    theAnimation.repeatCount = HUGE_VALF;
    theAnimation.removedOnCompletion = NO;
    [self.layer addAnimation:theAnimation forKey:@"transform"];
    //    CABasicAnimation
    
//    CAKeyframeAnimation *ani = [CAKeyframeAnimation animationWithKeyPath:@"transform.translation.y"];
//    ani.values = [NSArray arrayWithObjects:
//                  [NSNumber numberWithFloat:0],
//                  [NSNumber numberWithFloat:-10],
//                  [NSNumber numberWithFloat:0],
//                  [NSNumber numberWithFloat:-10],
//                  [NSNumber numberWithFloat:0],
//                  [NSNumber numberWithFloat:0],
//                  [NSNumber numberWithFloat:0],
//                  [NSNumber numberWithFloat:0],
//                  nil];
//    ani.cumulative = YES;
//    ani.duration = 2.3f;
//    ani.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear];
//    ani.repeatCount = HUGE_VALF;
//    //    ani.autoreverses = YES;
//    ani.removedOnCompletion = NO;
//    [self.layer addAnimation:ani forKey:@"move_forever"];
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


