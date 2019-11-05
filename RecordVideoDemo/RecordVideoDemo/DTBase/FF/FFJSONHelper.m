
//
//  FFJSONHelper.m
//  FFStory
//
//  Created by PageZhang on 15/7/15.
//  Copyright (c) 2015年 PG. All rights reserved.
//

#import "FFJSONHelper.h"

@implementation NSString (JSON)
- (id)JSONObject {
    NSData *json = [self dataUsingEncoding:NSUTF8StringEncoding];
    return [json JSONObject];
}
@end

@implementation NSData (JSON)
- (id)JSONObject {
    return [NSJSONSerialization JSONObjectWithData:self options:NSJSONReadingAllowFragments error:nil];
}
@end


@implementation NSArray (JSON)
- (NSData *)JSONData {
    return self.count ? [NSJSONSerialization dataWithJSONObject:self options:NSJSONWritingPrettyPrinted error:nil] : nil;
}
- (NSString *)JSONString {
    NSData *data = [self JSONData];
    return data ? [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] : nil;
}
@end

@implementation NSDictionary (JSON)
- (NSData *)JSONData {
    return self.count ? [NSJSONSerialization dataWithJSONObject:self options:NSJSONWritingPrettyPrinted error:nil] : nil;
}
- (NSString *)JSONString {
    NSData *data = [self JSONData];
    return data ? [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] : nil;
}
@end

