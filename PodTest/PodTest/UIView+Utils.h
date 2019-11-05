//
//  UIView+Utils.h
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (Utils)

// frame element
@property (nonatomic) CGFloat top;
@property (nonatomic) CGFloat left;
@property (nonatomic) CGFloat bottom;
@property (nonatomic) CGFloat right;

@property (nonatomic) CGFloat width;
@property (nonatomic) CGFloat height;

@property (nonatomic) CGSize  size;
@property (nonatomic) CGPoint origin;

@property (nonatomic) CGFloat centerX;
@property (nonatomic) CGFloat centerY;

// 生成图片
- (UIImage *)capturedImage;

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



