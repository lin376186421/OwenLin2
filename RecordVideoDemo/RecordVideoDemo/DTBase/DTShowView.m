//
//  DTShowView.m
//  DrivingTest
//
//  Created by cheng on 16/10/29.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import "DTShowView.h"
//#import "CWKeyboardManager.h"


@implementation UIView (DTShowView)

- (void)_dt_show_dismiss {
    
    UIView *superView = self.superview;
    while (superView) {
        if ([superView isKindOfClass:[DTShowView class]]) {
            DTShowView *showView = (id)superView;
            [showView dismissView];
            return;
        }
        superView = superView.superview;
    }
    
    [self removeFromSuperview];
}


@end

@interface DTShowView () {
    DTShowStatus _status;
    UITapGestureRecognizer *_tap;
    CGFloat _keyBoardHeight;
}

@end

@implementation DTShowView

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)removeFromSuperview
{
//    [DTQueueAlertManager scheduledPopViewBlock];
    [super removeFromSuperview];
}

- (instancetype)init
{
    return [self initWithFrame:[UIScreen mainScreen].bounds];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        
        _bgView = [[UIView alloc] initWithFrame:self.bounds];
        _bgView.backgroundColor = [UIColor colorWithWhite:0.f alpha:0.5];
        _bgView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
        [self addSubview:_bgView];
        
        _contentView = [[UIView alloc] initWithFrame:CGRectMake(self.width/2-200/2, self.height/2-200/2, 200, 200)];
        _contentView.backgroundColor = [UIColor whiteColor];
        _contentView.cornerRadius = 5;
        _contentView.layer.masksToBounds = YES;
        _contentView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin|UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleTopMargin|UIViewAutoresizingFlexibleBottomMargin;
        [self addSubview:_contentView];
        
//        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(dismissView) name:APP_EVENT_ALERT_VIEW_DISMISS object:nil];
    }
    return self;
}

- (void)scaleToScreenSize
{
    [self scaleToScreenSizeBaseWidth:375.f];
}

- (void)scaleToScreenSizeBaseWidth:(CGFloat)baseWidth
{
    float scale =  SCREEN_WIDTH/baseWidth;
    if (SCREEN_WIDTH > 414) {
        scale = 414/baseWidth;
    }
    if (scale < 1.f) {
        scale = 1.f;
    }
    self.transform = CGAffineTransformMakeScale(scale, scale);
}


- (void)setBgAlpha:(CGFloat)bgAlpha
{
    _bgView.backgroundColor = [UIColor colorWithWhite:0.f alpha:bgAlpha];
}

- (void)setTapDismiss:(BOOL)tapDismiss
{
    _tapDismiss = tapDismiss;
    if (_tapDismiss && _tap == nil) {
        _tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissView)];
        [_bgView addGestureRecognizer:_tap];
    }
    _tap.enabled = tapDismiss;
}

+ (DTShowView *)showInView:(UIView*)superView withView:(UIView *)subView
{
    return [self showInView:superView withView:subView config:nil];
}

+ (DTShowView *)showInView:(UIView*)superView withView:(UIView *)subView config:(DTShowViewConfigCallBack)call
{
    DTShowView *view = [[DTShowView alloc] init];
    view.contentView.frame = CGRectMake(view.width/2-subView.width/2, view.height/2-subView.height/2, subView.width, subView.height);
    [view.contentView addSubview:subView];
    subView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
    [view showInView:superView config:^{
        if (call) {
            call(view);
        }
    }];
    return view;
}

+ (DTShowView *)showInView:(UIView*)superView withXibName:(NSString *)xibName
{
    return [self showInView:superView withXibName:xibName config:nil];
}

+ (DTShowView *)showInView:(UIView*)superView withXibName:(NSString *)xibName config:(DTShowViewConfigCallBack)call
{
    UIView *xibView = [[NSBundle mainBundle] loadNibNamed:xibName owner:self options:nil].firstObject;
    if (xibView) {
        return [self showInView:superView withView:xibView config:call];
    }
    return nil;
}

- (void)showInView:(UIView *)view
{
    [self showInView:view config:nil completion:nil];
}

- (void)showInView:(UIView *)view config:(void (^)(void))config
{
    [self showInView:view config:config completion:nil];
}

- (void)showInView:(UIView *)view completion:(void (^)(void))completion
{
    [self showInView:view config:nil completion:completion];
}


- (void)showInView:(UIView *)view config:(void (^)(void))config completion:(void (^)(void))completion
{
    if (config) {
        config();
    }
    
    if (view == nil) {
        view = [UIApplication sharedApplication].keyWindow;
    }
    
    if (!_disableAutoBounds) {
        self.frame = view.bounds;
    }
    [view addSubview:self];
    [self showAnimationWithComplete:completion];
    
    if (_isKeyBoardAdjust) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    }
    
}

#pragma mark - notification observer

- (void)keyboardWillShow:(NSNotification *)notification
{
    CGRect keyboardRect = [[[notification userInfo] objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    _keyBoardHeight = keyboardRect.size.height;
    float duration = [[notification.userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] floatValue];

    
    [UIView animateWithDuration:duration animations:^{
        self.contentView.top = ([UIScreen mainScreen].bounds.size.height - _keyBoardHeight - _contentView.height)/2;
        //        _bgView.alpha = 1.0f;
    }];
}

- (void)keyboardWillHide:(NSNotification *)notification
{
    _keyBoardHeight = 0;
    float duration = [[notification.userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] floatValue];

    [UIView animateWithDuration:duration animations:^{
        self.contentView.top = ([UIScreen mainScreen].bounds.size.height - _keyBoardHeight - _contentView.height)/2;
    }];
}

- (void)showAnimationWithComplete:(void (^)(void))completion
{
    [self updateStatus:DTShowStatusAnimationSet];
    [UIView animateWithDuration:0.25 animations:^{
        [self updateStatus:DTShowStatusAnimationing];
    } completion:^(BOOL finished) {
        if (completion) {
            completion();
        }
    }];
}

- (void)updateStatus:(DTShowStatus)status
{
    _status = status;
    
    _bgView.alpha = (status == DTShowStatusAnimationing ? 1.f : 0.f);

    switch (_animationType) {
        case DTShowTypeAnimationFade:
        {
            _contentView.alpha = (status == DTShowStatusAnimationing ? 1.f : 0.f);
        }
            break;
        case DTShowTypeAnimationFallDown:
        case DTShowTypeAnimationFallRotate:
        {
            if (status == DTShowStatusAnimationSet) {
                _contentView.center = CGPointMake(self.width/2, -_contentView.height/2);
                if (_animationType == DTShowTypeAnimationFallRotate) {
                    _contentView.transform = CGAffineTransformMakeRotation(-M_PI_4);
                    _contentView.alpha = (status == DTShowStatusAnimationing ? 1.f : 0.f);
                }
            } else if (status == DTShowStatusAnimationOut) {
                _contentView.center = CGPointMake(self.width/2, self.height + _contentView.height/2);
                if (_animationType == DTShowTypeAnimationFallRotate) {
                    _contentView.transform = CGAffineTransformMakeRotation(M_PI_4);
                }
            } else {
                _contentView.center = CGPointMake(self.width/2, self.height/2);
                if (_animationType == DTShowTypeAnimationFallRotate) {
                    _contentView.transform = CGAffineTransformIdentity;
                    _contentView.alpha = (status == DTShowStatusAnimationing ? 1.f : 0.f);
                }
            }
        }
            break;
        case DTShowTypeAnimationPushDown:
        {
            _contentView.top = (status == DTShowStatusAnimationing ? self.height/2-_contentView.height/2 : self.height);
        }
            break;
        case DTShowTypeAnimationPushDownBottom:
        {
            _contentView.top = (status == DTShowStatusAnimationing ? self.height-_contentView.height : self.height);
        }
            break;
        case DTShowTypeAnimationZoomin:
        case DTShowTypeAnimationZoomOut:
        {
            if (status == DTShowStatusAnimationing) {
                _contentView.alpha = 1.f;
                _contentView.transform = CGAffineTransformIdentity;
            } else {
                _contentView.alpha = 0.f;
                CGFloat scale = (_animationType ==  DTShowTypeAnimationZoomin ? 1.3 : 0.7);
                _contentView.transform = CGAffineTransformMakeScale(scale, scale);
            }
        }
            break;
        default:
            break;
    }
}

- (void)dismissView
{
    [self dismissViewCompletion:nil];
}

- (void)dismissViewCompletion:(void (^)(void))completion
{
    if (_dismissBlock) {
        _dismissBlock();
    }
    [UIView animateWithDuration:0.25 animations:^{
        [self updateStatus:DTShowStatusAnimationOut];
    } completion:^(BOOL finished) {
        [self removeFromSuperview];
        if (completion) {
            completion();
        }
    }];
}

@end
