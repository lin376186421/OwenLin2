//
//  FFFormatHelper.m
//  FFStory
//
//  Created by PageZhang on 15/9/11.
//  Copyright (c) 2015年 eclicks. All rights reserved.
//

#import "FFFormatHelper.h"
#import "NSDate+Utils.h"

NSString *FFStatementFileSize(uint64_t size) {
    if (size < pow(1024, 1)*100) {
        return [NSString stringWithFormat:@"%.1f KB", size/1024.0];
    } else if (size < pow(1024, 2)*100) {
        return [NSString stringWithFormat:@"%.1f MB", size/pow(1024, 2)];
    } else if (size < pow(1024, 3)*100) {
        return [NSString stringWithFormat:@"%.1f GB", size/pow(1024, 3)];
    } else {
        return [NSString stringWithFormat:@"%.1f TB", size/pow(1024, 4)];
    }
}

NSString *FFStatementDistance(CGFloat distance) {
    if (distance <= 0) {
        return @"";
    }
    if (distance < 1000) {
        return [NSString stringWithFormat:@"%.0fm", distance];
    } else {
        return [NSString stringWithFormat:@"%.1fkm", distance/1000];
    }
}

NSString *FFStatementTimeInterval(NSTimeInterval timeInterval) {
    NSTimeInterval current = FFTimeIntervalWithSecondsSince1970();
    if (timeInterval > current) {
        return [[NSDate dateWithTimeIntervalSince1970:timeInterval] stringValue:yyyyMMddHHmm];
    } else {
        NSTimeInterval intval = current - timeInterval;
        if (intval < 60) {
            return [NSString stringWithFormat:@"%.0f秒前", intval];
        } else if (intval < 60 * 60) {
            return [NSString stringWithFormat:@"%.0f分钟前", intval/60];
        } else if (intval < 60 * 60 * 24 ) {
            return [NSString stringWithFormat:@"%.0f小时前", intval/60/60];
        } else if (intval < 60 * 60 * 24 * 30) {
            return [NSString stringWithFormat:@"%.0f天前", intval/60/60/24];
        } else {
            return [[NSDate dateWithTimeIntervalSince1970:timeInterval] stringValue:yyyyMMdd];
        }
    }
}

NSString *FFStatementNumber(CGFloat number) {
    if (number <= 0.01) {
        return @"0.01";
    }
    // 小数点前几位（至少一位）
    NSUInteger head = 1;
    while (number / pow(10, head) >= 1) {
        head++;
    }
    // 小数点后几位
    NSUInteger tail = 0;
    while ((int)(number * pow(10, tail)) / pow(10, tail) != number) {
        if (++tail >= 2) {
            break;
        }
    }
    // 加上小数点
    if (tail > 0) {
        tail++;
    }
    NSString *value = [NSString stringWithFormat:@"%f", number];
    return [value substringToIndex:MIN(value.length, head+tail)];
}



BOOL FFisValidateEmail(NSString *email) {
    NSString *regex = @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    return [predicate evaluateWithObject:email];
}

BOOL FFisValidatePhone(NSString *phone) {
    NSString *regex = @"^(86)?((13[0-9])|(15[^4,\\D])|(17[0135-8,\\D])|(18[0-9]))\\d{8}$";
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",regex];
    return [predicate evaluateWithObject:phone];
}

BOOL FFisValidateIDCard(NSString *number) {
    NSString *regex = @"^(\\d{14}|\\d{17})(\\d|[xX])$";
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",regex];
    return [predicate evaluateWithObject:number];
}


