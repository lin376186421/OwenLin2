//
//  FFYYModelEntity.h
//  Coach
//
//  Created by cheng on 16/5/25.
//  Copyright © 2016年 Chelun. All rights reserved.
//


@interface DTYYModelEntity : NSObject <YYModel>

//与DTEntiyBase 用法一致
+ (id)itemFromDict:(NSDictionary *)dict;
+ (NSArray *)itemsFromArray:(NSArray *)array;
+ (NSDictionary *)itemsDictFromDict:(NSDictionary *)dict;

+ (NSDictionary *)dictFromItem:(DTYYModelEntity *)item;
+ (NSArray *)arrayFromItems:(NSArray *)array;
- (NSDictionary *)dictItem;
- (NSDate *)dateFromDict:(NSDictionary *)dict forKey:(NSString *)key;

@end

