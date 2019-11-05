//
//  NSDate+Utils.h
//  FFStory
//
//  Created by PageZhang on 14/11/17.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <Foundation/Foundation.h>

#define yyyy_Md         @"M月d号"                // 2月3号
#define yyyy_Hmm        @"H:mm"                 // 8:03
#define yyyy_HHmm       @"HH:mm"                // 08:03
#define yyyyMMdd        @"yyyy-MM-dd"           // 2016-02-03
#define yyyyMMddHHmm    @"yyyy-MM-dd HH:mm"     // 2016-02-03 08:03
#define yyyyMMddHHmmss  @"yyyy-MM-dd HH:mm:ss"  // 2016-02-03 08:03:18

@interface NSDate (Utils)

+ (NSCalendar *)sharedCalendar;

// 系统时间表示
+ (NSDateFormatter *)shorTimeFormatter;
+ (NSDateFormatter *)shortDateFormatter;

// 转换为字符串
- (NSString *)dayOfTheWeek; // 日、一、二..
- (NSString *)stringValue:(NSString *)dateFormat;

// 操作指定秒数
+ (NSDate *)datePlus:(NSTimeInterval)interval;
+ (NSDate *)dateMinus:(NSTimeInterval)interval;

// 当前的年月日，小时，分钟，秒数
+ (NSInteger)nowValue:(NSCalendarUnit)unit;

// 给定时间对应的年月日
- (NSInteger)thenValue:(NSCalendarUnit)unit;

// 给定时间到当前的天数，月数，年数，小时，分钟，秒数
- (NSInteger)valueToNow:(NSCalendarUnit)unit;

//返回 昨天 今天 或1月1日
- (NSString*)stringFormatterDay;

+ (NSString *)todayString;

//将2016-01-01 转化成date
+ (NSDate *)dateFromDateStr:(NSString *)dateStr formatter:(NSString *)formatter;

+ (NSTimeInterval)timeIntervalFromDateStr:(NSString *)dateStr formatter:(NSString *)formatter;

//string : 2017-01-01
+ (NSDateComponents *)dateComponentsFromDateString:(NSString *)string;

@end

@interface NSDate (Dict)

+ (NSDate *)dateFromDict:(NSDictionary *)dict forKey:(NSString *)key;

@end

@interface NSDate (Compare)

+ (BOOL)NoLaterToday:(NSString *)string;//不晚于 今天
+ (BOOL)NoEarlierToday:(NSString *)string;//不早于 今天
+ (BOOL)todayIsValidWithBegin:(NSString *)begin andEnd:(NSString *)end;

@end
