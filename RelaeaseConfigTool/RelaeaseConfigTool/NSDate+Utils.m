//
//  NSDate+Utils.m
//  FFStory
//
//  Created by PageZhang on 14/11/17.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "NSDate+Utils.h"

@implementation NSDate (Utils)

+ (NSCalendar *)sharedCalendar {
    static __strong NSCalendar *_calendar = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _calendar = [NSCalendar currentCalendar];
    });
    return _calendar;
}
+ (NSDateFormatter *)sharedDateFormatter {
    static __strong NSDateFormatter *_formatter = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _formatter = [[NSDateFormatter alloc] init];
    });
    return _formatter;
}

+ (NSDate *)datePlus:(NSTimeInterval)interval {
    return [[NSDate date] dateByAddingTimeInterval:interval];
}
+ (NSDate *)dateMinus:(NSTimeInterval)interval {
    return [[NSDate date] dateByAddingTimeInterval:-interval];
}

///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - format
+ (NSDateFormatter *)shorTimeFormatter {
    NSDateFormatter *formatter = [self sharedDateFormatter];
    [formatter setTimeStyle:NSDateFormatterShortStyle];
    return formatter;
}
+ (NSDateFormatter *)shortDateFormatter {
    NSDateFormatter *formatter = [self sharedDateFormatter];
    [formatter setDateStyle:NSDateFormatterShortStyle];
    return formatter;
}
+ (NSDateFormatter *)weekFormatter {
    NSDateFormatter *formatter = [self sharedDateFormatter];
    [formatter setDateFormat:@"c"];
    return formatter;
}

- (NSString *)dayOfTheWeek {
    NSUInteger week = [[[NSDate weekFormatter] stringFromDate:self] integerValue];
    static NSArray *weeks = nil;
    if (!weeks) weeks = @[@"日", @"一", @"二", @"三", @"四", @"五", @"六"];
    return weeks[week-1];
}

- (NSString *)stringValue:(NSString *)dateFormat {
    NSDateFormatter *dateFormatter = [NSDate sharedDateFormatter];
    dateFormatter.dateFormat = dateFormat;
    return [dateFormatter stringFromDate:self];
}

- (NSString*)stringFormatterDay
{
    NSCalendar* cal = [NSCalendar currentCalendar];
    NSDateComponents* day = [cal components:NSDayCalendarUnit|NSMonthCalendarUnit|NSYearCalendarUnit
                                   fromDate:self];
    NSDateComponents* today = [cal components:NSDayCalendarUnit|NSMonthCalendarUnit|NSYearCalendarUnit
                                     fromDate:[NSDate date]];
    NSDateComponents* yesterday = [cal components:NSDayCalendarUnit|NSMonthCalendarUnit|NSYearCalendarUnit
                                         fromDate:[[NSDate date] dateByAddingTimeInterval:-(24*60*60.f)]];
    
    if (day.day == today.day && day.month == today.month && day.year == today.year) {
        return @"今天";
    } else if (day.day == yesterday.day && day.month == yesterday.month
               && day.year == yesterday.year) {
        return @"昨天";
    } else {
        return [self stringValue:yyyy_Md];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
#pragma mark - convert
+ (NSInteger)nowValue:(NSCalendarUnit)unit {
    return [[NSDate date] thenValue:unit];
}
- (NSInteger)thenValue:(NSCalendarUnit)unit {
    NSDateComponents *comps = [[self.class sharedCalendar] components:unit fromDate:self];
    switch (unit) {
        case NSCalendarUnitYear:   return [comps year];
        case NSCalendarUnitMonth:  return [comps month];
        case NSCalendarUnitDay:    return [comps day];
        case NSCalendarUnitHour:   return [comps hour];
        case NSCalendarUnitMinute: return [comps minute];
        case NSCalendarUnitSecond: return [comps second];
        default: return 0;
    }
}

- (NSInteger)valueToNow:(NSCalendarUnit)unit {
    if (unit == NSCalendarUnitDay) {
        // 需要先把时间转成年月日格式，才能得到准确的天数
        NSDateComponents *comps1 = [[self.class sharedCalendar] components:NSCalendarUnitYear|NSCalendarUnitMonth|NSCalendarUnitDay fromDate:self];
        NSDate *date1 = [[self.class sharedCalendar] dateFromComponents:comps1];
        NSDateComponents *comps2 = [[NSDate sharedCalendar] components:NSCalendarUnitYear|NSCalendarUnitMonth|NSCalendarUnitDay fromDate:[NSDate date]];
        NSDate *date2 = [[NSDate sharedCalendar] dateFromComponents:comps2];
        NSDateComponents *comps3 = [[self.class sharedCalendar] components:NSCalendarUnitDay fromDate:date1 toDate:date2 options:0];
        return [comps3 day];
    } else {
        NSDateComponents *comps = [[self.class sharedCalendar] components:unit fromDate:self toDate:[NSDate date] options:0];
        switch (unit) {
            case NSCalendarUnitYear:   return [comps year];
            case NSCalendarUnitMonth:  return [comps month];
            case NSCalendarUnitHour:   return [comps hour];
            case NSCalendarUnitMinute: return [comps minute];
            case NSCalendarUnitSecond: return [comps second];
            default: return 0;
        }
    }
}

+ (NSString *)todayString
{
    return [[NSDate date] stringValue:yyyyMMdd];
}

+ (NSDate *)dateFromDateStr:(NSString *)dateStr formatter:(NSString *)formatter
{
    NSDateFormatter *dateFormatter = [self sharedDateFormatter];
    dateFormatter.dateFormat = formatter;
    return [dateFormatter dateFromString:dateStr];
}

+ (NSTimeInterval)timeIntervalFromDateStr:(NSString *)dateStr formatter:(NSString *)formatter
{
    NSDate *date = [self dateFromDateStr:dateStr formatter:formatter];
    NSTimeInterval times = [date timeIntervalSince1970];
    return times;
}

+ (NSDateComponents *)dateComponentsFromDateString:(NSString *)string
{
    NSArray *array = [string componentsSeparatedByString:@"-"];
    if (array.count==3) {
        NSDateComponents *components = [[NSDateComponents alloc] init];
        components.year = [[array objectAtIndex:0] integerValue];
        components.month = [[array objectAtIndex:1] integerValue];
        components.day = [[array objectAtIndex:2] integerValue];
        return components;
    }
    return nil;
}

@end


@implementation NSDate (Dict)

+ (NSDate *)dateFromDict:(NSDictionary *)dict forKey:(NSString *)key
{
    if (dict&&key&&[dict isKindOfClass:[NSDictionary class]]) {
        if ([[dict objectForKey:key] doubleValue]>0.f) {
            return [NSDate dateWithTimeIntervalSince1970:[[dict objectForKey:key] doubleValue]];
        }
    }
    return nil;
}

@end

@implementation NSDate (Compare)

+ (BOOL)NoLaterToday:(NSString *)string
{
    NSArray *array = [string componentsSeparatedByString:@"-"];
    NSDateComponents *components = [[NSCalendar currentCalendar] components:NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit fromDate:[NSDate date]];
    NSArray *today = @[@(components.year), @(components.month), @(components.day)];
    if (array.count == today.count) {
        for (NSInteger i=0; i<today.count; i++) {
            if ([array[i] integerValue] == [today[i] integerValue]) {
                continue;
            } else {
                return [array[i] integerValue] < [today[i] integerValue];
            }
        }
        //同一天
        return YES;
    }
    return NO;
}

+ (BOOL)NoEarlierToday:(NSString *)string
{
    NSArray *array = [string componentsSeparatedByString:@"-"];
    NSDateComponents *components = [[NSCalendar currentCalendar] components:NSDayCalendarUnit | NSMonthCalendarUnit | NSYearCalendarUnit fromDate:[NSDate date]];
    NSArray *today = @[@(components.year), @(components.month), @(components.day)];
    if (array.count == today.count) {
        for (NSInteger i=0; i<today.count; i++) {
            if ([array[i] integerValue] == [today[i] integerValue]) {
                continue;
            } else {
                return [array[i] integerValue] > [today[i] integerValue];
            }
        }
        //同一天
        return YES;
    }
    return NO;
}

+ (BOOL)todayIsValidWithBegin:(NSString *)begin andEnd:(NSString *)end
{
    return [self NoLaterToday:begin] && [self NoEarlierToday:end];
}

@end
