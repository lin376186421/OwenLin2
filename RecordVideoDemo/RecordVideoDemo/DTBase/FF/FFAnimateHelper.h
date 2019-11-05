//
//  FFAnimateHelper.h
//  FFStory
//
//  Created by PageZhang on 14/12/9.
//  Copyright (c) 2014年 PageZhang. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, FFAnimateType) {
    FFAnimateTypeNone, // animated = NO
    FFAnimateTypeDefault, // animated = YES
    FFAnimateTypeFade,
    FFAnimateTypePushPop,
    FFAnimateTypeFlipX,
    FFAnimateTypeFlipY,
    FFAnimateTypeCubeX,
    FFAnimateTypeCubeY,
    FFAnimateTypeCurlX,
    FFAnimateTypeCurlY,
    FFAnimateTypeRipple, // 水滴效果
    FFAnimateTypeSuck, // 神奇效果
    FFAnimateTypeCamera // 摄像头打开关闭效果
};

@interface CATransition (Utils)
+ (instancetype)transitionIn:(FFAnimateType)type;
+ (instancetype)transitionOut:(FFAnimateType)type;
@end

// Removes a lot of boiler plate by properly setting up the animation context (smartly only if animationTime is > 0)
// Also enables rasterization just until the animation is done (if we animate)
extern void FFSpringAnimateWithBlock(CGFloat duration, CGFloat delay, void (^actionBlock)(void), void (^completionBlock)(BOOL finished));

// Removes animations in view and all subviews
extern void FFRemoveAnimationsRecursively(UIView *view);

/* 颜色：fromColor为nil，currentColor -> toColor -> currentColor */
extern CAAnimation *FFFlashAnimation(CGFloat duration, UIColor *toColor, UIColor *fromColor);

/* 旋转：顺时针转360度，toRadians=@(M_PI*2)；fromRadians=nil */
extern CAAnimation *FFRotateAnimation(CGFloat duration, NSNumber *toRadians, NSNumber *fromRadians);

/* 左右抖动：不会停止 */
extern CAAnimation *FFShakeAnimation(CGFloat duration, NSUInteger degress);
