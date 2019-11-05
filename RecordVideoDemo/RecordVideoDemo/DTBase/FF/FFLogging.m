//
//  FFLogging.m
//  FFStory
//
//  Created by PageZhang on 14/11/12.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "FFLogging.h"

void FFAssertIfNotMainThread(void) {
    FFAssert(NSThread.isMainThread, @"\nERROR: All calls to UIKit need to happen on the main thread. You have a bug in your code. Use dispatch_async(dispatch_get_main_queue(), ^{ ... }); if you're unsure what thread you're in.\n\nBreak on PSPDFAssertIfNotMainThread to find out where.\n\nStacktrace: %@", NSThread.callStackSymbols);
}

// 误当成：NSString、NSArray、NSDictionary
@interface NSNull (SafeMethod)
@end
@implementation NSNull (SafeMethod)
- (void)length {
    FFLog(@"Exception!!!");
}
- (void)count {
    FFLog(@"Exception!!!");
}
@end

// 误当成：NSDictionary
@interface NSArray (SafeMethod)
@end
@implementation NSArray (SafeMethod)
- (id)objectForKey:(id)aKey {
    FFLog(@"Exception!!!");
    return nil;
}
- (id)objectForKeyedSubscript:(id <NSCopying>)key {
    FFLog(@"Exception!!!");
    return nil;
}
@end

// 误当成：NSArray
@interface NSDictionary (SafeMethod)
@end
@implementation NSDictionary (SafeMethod)
- (id)firstObject {
    FFLog(@"Exception!!!");
    return nil;
}
- (id)lastObject {
    FFLog(@"Exception!!!");
    return nil;
}
- (id)objectAtIndex:(NSUInteger)index {
    FFLog(@"Exception!!!");
    return nil;
}
- (id)objectAtIndexedSubscript:(NSUInteger)idx {
    FFLog(@"Exception!!!");
    return nil;
}
@end