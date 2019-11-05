//
//  FFFormatHelper.h
//  FFStory
//
//  Created by PageZhang on 15/9/11.
//  Copyright (c) 2015年 eclicks. All rights reserved.
//

#import <Foundation/Foundation.h>

// 大小：KB、MB
extern NSString *FFStatementFileSize(uint64_t size);

// 距离：m、km
extern NSString *FFStatementDistance(CGFloat distance);

// 时间：天、小时、分钟、秒
extern NSString *FFStatementTimeInterval(NSTimeInterval timeInterval);

// 根据小数点后位数返回，最多两位
extern NSString *FFStatementNumber(CGFloat number);


// 是否邮件地址
extern BOOL FFisValidateEmail(NSString *email);
// 是否手机号码（中国）
extern BOOL FFisValidatePhone(NSString *phone);
// 是否身份证号码
extern BOOL FFisValidateIDCard(NSString *number);
