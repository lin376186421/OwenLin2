//
//  FFAnimateHelper.m
//  FFStory
//
//  Created by PageZhang on 14/12/9.
//  Copyright (c) 2014年 PageZhang. All rights reserved.
//

#import "FFAnimateHelper.h"
#import "FFLogging.h"
/*
 kCATransitionFade   交叉淡化过渡
 kCATransitionMoveIn 新视图移到旧视图上面
 kCATransitionPush   新视图把旧视图推出去
 kCATransitionReveal 将旧视图移开,显示下面的新视图
 pageCurl       向上翻一页
 pageUnCurl     向下翻一页
 rippleEffect   水滴效果
 suckEffect     神奇效果
 cube           立方体效果
 oglFlip        上下翻转效果
 cameraIrisHollowOpen 摄像头打开效果
 cameraIrisHollowClose 摄像头关闭效果
 */
@implementation CATransition (Utils)
+ (instancetype)transition:(CGFloat)duration {
    CATransition *transition = [CATransition animation];
    transition.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    transition.duration = duration;
    return transition;
}
+ (instancetype)transitionIn:(FFAnimateType)type {
    CATransition *transition = [self transition:0.38];
    switch (type) {
        case FFAnimateTypeFade: {
            transition.type = kCATransitionFade;
        } break;
        case FFAnimateTypePushPop: {
            transition.type = kCATransitionMoveIn;
            transition.subtype = kCATransitionFromRight;
        } break;
        case FFAnimateTypeFlipX: {
            transition.type = @"oglFlip";
            transition.subtype = kCATransitionFromRight;
        } break;
        case FFAnimateTypeFlipY: {
            transition.type = @"oglFlip";
            transition.subtype = kCATransitionFromTop;
        } break;
        case FFAnimateTypeCubeX: {
            transition.type = @"cube";
            transition.subtype = kCATransitionFromRight;
        } break;
        case FFAnimateTypeCubeY: {
            transition.type = @"cube";
            transition.subtype = kCATransitionFromTop;
        } break;
        case FFAnimateTypeCurlX: {
            transition.type = @"pageCurl";
            transition.subtype = kCATransitionFromRight;
        } break;
        case FFAnimateTypeCurlY: {
            transition.type = @"pageCurl";
            transition.subtype = kCATransitionFromBottom;
        } break;
        case FFAnimateTypeRipple: {
            transition.duration = 1.75;
            transition.type = @"rippleEffect";
        } break;
        case FFAnimateTypeSuck: {
            transition.duration = 0.5;
            transition.type = @"suckEffect";
        } break;
        case FFAnimateTypeCamera: {
            transition.duration = 0.65;
            transition.type = @"cameraIrisHollowOpen";
        } break;
        default: break;
    }
    return transition;
}

+ (instancetype)transitionOut:(FFAnimateType)type {
    CATransition *transition = [self transition:0.38];
    switch (type) {
        case FFAnimateTypeFade: {
            transition.type = kCATransitionFade;
        } break;
        case FFAnimateTypePushPop: {
            transition.type = kCATransitionReveal;
            transition.subtype = kCATransitionFromLeft;
        } break;
        case FFAnimateTypeFlipX: {
            transition.type = @"oglFlip";
            transition.subtype = kCATransitionFromLeft;
        } break;
        case FFAnimateTypeFlipY: {
            transition.type = @"oglFlip";
            transition.subtype = kCATransitionFromBottom;
        } break;
        case FFAnimateTypeCubeX: {
            transition.type = @"cube";
            transition.subtype = kCATransitionFromLeft;
        } break;
        case FFAnimateTypeCubeY: {
            transition.type = @"cube";
            transition.subtype = kCATransitionFromBottom;
        } break;
        case FFAnimateTypeCurlX: {
            transition.type = @"pageUnCurl";
            transition.subtype = kCATransitionFromRight;
        } break;
        case FFAnimateTypeCurlY: {
            transition.type = @"pageUnCurl";
            transition.subtype = kCATransitionFromBottom;
        } break;
        case FFAnimateTypeRipple: {
            transition.duration = 1.75;
            transition.type = @"rippleEffect";
        } break;
        case FFAnimateTypeSuck: {
            transition.duration = 0.5;
            transition.type = @"suckEffect";
        } break;
        case FFAnimateTypeCamera: {
            transition.duration = 0.65;
            transition.type = @"cameraIrisHollowClose";
        } break;
        default: break;
    }
    return transition;
}
@end

void FFSpringAnimateWithBlock(CGFloat duration, CGFloat delay, void (^actionBlock)(void), void (^completionBlock)(BOOL finished)) {
    FFAssertIfNotMainThread();
    if (duration > 0.001f || delay > 0.001f) {
        const CGFloat durationExpansionFactor = 2.4f;
        [UIView animateWithDuration:durationExpansionFactor * duration
                              delay:durationExpansionFactor * delay
             usingSpringWithDamping:0.75f
              initialSpringVelocity:1.5f
                            options:UIViewAnimationOptionBeginFromCurrentState|UIViewAnimationOptionAllowUserInteraction
                         animations:^{
                             if (actionBlock) actionBlock();
                         }
                         completion:^(BOOL finished) {
                             if (completionBlock) completionBlock(finished);
                         }];
    }else {
        actionBlock();
        if (completionBlock) completionBlock(YES);
    }
}

void FFRemoveAnimationsRecursively(UIView *view) {
    [view.layer removeAllAnimations];
    for (UIView *subview in view.subviews) {
        FFRemoveAnimationsRecursively(subview);
    }
}

#pragma mark - Animations
CAAnimation *FFFlashAnimation(CGFloat duration, UIColor *toColor, UIColor *fromColor) {
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"backgroundColor"];
    if (fromColor) {
        animation.fromValue = (id)fromColor.CGColor;
    } else {
        animation.autoreverses = YES;
    }
    animation.toValue = (id)toColor.CGColor;
    animation.duration = duration;
    return animation;
}

CAAnimation *FFRotateAnimation(CGFloat duration, NSNumber *toRadians, NSNumber *fromRadians) {
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
    animation.fromValue = fromRadians;
    animation.toValue = toRadians;
    animation.duration = duration;
    return animation;
}


CAAnimation *FFShakeAnimation(CGFloat duration, NSUInteger degress) {
    CAKeyframeAnimation *animation = [CAKeyframeAnimation animationWithKeyPath:@"transform.rotation.z"];
    animation.values = @[@(0), @(-FFDegreesToRadians(degress)), @(0), @(FFDegreesToRadians(degress)), @(0)];
    animation.duration = duration;
    animation.repeatCount = HUGE_VALF;
    return animation;
}

