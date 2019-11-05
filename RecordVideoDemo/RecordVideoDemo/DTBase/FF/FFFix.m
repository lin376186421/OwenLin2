//
//  FFFix.m
//  FFStory
//
//  Created by PageZhang on 16/3/3.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFFix.h"
#import <objc/runtime.h>
#import "BPExecutorService.h"
#import "UIView+animation.h"
#import "BPSDImageManagerAdditions.h"

@implementation NSObject (Fix)

//拦截异常 这些方法 系统会自动抛出异常
- (void)setNilValueForKey:(NSString *)key
{
    NSString *title = [NSString stringWithFormat:@"%@ - %s key = %@",NSStringFromClass(self.class),__func__,key];
    [SCUMengManager event:APP_CRASH_EVENT title:title];
    NSLog(@"%@", title);
}

- (void)setValue:(id)value forUndefinedKey:(NSString *)key
{
    NSString *title = [NSString stringWithFormat:@"%@ - %s key = %@ value = %@",NSStringFromClass(self.class),__func__,key,value];
    [SCUMengManager event:APP_CRASH_EVENT title:title];
    NSLog(@"%@", title);
}

- (nullable id)valueForUndefinedKey:(NSString *)key
{
    NSString *title = [NSString stringWithFormat:@"%@ - %s key = %@",NSStringFromClass(self.class),__func__,key];
    [SCUMengManager event:APP_CRASH_EVENT title:title];
    NSLog(@"%@", title);
    return nil;
}

@end

// 自爆菊花会Crash
@implementation UIView (Fix)
+ (void)load {
    Method originalInitMethod = class_getInstanceMethod(self, @selector(addSubview:));
    Method modifiedInitMethod = class_getInstanceMethod(self, @selector(safeAddSubview:));
    method_exchangeImplementations(originalInitMethod, modifiedInitMethod);
}
- (void)safeAddSubview:(UIView *)view {
    if (self == view) return;
    [self safeAddSubview:view];
}
@end

// iOS7之前UILabel的默认背景色为白色，为了各版本保持一致，下面代码将默认背景色调整为透明
@implementation UILabel (Fix)
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_7_0
+ (void)load {
    if (!FFVersionGreaterThanIOS7()) {
        Method originalDecoderMethod = class_getInstanceMethod(self, @selector(initWithCoder:));
        Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(initWithCustomCoder:));
        method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
        
        Method originalInitMethod = class_getInstanceMethod(self, @selector(initWithFrame:));
        Method modifiedInitMethod = class_getInstanceMethod(self, @selector(initWithCustomFrame:));
        method_exchangeImplementations(originalInitMethod, modifiedInitMethod);
    }
}
- (instancetype)initWithCustomCoder:(NSCoder *)aDecoder {
    self = [self initWithCustomCoder:aDecoder];
    [self hotfixSetup];
    return self;
}
- (instancetype)initWithCustomFrame:(CGRect)frame {
    self = [self initWithCustomFrame:frame];
    [self hotfixSetup];
    return self;
}
- (void)hotfixSetup {
    if (self.backgroundColor == nil) {
        self.backgroundColor = [UIColor clearColor];
    }
}
#endif
@end

@implementation UITableView (Fix)
+ (void)load {
#ifdef __IPHONE_11_0
    if (AvailableiOS(11.0)) {
        Method originalDecoderMethod = class_getInstanceMethod(self, @selector(initWithCoder:));
        Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(initWithCustomCoder:));
        method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
        
        Method originalInitMethod = class_getInstanceMethod(self, @selector(initWithFrame:));
        Method modifiedInitMethod = class_getInstanceMethod(self, @selector(initWithCustomFrame:));
        method_exchangeImplementations(originalInitMethod, modifiedInitMethod);
        
        Method originalInitStyleMethod = class_getInstanceMethod(self, @selector(initWithFrame:style:));
        Method modifiedInitStyleMethod = class_getInstanceMethod(self, @selector(initWithCustomFrame:style:));
        method_exchangeImplementations(originalInitStyleMethod, modifiedInitStyleMethod);
    }
#endif
    
}
- (instancetype)initWithCustomCoder:(NSCoder *)aDecoder {
    self = [self initWithCustomCoder:aDecoder];
    [self hotfixSetup];
    return self;
}

- (instancetype)initWithCustomFrame:(CGRect)frame {
    self = [self initWithCustomFrame:frame];
    [self hotfixSetup];
    return self;
}

- (instancetype)initWithCustomFrame:(CGRect)frame style:(UITableViewStyle)style
{
    self = [self initWithCustomFrame:frame style:style];
    [self hotfixSetup];
    return self;
}

- (void)hotfixSetup {
    
    if ([DTOnlineConfig onlineBoolValue:@"tableview_no_hotfix"]) {
        return;
    }
    self.estimatedRowHeight = 0;
    self.estimatedSectionHeaderHeight = 0;
    self.estimatedSectionFooterHeight = 0;
    self.showsVerticalScrollIndicator = self.showsHorizontalScrollIndicator = NO;
#ifdef __IPHONE_11_0
    if (AvailableiOS(11.0)) {
        [self setContentInsetAdjustmentBehavior:UIScrollViewContentInsetAdjustmentNever];
    }
#endif
}
@end


@interface UIImageView ()

- (void)setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder failedImage:(UIImage *)failedImage downloadTagStr:(NSString *)tagStr;

@end

@implementation UIImageView (Fix)

+ (void)load
{
    SEL methodName = @selector(setImageWithURL:placeholderImage:failedImage:downloadTagStr:);
    if ([self instancesRespondToSelector:methodName]) {
        Method originalMethod = class_getInstanceMethod(self, methodName);
        Method modifiedMethod = class_getInstanceMethod(self, @selector(ff_setImageWithURL:placeholderImage:failedImage:downloadTagStr:));
        method_exchangeImplementations(originalMethod, modifiedMethod);
    }
    
    Method originalSdMethod = class_getInstanceMethod(self, @selector(sd_setImageWithURL:placeholderImage:options:progress:completed:));
    Method modifiedSdMethod = class_getInstanceMethod(self, @selector(ff_setImageWithURL:placeholderImage:options:progress:completed:));
    method_exchangeImplementations(originalSdMethod, modifiedSdMethod);
}

- (BOOL)ff_haveImageCacheWithURL:(NSURL *)url
{
    __weak UIImageView *weakSelf = self;
    NSString *cacheUrl = nil;
    if (weakSelf.layer.cornerRadius>0) {
        cacheUrl = [[url absoluteString] stringByAppendingFormat:@"_%.0fx%.0f_o%.0f", weakSelf.width, weakSelf.height, weakSelf.cornerRadius];
        if ([[SDWebImageManager sharedManager] hasCacheForURL:[NSURL URLWithString:cacheUrl]]) {
            return YES;
        }
    } else if (weakSelf.layer.cornerRadius==0) {
        if ([[SDWebImageManager sharedManager] hasCacheForURL:url]) {
            return YES;
        }
    }
    return NO;
}

- (void)ff_setImageWithURL:(NSURL *)url placeholderImage:(UIImage *)placeholder failedImage:(UIImage *)failedImage downloadTagStr:(NSString *)tagStr;
{
    if ([self ff_haveImageCacheWithURL:url]) {
        [self fixImageMode];
    }
    [self ff_setImageWithURL:url placeholderImage:placeholder failedImage:failedImage downloadTagStr:tagStr];
}

- (void)ff_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder
                   options:(SDWebImageOptions)options
                  progress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                 completed:(nullable SDExternalCompletionBlock)completedBlock
{
//    [self fixImageMode];
    [self ff_setImageWithURL:url placeholderImage:placeholder options:options progress:progressBlock completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
        if (error == nil) {
            [self fixImageMode];
        }
        if (completedBlock) {
            completedBlock(image, error, cacheType, imageURL);
        }
    }];
}

- (void)fixImageMode
{
    if (self.contentMode != UIViewContentModeScaleAspectFill && self.contentMode != UIViewContentModeScaleAspectFit) {
        if (self.contentMode == UIViewContentModeCenter) {
            self.contentMode = UIViewContentModeScaleAspectFit;
        } else {
            self.contentMode = UIViewContentModeScaleAspectFill;
        }
    }
}

@end


// iOS7下通过代码创建的cell，如果使用autolayout，运行时界面会出错
@implementation UITableViewCell (Fix)
//#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_8_0
+ (void)load {
    Method originalDecoderMethod = class_getInstanceMethod(self, @selector(initWithStyle:reuseIdentifier:));
    Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(initWithCustomStyle:reuseIdentifier:));
    method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
}
- (instancetype)initWithCustomStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [self initWithCustomStyle:style reuseIdentifier:reuseIdentifier];
    [self hotfixSetup];
    return self;
}
- (void)hotfixSetup {
    if (FFMajorVersion() == 7) {
        UIView *contentView = self.contentView;
        contentView.translatesAutoresizingMaskIntoConstraints = NO;
        [contentView.superview addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[contentView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(contentView)]];
        [contentView.superview addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[contentView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(contentView)]];
        [self addConstraint:[NSLayoutConstraint constraintWithItem:contentView attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeWidth multiplier:1 constant:0]];
        [self addConstraint:[NSLayoutConstraint constraintWithItem:contentView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeHeight multiplier:1 constant:0]];
    }
    
    UIView *selBgView = [[UIView alloc] initWithFrame:self.bounds];
    selBgView.backgroundColor = [UIColor colorWithHexString:@"eeeeee" alpha:1.0f];
    self.selectedBackgroundView = selBgView;
}
//#endif
@end


// iOS7下，如果使用autolayout，运行时界面会出错
@implementation UICollectionViewCell (Fix)
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_8_0
+ (void)load {
    if (FFMajorVersion() == 7) {
        Method originalDecoderMethod = class_getInstanceMethod(self, @selector(initWithCoder:));
        Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(initWithCustomCoder:));
        method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
        
        Method originalInitMethod = class_getInstanceMethod(self, @selector(initWithFrame:));
        Method modifiedInitMethod = class_getInstanceMethod(self, @selector(initWithCustomFrame:));
        method_exchangeImplementations(originalInitMethod, modifiedInitMethod);
    }
}
- (instancetype)initWithCustomCoder:(NSCoder *)aDecoder {
    self = [self initWithCustomCoder:aDecoder];
    ff_dispatch_main_async(^{
        [self hotfixSetup];
    });
    return self;
}
- (instancetype)initWithCustomFrame:(CGRect)frame {
    self = [self initWithCustomFrame:frame];
    [self hotfixSetup];
    return self;
}
- (void)hotfixSetup {
    self.contentView.frame = self.bounds;
    self.contentView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
}
#endif
@end

typedef NS_ENUM(NSInteger, DTFixNavigationBarViewPosition) {
    DTFixNavigationBarViewPositionLeft,
    DTFixNavigationBarViewPositionRight
};

//@interface DTFixNavigationBarView : UIView
//@property (nonatomic, assign) DTFixNavigationBarViewPosition position;
//@property (nonatomic, assign) BOOL applied;
//@property (nonatomic, assign) BOOL isMoveToWindow;
//
//@property (nonatomic, weak) UIView *navigationFixView;
//
//@property (nonatomic, strong) NSLayoutConstraint *leftFixConstraint;
//@property (nonatomic, strong) NSLayoutConstraint *rightFixConstraint;
//
//@property (nonatomic, assign) CGFloat fixGap;
//
//@end

//@implementation DTFixNavigationBarView
//
//- (instancetype)initWithFrame:(CGRect)frame
//{
//    self = [super initWithFrame:frame];
//    if (self) {
//        _fixGap = 10.f;
//    }
//    return self;
//}
//
//- (void)dealloc
//{
//    _isMoveToWindow = NO;
//    [self setNeedsLayout];
//}
//
//- (void)willMoveToWindow:(UIWindow *)newWindow
//{
//    [super willMoveToWindow:newWindow];
//    if (!newWindow) {
//        _isMoveToWindow = NO;
//    } else {
//        _isMoveToWindow = YES;
//    }
//    [self setNeedsLayout];
//}
//
//- (void)layoutSubviews {
//    [super layoutSubviews];
//    if ([UIDevice bigSystemVersion]< 11) return;
//
//    if (!_isMoveToWindow && self.applied) {
//        if (_navigationFixView) {
//            if (self.position == DTFixNavigationBarViewPositionLeft) {
//                if (_leftFixConstraint) {
//                    [_navigationFixView removeConstraint:_leftFixConstraint];
//                }
//            } else if (self.position == DTFixNavigationBarViewPositionRight) {
//                if (_rightFixConstraint) {
//                    [_navigationFixView removeConstraint:_rightFixConstraint];
//                }
//            }
//        }
//        self.applied = NO;
//    } else {
//        UIView *view = self;
//        while (![view isKindOfClass:UINavigationBar.class] && view.superview) {
//            view = [view superview];
//            if ([view isKindOfClass:UIStackView.class] && view.superview) {
//                _navigationFixView = view.superview;
//                if (self.position == DTFixNavigationBarViewPositionLeft) {
//                    _leftFixConstraint = [NSLayoutConstraint constraintWithItem:view
//                                                                      attribute:NSLayoutAttributeLeading
//                                                                      relatedBy:NSLayoutRelationEqual
//                                                                         toItem:view.superview
//                                                                      attribute:NSLayoutAttributeLeading
//                                                                     multiplier:1.0
//                                                                       constant:_fixGap];
//                    [view.superview addConstraint:_leftFixConstraint];
//                    self.applied = YES;
//                } else if (self.position == DTFixNavigationBarViewPositionRight) {
//                    _rightFixConstraint = [NSLayoutConstraint constraintWithItem:view
//                                                                       attribute:NSLayoutAttributeTrailing
//                                                                       relatedBy:NSLayoutRelationEqual
//                                                                          toItem:view.superview
//                                                                       attribute:NSLayoutAttributeTrailing
//                                                                      multiplier:1.0
//                                                                        constant:-_fixGap];
//                    [view.superview addConstraint:_rightFixConstraint];
//                    self.applied = YES;
//                }
//                break;
//            }
//        }
//    }
//}
//
//@end
//
//@implementation UINavigationItem (Fix)
//
//#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_6_1
//
//+ (void)load {
//    Method originalDecoderMethod = class_getInstanceMethod(self, @selector(setLeftBarButtonItem:));
//    Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(setDTLeftBarButtonItem:));
//    method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
//
//    Method originalInitMethod = class_getInstanceMethod(self, @selector(setRightBarButtonItem:));
//    Method modifiedInitMethod = class_getInstanceMethod(self, @selector(setDTRightBarButtonItem:));
//    method_exchangeImplementations(originalInitMethod, modifiedInitMethod);
//
////    for (NSString *method in @[@"NEWSetLeftBarButtonItems:", @"NEWSetRightBarButtonItems:"]) {
////        Method originalDecoderMethod1 = class_getInstanceMethod(self, NSSelectorFromString(method));
////        Method modifiedDecoderMethod1 = class_getInstanceMethod(self, @selector(setDTFixBPItem:));
////        method_exchangeImplementations(originalDecoderMethod1, modifiedDecoderMethod1);
////    }
//}
//
////- (void)setDTFixBPItem:(UIBarButtonItem *)item
////{
////
////}
//
//- (void)setDTLeftBarButtonItem:(UIBarButtonItem *)leftBarButtonItem
//{
//    [self setLeftBarButtonItems:nil animated:NO];
//    if ([[[UIDevice currentDevice] systemVersion] floatValue]>=7.0&&leftBarButtonItem&&leftBarButtonItem.customView) {
//        UIView *customView = leftBarButtonItem.customView;
//        if (AvailableiOS(11.0)) {//适配ios 11系统
//            DTFixNavigationBarView *barView = [[DTFixNavigationBarView alloc] initWithFrame:customView.bounds];
//            if ([customView isKindOfClass:NSClassFromString(@"CLJSBridgeWebBackView")]) {
//                barView.fixGap = 1.f;// jsbridge 特殊处理下
//            }
//            [barView addSubview:customView];
//            customView.center = barView.center;
//            [barView setPosition:DTFixNavigationBarViewPositionLeft];//说明这个view需要调整的是左边
//            [self setLeftBarButtonItems:@[[[UIBarButtonItem alloc] initWithCustomView:barView]]];
//
//        } else {
//            UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
//            CGFloat gap = -8.f;
//            if ([customView isKindOfClass:NSClassFromString(@"CLJSBridgeWebBackView")]) {
//                gap = -15.5f;// jsbridge 特殊处理下
//            }
//            negativeSeperator.width = gap;
//            [self setLeftBarButtonItems:@[negativeSeperator, leftBarButtonItem] animated:NO];
//        }
//    } else {
//        [self setLeftBarButtonItem:leftBarButtonItem animated:NO];
//    }
//}
//
//- (void)setDTRightBarButtonItem:(UIBarButtonItem *)rightBarButtonItem
//{
//    [self setRightBarButtonItems:nil animated:NO];
//    if ([[[UIDevice currentDevice] systemVersion] floatValue]>=7.0&&rightBarButtonItem&&rightBarButtonItem.customView) {
//        UIView *customView = rightBarButtonItem.customView;
//        if (AvailableiOS(11.0)) {
//            DTFixNavigationBarView *barView = [[DTFixNavigationBarView alloc] initWithFrame:customView.bounds];
//            [barView addSubview:customView];
//            customView.center = barView.center;
//            [barView setPosition:DTFixNavigationBarViewPositionRight];//说明这个view需要调整的是左边
//            [self setRightBarButtonItems:@[[[UIBarButtonItem alloc] initWithCustomView:barView]]];
//        } else {
//            UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
//            negativeSeperator.width = -8;
//            [self setRightBarButtonItems:@[negativeSeperator, rightBarButtonItem] animated:NO];
//        }
//    } else {
//        [self setRightBarButtonItem:rightBarButtonItem animated:NO];
//    }
//}
//
//#endif
//
//@end

// 微信sdk 1.6.2 重写了Decoder方法
@implementation NSDictionary (Fix)

+ (void)load {
    for (NSString *method in @[@"arrayForKey:", @"dictionaryForKey:", @"doubleForKey:", @"floatForKey:", @"integerForKey:", @"stringForKey:"]) {
        Method originalDecoderMethod = class_getInstanceMethod(self, NSSelectorFromString(method));
        Method modifiedDecoderMethod = class_getInstanceMethod(self, NSSelectorFromString([@"ff_" stringByAppendingString:method]));
        method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
    }
}

- (NSArray *)ff_arrayForKey:(id)aKey {
    id object = self[aKey];
    if ([object isKindOfClass:NSArray.class]) {
        return object;
    } else {
        return nil;
    }
}

- (NSDictionary *)ff_dictionaryForKey:(id)aKey {
    id object = self[aKey];
    if ([object isKindOfClass:NSDictionary.class]) {
        return object;
    } else {
        return nil;
    }
}

- (NSURL *)ff_urlForKey:(id)aKey {
    id object = self[aKey];
    if ([object isKindOfClass:NSURL.class]) {
        return object;
    }
    
    NSString *string = [self stringForKey:aKey];
    if (string) {
        return [NSURL URLWithString:string];
    } else {
        return nil;
    }
}

- (NSString *)ff_stringForKey:(id)aKey {
    id object = self[aKey];
    if ([object isKindOfClass:[NSString class]]) {
        return object;
    } else if ([object respondsToSelector:@selector(stringValue)]) {
        return [object stringValue];
    } else {
        return nil;
    }
}

- (NSNumber *)ff_numberForKey:(id)aKey {
    id object = self[aKey];
    if ([object isKindOfClass:NSNumber.class]) {
        return object;
    } else if ([object isKindOfClass:NSString.class]) {
        static NSNumberFormatter *formatter = nil;
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            formatter = [[NSNumberFormatter alloc] init];
            [formatter setNumberStyle:NSNumberFormatterDecimalStyle];
        });
        return [formatter numberFromString:object];
    } else {
        return nil;
    }
}

- (NSInteger)ff_integerForKey:(id)aKey {
    return [[self ff_numberForKey:aKey] integerValue];
}

- (float)ff_floatForKey:(id)aKey {
    return [[self ff_numberForKey:aKey] floatValue];
}

- (double)ff_doubleForKey:(id)aKey {
    return [[self ff_numberForKey:aKey] doubleValue];
}

- (BOOL)ff_boolForKey:(id)aKey {
    NSString *stringValue = [[self stringForKey:aKey] lowercaseString];
    if (stringValue && [stringValue respondsToSelector:@selector(boolValue)]) {
        return [stringValue boolValue];
    }
    return NO;
}

@end

@implementation NSArray (Fix)

+ (void)load
{
    Method originalDecoderMethod = class_getInstanceMethod(self, @selector(objectAtIndex:));
    Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(ff_objectAtIndex:));
    method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
}

- (id)ff_objectAtIndex:(NSUInteger)index
{
    if (index < self.count) {
        return [self ff_objectAtIndex:index];
    }
    return nil;
}

@end

@implementation NSURL (Fix)

+ (void)load
{
    Method originalDecoderMethod = class_getInstanceMethod(self, @selector(initFileURLWithPath:));
    Method modifiedDecoderMethod = class_getInstanceMethod(self, @selector(ff_initFileURLWithPath:));
    method_exchangeImplementations(originalDecoderMethod, modifiedDecoderMethod);
}

- (instancetype)ff_initFileURLWithPath:(NSString *)path
{
    if (path.length) {
        return [self ff_initFileURLWithPath:path];
    }
    return nil;
}


@end


