//
//  UIView+Utils.h
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (Utils)

@property (nonatomic) CGFloat centerX;
@property (nonatomic) CGFloat centerY;

- (void)setFixRightWidth:(CGFloat)width;
- (void)setFixRight:(CGFloat)right width:(CGFloat)width;

- (void)setFixBottomHeight:(CGFloat)height;
- (void)setFixBottom:(CGFloat)bottom height:(CGFloat)height;

- (void)setFixCenterWidth:(CGFloat)width;
- (void)setFixCenterHeight:(CGFloat)height;
- (void)setFixCenterWidth:(CGFloat)width height:(CGFloat)height;

- (void)setTop:(CGFloat)top andHeight:(CGFloat)height;
- (void)setTop:(CGFloat)top andWidth:(CGFloat)width;
- (void)setLeft:(CGFloat)left andWidth:(CGFloat)width;

// 生成图片
- (UIImage *)capturedImage;
- (UIImage *)capturedImageWithRect:(CGRect)rect;
// 截屏，对shapelayer 有效
- (UIImage *)hierarchyImage;
- (UIImage *)hierarchyImageWithRect:(CGRect)rect;

// po [self recursiveDescription]
- (void)recursiveSubviews;

// remove subviews
- (void)removeAllSubviews;

// 查找该view下的第一响应者
- (UIView *)findFirstResponderView;

// 查找该view隶属于的viewController
- (UIViewController *)findResponderViewController;

// 增加点击事件
- (void)addTarget:(id)target singleTapAction:(SEL)action;
- (void)addTarget:(id)target longPressAction:(SEL)action;

- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColor:(UIColor *)borderColor;
- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColorStr:(NSString *)borderColorStr;
- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColor:(UIColor *)borderColor cornerRadius:(CGFloat)cornerRadius;
- (void)setLayerBorderWidth:(CGFloat)borderWidth borderColorStr:(NSString *)borderColorStr cornerRadius:(CGFloat)cornerRadius;

- (void)setBackgroundColorString:(NSString *)colorString;

+ (UIView *)clearColorView:(CGRect)frame;

- (void)addBottomLine;
- (void)addBottomLineWithColorStr:(NSString *)colorStr;

- (void)addTopLine;
- (void)addTopLineWithColorStr:(NSString *)colorStr;

- (UIViewController *)ownerViewController;

/**
 *  高斯模糊（毛玻璃效果）
 *
 *  @param view 传入需要进行高斯模糊的视图
 *
 *  @return 返回经过处理后的视图
 */
- (UIView *)gaussView:(UIView *)view;

@end


@interface UIView (Animate)

// 从指定位置扩张
- (void)expandAnimated:(CGRect)rect;

@end


@interface UIView (Hook)

// Allows to change frame/bounds without triggering `layoutSubviews` on the parent.
// Not needed for changes that are performed within `layoutSubviews`.
- (void)performWithoutTriggeringSetNeedsLayout:(dispatch_block_t)block;

@end



