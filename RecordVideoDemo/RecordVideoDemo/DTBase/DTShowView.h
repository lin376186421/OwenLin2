//
//  DTShowView.h
//  DrivingTest
//
//  Created by cheng on 16/10/29.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum {
    DTShowTypeAnimationFade = 0,//透明度 0 ~ 1
    DTShowTypeAnimationFallDown,//从顶部掉入， 底部掉出
    DTShowTypeAnimationFallRotate,//从顶部倾斜掉入，底部倾斜掉出
    DTShowTypeAnimationPushDown,//从底部进入， 底部掉出
    DTShowTypeAnimationPushDownBottom,//从底部进入， 底部掉出
    DTShowTypeAnimationZoomin,//放大， 进场 由放大到正常，
    DTShowTypeAnimationZoomOut,//缩小，进场 由缩小到正常，
}DTShowType;

typedef enum {
    DTShowStatusAnimationSet = 0,
    DTShowStatusAnimationing,
    DTShowStatusAnimationOut,
    DTShowStatusAnimationFinish,
}DTShowStatus;

@class DTShowView;

typedef void (^DTShowViewConfigCallBack)(DTShowView *showView);

@interface UIView (DTShowView)

- (void)_dt_show_dismiss;

@end

@interface DTShowView : UIView

@property (nonatomic, strong) UIView *bgView;
@property (nonatomic, strong) UIView *contentView;
@property (nonatomic) CGFloat bgAlpha;//default is 0.5
@property (nonatomic) BOOL disableAutoBounds;
@property (nonatomic) BOOL tapDismiss;//default is NO
@property (nonatomic, assign) BOOL isKeyBoardAdjust;//键盘弹起自适应UI

//动画类型
@property (nonatomic) DTShowType animationType;

@property (nonatomic, strong) void (^dismissBlock)(void);
@property (nonatomic, strong) void (^submitBlock)(void);

- (void)updateStatus:(DTShowStatus)status;

- (void)showInView:(UIView *)view;
- (void)showInView:(UIView *)view config:(void (^)(void))config;
- (void)showInView:(UIView *)view completion:(void (^)(void))completion;
- (void)showInView:(UIView *)view config:(void (^)(void))config completion:(void (^)(void))completion;

- (void)dismissView;
- (void)dismissViewCompletion:(void (^)(void))completion;
- (void)showAnimationWithComplete:(void (^)(void))completion;

- (void)scaleToScreenSize;
- (void)scaleToScreenSizeBaseWidth:(CGFloat)baseWidth;

//subView最终会add到contentView上

+ (DTShowView *)showInView:(UIView*)superView withView:(UIView *)subView;
+ (DTShowView *)showInView:(UIView*)superView withView:(UIView *)subView config:(DTShowViewConfigCallBack)call;

+ (DTShowView *)showInView:(UIView*)superView withXibName:(NSString *)xibName;
+ (DTShowView *)showInView:(UIView*)superView withXibName:(NSString *)xibName config:(DTShowViewConfigCallBack)call;

@end
