//
//  FFKit.m
//  FFStory
//
//  Created by PageZhang on 14/11/12.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "FFKit.h"
#import "FFKeyChain.h"
#import "FFLogging.h"

FFScreenResolution FFScreenResolutionType() {
    int height = (int)SCREEN_HEIGHT;
    switch (height) {
        case 480: return FFScreenResolution320x480;
        case 568: return FFScreenResolution320x568;
        case 667: return FFScreenResolution375x667;
        case 736: return FFScreenResolution414x736;
        default:  return FFScreenResolutionUnknow;
    }
}

BOOL FF_iPad() {
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
}
BOOL FF_iPhone() {
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone;
}

NSInteger FFMajorVersion() {
#if __IPHONE_OS_VERSION_MIN_REQUIRED < 80000
    NSArray *array = [[[UIDevice currentDevice] systemVersion] componentsSeparatedByString:@"."];
    return [array.firstObject intValue];
#else
    return [NSProcessInfo processInfo].operatingSystemVersion.majorVersion;
#endif
}
BOOL FFVersionGreaterThanIOS7() {
    return FFMajorVersion() >= 7;
}
BOOL FFVersionGreaterThanIOS8() {
    return FFMajorVersion() >= 8;
}
BOOL FFVersionGreaterThanIOS9() {
    return FFMajorVersion() >= 9;
}


#pragma mark - UI

// GET BUNDLE
NSString *UIBundlePath(NSString *bundle_name) {
    static NSCache *_bundle_cache = nil;
    if (!_bundle_cache) _bundle_cache = [NSCache new];
    NSString *bundle_path = [_bundle_cache objectForKey:bundle_name];
    if (!bundle_path) {
        bundle_path = [NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:bundle_name];
        bundle_path = [bundle_path.stringByDeletingPathExtension stringByAppendingPathExtension:@"bundle"];
        [_bundle_cache setObject:bundle_path forKey:bundle_name];
    }
    return bundle_path;
}

// GET IMAGE
UIImage *UIBundleImage(NSString *bundle_name, NSString *image_name) {
    static NSCache *_img_cache = nil;
    if (!_img_cache) _img_cache = [NSCache new];
    NSString *bundle_path = UIBundlePath(bundle_name);
    NSString *image_path = [bundle_path stringByAppendingPathComponent:image_name];
    UIImage *image = [_img_cache objectForKey:image_path];
    if (!image) {
        image = [UIImage imageWithContentsOfFile:image_path];
        [_img_cache setObject:image ?: NSNull.null forKey:image_path];
        if (!image) FFLog(@"could not load the image \"%@\".", image_path);
    }
    return image;
}

// GET STORYBOARD
id UIStoryboardController(NSString *sb_name, NSString *vc_name) {
    static NSCache *_sb_cache = nil;
    if (!_sb_cache) _sb_cache = [NSCache new];
    UIStoryboard *sb = [_sb_cache objectForKey:sb_name];
    if (!sb) {
        sb = [UIStoryboard storyboardWithName:sb_name.stringByDeletingPathExtension bundle:nil];
        [_sb_cache setObject:sb forKey:sb_name];
    }
    return [sb instantiateViewControllerWithIdentifier:vc_name];
}

#pragma mark - Device

#import "sys/utsname.h"
NSString *FFDeviceModel() {
    struct utsname systemInfo;
    uname(&systemInfo);
    return [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
}
NSString *FFDeviceVersion() {
    return [UIDevice currentDevice].systemVersion;
}

#import <ifaddrs.h>
#import <arpa/inet.h>
NSString *FFDeviceIPAddress() {
    NSMutableDictionary *addresses = [NSMutableDictionary dictionaryWithCapacity:8];
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    // retrieve the current interfaces - returns 0 on success
    if (getifaddrs(&interfaces) == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Get NSString from C String
                NSString *chanel = [NSString stringWithUTF8String:temp_addr->ifa_name];
                NSString *address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                [addresses setObject:address forKey:chanel];
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    
    __block NSString *address =  @"0.0.0.0";
    NSArray *chanels = @[@"en0", @"pdp_ip0", @"lo0"]; // WIFI,CELLULAR,LOCAL
    [chanels enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        if ([addresses.allKeys indexOfObject:obj] != NSNotFound) {
            address = [addresses objectForKey:obj];
            *stop = YES;
        }
    }];
    return address;
}

NSString *FFDeviceUDIDString() {
    static NSString *UDIDString = nil;
    if (UDIDString == nil) {
        UDIDString = [FFKeyChain passwordForService:@"DEVICE_UDID" account:@"DEVICE_INFO" error:nil];
    }
    if (UDIDString == nil) {
        UDIDString = FFUUIDString();
        [FFKeyChain setPassword:UDIDString forService:@"DEVICE_UDID" account:@"DEVICE_INFO" error:nil];
    }
    return UDIDString;
}

#pragma mark - String
NSString *FFUUIDString(void) {
    CFUUIDRef uuidRef = CFUUIDCreate(NULL);
    NSString *uuid = CFBridgingRelease(CFUUIDCreateString(NULL, uuidRef));
    CFRelease(uuidRef);
    return uuid;
}

#import <CommonCrypto/CommonDigest.h>
id FFCryptoObject(id input) {
    if ([input respondsToSelector:@selector(length)] && [input length] == 0)
        return nil;
    if ([input respondsToSelector:@selector(count)] && [input count] == 0)
        return nil;
    if ([input isKindOfClass:[NSArray class]] || [input isKindOfClass:[NSDictionary class]]) {
        NSData *json = [NSJSONSerialization dataWithJSONObject:input options:NSJSONWritingPrettyPrinted error:nil];
        if (json) {
            input = [[NSString alloc] initWithData:json encoding:NSUTF8StringEncoding];
        }
    }
    return input;
}

NSString *FFMD5String(id input) {
    input = FFCryptoObject(input);
    if ([input isKindOfClass:[NSString class]]) {
        const char *ptr = [input UTF8String];
        unsigned char outputBuffer[CC_MD5_DIGEST_LENGTH];
        CC_MD5(ptr, (CC_LONG)strlen(ptr), outputBuffer);
        
        NSMutableString *outputString = [[NSMutableString alloc] initWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
        for(NSInteger count = 0; count < CC_MD5_DIGEST_LENGTH; count++){
            [outputString appendFormat:@"%02x", outputBuffer[count]];
        }
        return [outputString copy];
    } else if ([input isKindOfClass:[NSData class]]) {
        unsigned char md[CC_MD5_DIGEST_LENGTH];
        CC_MD5([input bytes], (CC_LONG)[input length], md);
        
        NSMutableString *digest = [NSMutableString string];
        for (NSUInteger i = 0; i < CC_MD5_DIGEST_LENGTH; i++) {
            [digest appendFormat:@"%02x", md[i]];
        }
        return [digest copy];
    }
    return nil;
}

NSString *FFSHA1String(id input) {
    input = FFCryptoObject(input);
    if ([input isKindOfClass:[NSString class]]) {
        const char *ptr = [input UTF8String];
        unsigned char outputBuffer[CC_SHA1_DIGEST_LENGTH];
        CC_SHA1(ptr, (CC_LONG)strlen(ptr), outputBuffer);
        
        NSMutableString *outputString = [[NSMutableString alloc] initWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
        for(NSInteger count = 0; count < CC_SHA1_DIGEST_LENGTH; count++){
            [outputString appendFormat:@"%02x", outputBuffer[count]];
        }
        return [outputString copy];
    } else if ([input isKindOfClass:[NSData class]]) {
        unsigned char md[CC_SHA1_DIGEST_LENGTH];
        CC_SHA1([input bytes], (CC_LONG)[input length], md);
        
        NSMutableString *digest = [NSMutableString string];
        for (NSUInteger i = 0; i < CC_SHA1_DIGEST_LENGTH; i++) {
            [digest appendFormat:@"%02x", md[i]];
        }
        return [digest copy];
    }
    return nil;
}

NSString *FFURLEncodeString(NSString *ret) {
    return CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,
                                                                     (CFStringRef)ret,
                                                                     (CFStringRef)@"!$&'()*+,-./:;=?@_~%#[]",
                                                                     NULL,
                                                                     kCFStringEncodingUTF8));
}

#pragma mark - Time
#include <sys/time.h>
NSTimeInterval FFTimeIntervalWithSecondsSince1970(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
int64_t FFTimeIntervalWithMilliSecondsSince1970(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (NSTimeInterval)tv.tv_usec / 1000.0;
}

#pragma mark - Perform
// Benchmark feature. Returns time in nanoseconds. (nsec/1E9 = seconds)
#include <mach/mach_time.h>
double FFPerformAndTrackTime(dispatch_block_t block, BOOL trackTime) {
    if (!trackTime) {
        block();
        return 0;
    }
    
    uint64_t startTime = mach_absolute_time();
    block();
    uint64_t endTime = mach_absolute_time();
    
    // Elapsed time in mach time units
    uint64_t elapsedTime = endTime - startTime;
    
    // The first time we get here, ask the system
    // how to convert mach time units to nanoseconds
    static double ticksToNanoseconds = 0.0;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        ticksToNanoseconds = (double)timebase.numer / timebase.denom;
    });
    return elapsedTime * ticksToNanoseconds;
}

void FFRetainAndPerformBlock(id self, dispatch_block_t block) {
    // We need to protect ourselves against early release, since it might be on the edge of being killed, but retained by the perform request alone.
    CFRetain((__bridge CFTypeRef)(self));
    block();
    CFRelease((__bridge CFTypeRef)(self));
}

#pragma mark - Window
void FFRestoreKeyWindow(void) {
    [UIApplication.sharedApplication.windows enumerateObjectsWithOptions:NSEnumerationReverse usingBlock:^(UIWindow *w, NSUInteger idx, BOOL *stop) {
        if ([w isMemberOfClass:UIWindow.class] && w.windowLevel == UIWindowLevelNormal && w.rootViewController && !w.hidden) {
            [w makeKeyWindow];
            *stop = YES;
        }
    }];
}

void FFRestoreKeyboardWindow(void) {
    if (!FFVersionGreaterThanIOS8()) return;
    [UIApplication.sharedApplication.windows enumerateObjectsWithOptions:NSEnumerationReverse usingBlock:^(UIWindow *w, NSUInteger idx, BOOL *stop) {
        if (!w.opaque && [NSStringFromClass(w.class) hasPrefix:@"UIText"]) { // windowLevel = 10
            // The keyboard sometimes disables interaction. This brings it back to normal.
            BOOL wasHidden = w.hidden;
            w.hidden = YES;
            w.hidden = wasHidden;
            *stop = YES;
        }
    }];
}

#pragma mark - Regular
UIViewController *FFWindowTopViewController(void) {
    UIViewController *vc = FFWindowRootViewController();
    while (vc.presentedViewController) {
        vc = vc.presentedViewController;
    }
    return vc;
}
UIViewController *FFWindowRootViewController(void) {
    UIViewController *vc = [UIApplication sharedApplication].keyWindow.rootViewController;
    if (!vc) {
        for (UIWindow *window in [UIApplication sharedApplication].windows) {
            vc = window.rootViewController;
            if (vc) break;
        }
    }
    if (!vc) return nil;
    if (!vc.view.window) return nil;
    return vc;
}

void FFReplaceBaseTableViewWithRegularView(UITableViewController *vc) {
    UITableView *tableView = (UITableView *)vc.view;
    tableView.translatesAutoresizingMaskIntoConstraints = NO;
    vc.view = [[UIView alloc] initWithFrame:tableView.bounds];
    vc.view.backgroundColor = [UIColor whiteColor];
    [vc.view addSubview:tableView];
    [vc.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[tableView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(tableView)]];
    [vc.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[tableView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(tableView)]];
}

#pragma mark - GCD
void ff_dispatch_async(dispatch_block_t block) {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        block();
    });
}
void ff_dispatch_main_async(dispatch_block_t block) {
    dispatch_async(dispatch_get_main_queue(), block);
}

void ff_dispatch_sync(dispatch_block_t block) {
    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        block();
    });
}
void ff_dispatch_main_sync(dispatch_block_t block) {
    dispatch_sync(dispatch_get_main_queue(), block);
}

NSString * readTxtFromPath(NSString *path)
{
    NSString *string = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    return string;
}

void writeTxtToPath(NSString *text, NSString *path)
{
    [text writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
}

BOOL CGFloatEqualWithFloat(CGFloat f1, CGFloat f2)
{
    return fabs(f1 - f2) < 0.0001;
}

int CGFloatCompareWithFloat(CGFloat f1, CGFloat f2)
{
    if (CGFloatEqualWithFloat(f1, f2)) {
        return 0;
    } else if (f1 > f2) {
        return 1;
    } else {
        return -1;
    }
}

CGFloat validValue(CGFloat f, CGFloat min, CGFloat max)
{
    if (f < min) {
        return min;
    } else if (f > max) {
        return max;
    }
    return f;
}
