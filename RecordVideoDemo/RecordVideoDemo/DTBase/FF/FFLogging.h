//
//  FFLogging.h
//  FFStory
//
//  Created by PageZhang on 14/11/12.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <Foundation/Foundation.h>

// Assert if we're not on the main thread.
extern void FFAssertIfNotMainThread(void);

#ifdef DEBUG

#define FFLogVar(var)       FFLog(@"%s = %@", #var, __ff_box(@encode(__typeof__(var)), (var)))
#define FFLog(fmt, ...)     NSLog(@"🎈%s + %d📍 %@", __PRETTY_FUNCTION__, __LINE__, [NSString stringWithFormat:fmt, ##__VA_ARGS__])

// A better assert. NSAssert is too runtime dependent, and assert() doesn't log.
// http://www.mikeash.com/pyblog/friday-qa-2014-05-03-proper-use-of-asserts.html
// Accepts both:
// - FFAssert(x > 0);
// - FFAssert(y > 3, @"Bad value for y");
#define FFAssert(expression, ...) \
do { if(!(expression)) { \
FFLog(@"%@", [NSString stringWithFormat: @"Assertion failure [ %s ] : %@", #expression, [NSString stringWithFormat:@"" __VA_ARGS__]]); \
abort(); }} while(0)

#else

#define FFLogVar(var)               {}
#define FFLog(fmt, ...)             {}
#define FFAssert(expression, ...)   {}

#endif

static inline id __ff_box(const char * type, ...)
{
    va_list variable_param_list;
    va_start(variable_param_list, type);
    
    id object = nil;
    if (strcmp(type, @encode(id)) == 0) {
        id param = va_arg(variable_param_list, id);
        object = param;
    } else if (strcmp(type, @encode(CGPoint)) == 0) {
        CGPoint param = (CGPoint)va_arg(variable_param_list, CGPoint);
        object = [NSValue valueWithCGPoint:param];
    } else if (strcmp(type, @encode(CGSize)) == 0) {
        CGSize param = (CGSize)va_arg(variable_param_list, CGSize);
        object = [NSValue valueWithCGSize:param];
    } else if (strcmp(type, @encode(CGVector)) == 0) {
        CGVector param = (CGVector)va_arg(variable_param_list, CGVector);
        object = [NSValue valueWithCGVector:param];
    } else if (strcmp(type, @encode(CGRect)) == 0) {
        CGRect param = (CGRect)va_arg(variable_param_list, CGRect);
        object = [NSValue valueWithCGRect:param];
    } else if (strcmp(type, @encode(NSRange)) == 0) {
        NSRange param = (NSRange)va_arg(variable_param_list, NSRange);
        object = [NSValue valueWithRange:param];
    } else if (strcmp(type, @encode(CFRange)) == 0) {
        CFRange param = (CFRange)va_arg(variable_param_list, CFRange);
        object = [NSValue value:&param withObjCType:type];
    } else if (strcmp(type, @encode(CGAffineTransform)) == 0) {
        CGAffineTransform param = (CGAffineTransform)va_arg(variable_param_list, CGAffineTransform);
        object = [NSValue valueWithCGAffineTransform:param];
    } else if (strcmp(type, @encode(CATransform3D)) == 0) {
        CATransform3D param = (CATransform3D)va_arg(variable_param_list, CATransform3D);
        object = [NSValue valueWithCATransform3D:param];
    } else if (strcmp(type, @encode(SEL)) == 0) {
        SEL param = (SEL)va_arg(variable_param_list, SEL);
        object = NSStringFromSelector(param);
    } else if (strcmp(type, @encode(Class)) == 0) {
        Class param = (Class)va_arg(variable_param_list, Class);
        object = NSStringFromClass(param);
    } else if (strcmp(type, @encode(UIOffset)) == 0) {
        UIOffset param = (UIOffset)va_arg(variable_param_list, UIOffset);
        object = [NSValue valueWithUIOffset:param];
    } else if (strcmp(type, @encode(UIEdgeInsets)) == 0) {
        UIEdgeInsets param = (UIEdgeInsets)va_arg(variable_param_list, UIEdgeInsets);
        object = [NSValue valueWithUIEdgeInsets:param];
    } else if (strcmp(type, @encode(short)) == 0) {
        short param = (short)va_arg(variable_param_list, int);
        object = @(param);
    } else if (strcmp(type, @encode(int)) == 0) {
        int param = (int)va_arg(variable_param_list, int);
        object = @(param);
    } else if (strcmp(type, @encode(long)) == 0) {
        long param = (long)va_arg(variable_param_list, long);
        object = @(param);
    } else if (strcmp(type, @encode(long long)) == 0) {
        long long param = (long long)va_arg(variable_param_list, long long);
        object = @(param);
    } else if (strcmp(type, @encode(float)) == 0) {
        float param = (float)va_arg(variable_param_list, double);
        object = @(param);
    } else if (strcmp(type, @encode(double)) == 0) {
        double param = (double)va_arg(variable_param_list, double);
        object = @(param);
    } else if (strcmp(type, @encode(BOOL)) == 0) {
        BOOL param = (BOOL)va_arg(variable_param_list, int);
        object = param ? @"YES" : @"NO";
    } else if (strcmp(type, @encode(bool)) == 0) {
        bool param = (bool)va_arg(variable_param_list, int);
        object = param ? @"true" : @"false";
    } else if (strcmp(type, @encode(char)) == 0) {
        char param = (char)va_arg(variable_param_list, int);
        object = [NSString stringWithFormat:@"%c", param];
    } else if (strcmp(type, @encode(unsigned short)) == 0) {
        unsigned short param = (unsigned short)va_arg(variable_param_list, unsigned int);
        object = @(param);
    } else if (strcmp(type, @encode(unsigned int)) == 0) {
        unsigned int param = (unsigned int)va_arg(variable_param_list, unsigned int);
        object = @(param);
    } else if (strcmp(type, @encode(unsigned long)) == 0) {
        unsigned long param = (unsigned long)va_arg(variable_param_list, unsigned long);
        object = @(param);
    } else if (strcmp(type, @encode(unsigned long long)) == 0) {
        unsigned long long param = (unsigned long long)va_arg(variable_param_list, unsigned long long);
        object = @(param);
    } else if (strcmp(type, @encode(unsigned char)) == 0) {
        unsigned char param = (unsigned char)va_arg(variable_param_list, unsigned int);
        object = [NSString stringWithFormat:@"%c", param];
    } else {
        void * param = (void *)va_arg(variable_param_list, void *);
        object = [NSString stringWithFormat:@"%p", param];
    }
    
    va_end(variable_param_list);
    
    return object;
}


