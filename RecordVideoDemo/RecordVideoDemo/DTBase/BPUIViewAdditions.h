//
//  BPUIViewAdditions.h
//  VGEUtil
//
//  Created by Hunter Huang on 11/23/11.
//  Copyright (c) 2011 vge design. All rights reserved.
//

#import <UIKit/UIKit.h>
//#import "BPUIResponderAdditions.h"

NS_ASSUME_NONNULL_BEGIN

@interface UIView(BPAdditions)

@property (nonatomic) CGFloat left;
@property (nonatomic) CGFloat top;
@property (nonatomic) CGFloat right;
@property (nonatomic) CGFloat bottom;
@property (nonatomic) CGFloat width;
@property (nonatomic) CGFloat height;
@property (nonatomic) CGFloat centerX;
@property (nonatomic) CGFloat centerY;

@property (nonatomic) CGPoint origin;
@property (nonatomic) CGSize size;

@property (nonatomic) CGFloat cornerRadius;

@property (nonatomic, readonly, nullable) UIViewController *controller;
@property (nonatomic, readonly, nullable) UINavigationController *navigationController;

+ (void)drawGradientInRect:(CGRect)rect withColors:(nonnull NSArray<UIColor *>*)colors;

//水平居中
- (void)xCenterInView:(nonnull UIView *)view;
//垂直居中
- (void)yCentetInView:(UIView *)view;


- (void)paddingRight:(CGFloat)padding toSuper:(nullable UIView *)to;
- (void)paddingBottom:(CGFloat)padding toSuper:(nullable UIView *)to;
- (void)paddingLeft:(CGFloat)padding;
- (void)paddingTop:(CGFloat)padding;



/**
 将view的内容保存成image，即对view进行截屏
 */
- (UIImage *)captureView;

- (nullable UIView *)nextTextResponder;
- (UIView *)firstTextResponder;

- (void)bp_tapped:(void (^)(void))block;
- (void)bp_whenTouches:(NSUInteger)numberOfTouches tapped:(NSUInteger)numberOfTaps handler:(void (^)(void))block;

@end

NS_ASSUME_NONNULL_END

