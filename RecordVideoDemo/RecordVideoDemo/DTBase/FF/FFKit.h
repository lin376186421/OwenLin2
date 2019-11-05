//
//  FFKit.h
//  FFStory
//
//  Created by PageZhang on 14/11/12.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, FFScreenResolution) {
    FFScreenResolution320x480,
    FFScreenResolution320x568,
    FFScreenResolution375x667,
    FFScreenResolution414x736,
    FFScreenResolutionUnknow
};
extern FFScreenResolution FFScreenResolutionType();

extern BOOL FF_iPad();
extern BOOL FF_iPhone();

// majorVersion
extern NSInteger FFMajorVersion();
extern BOOL FFVersionGreaterThanIOS7();
extern BOOL FFVersionGreaterThanIOS8();
extern BOOL FFVersionGreaterThanIOS9();


// UI
extern NSString *UIBundlePath(NSString *bundle_name);
extern UIImage *UIBundleImage(NSString *bundle_name, NSString *image_name);
extern id UIStoryboardController(NSString *sb_name, NSString *vc_name);


// device relation
extern NSString *FFDeviceModel();
extern NSString *FFDeviceVersion();
extern NSString *FFDeviceIPAddress();
extern NSString *FFDeviceUDIDString();


// Return a specific string
extern NSString *FFUUIDString(void);
extern NSString *FFMD5String(id input);
extern NSString *FFSHA1String(NSString *input);
extern NSString *FFURLEncodeString(NSString *input);


// Same as [[NSDate date] timeIntervalSince1970], just a lot faster.
extern NSTimeInterval FFTimeIntervalWithSecondsSince1970(void);
extern int64_t FFTimeIntervalWithMilliSecondsSince1970(void);


// Time tracking. Return time in nanoseconds. Use result/1E9 to print seconds.
extern double FFPerformAndTrackTime(dispatch_block_t block, BOOL trackTime);
// Execute a block that could release our object. Protects against [NSObject cancelPreviousPerformRequestsWithTarget:] woes which does a release instead of an autorelease.
extern void FFRetainAndPerformBlock(id self, dispatch_block_t block);


// If we add a different window, we need to make the current window active again, else text input might break.
extern void FFRestoreKeyWindow(void);
// On iOS8, Once you changed keywindow‘s rootViewController or switch into background,
// Touches on the keyboard area, root view's hitTest method will be triggered
extern void FFRestoreKeyboardWindow(void);


// Try to retuen a avaliable vc
extern UIViewController *FFWindowTopViewController(void);
extern UIViewController *FFWindowRootViewController(void);
// Replace `UITableView` base with `UIView` as base with child `UITableView`.
extern void FFReplaceBaseTableViewWithRegularView(UITableViewController *vc);


// Will call a block asynchronously.
extern void ff_dispatch_async(dispatch_block_t block);
// Will directly call a block if on main thread, else dispatch it asynchronously.
extern void ff_dispatch_main_async(dispatch_block_t block);

// Will call a block synchronously.
extern void ff_dispatch_sync(dispatch_block_t block);
// Will directly call a block if on main thread, else dispatch it synchronously.
extern void ff_dispatch_main_sync(dispatch_block_t block);

extern NSString * readTxtFromPath(NSString *path);

extern void writeTxtToPath(NSString *text, NSString *path);

extern BOOL CGFloatEqualWithFloat(CGFloat f1, CGFloat f2);

/* 比较 f1 与 f2
 * 0 为 等于， 1 为 大于   -1 为小于
 */
extern int CGFloatCompareWithFloat(CGFloat f1, CGFloat f2);

extern CGFloat validValue(CGFloat f, CGFloat min, CGFloat max);

// Helper to create dispatch queues with a default name.
#define ff_dispatch_queue_create() dispatch_queue_create([NSString stringWithFormat:@"com.ffkit.<%@: %p>", self.class, self].UTF8String, NULL)



/// @name Atomic getting/setting of properties.

// See http://www.opensource.apple.com/source/objc4/objc4-371.2/runtime/Accessors.subproj/objc-accessors.h
extern void objc_setProperty(id self, SEL _cmd, ptrdiff_t offset, id newValue, BOOL atomic, BOOL shouldCopy);
extern id objc_getProperty(id self, SEL _cmd, ptrdiff_t offset, BOOL atomic);
extern void objc_copyStruct(void *dest, const void *src, ptrdiff_t size, BOOL atomic, BOOL hasStrong);
#define FFAtomicRetainedSetToFrom(dest, source) objc_setProperty(self, _cmd, (ptrdiff_t)(&dest) - (ptrdiff_t)(self), source, YES, NO)
#define FFAtomicCopiedSetToFrom(dest, source) objc_setProperty(self, _cmd, (ptrdiff_t)(&dest) - (ptrdiff_t)(self), source, YES, YES)
#define FFAtomicAutoreleasedGet(source) objc_getProperty(self, _cmd, (ptrdiff_t)(&source) - (ptrdiff_t)(self), YES)
#define FFAtomicStructToFrom(dest, source) objc_copyStruct(&dest, &source, sizeof(__typeof__(source)), YES, NO)



