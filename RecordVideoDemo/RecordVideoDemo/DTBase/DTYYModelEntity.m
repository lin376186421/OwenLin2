//
//  FFYYModelEntity.m
//  Coach
//
//  Created by cheng on 16/5/25.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "DTYYModelEntity.h"
//#import "NSDate+Utils.h"
#import "BPCoreUtil.h"

@implementation DTYYModelEntity

//- (NSDate *)dateFromDict:(NSDictionary *)dict forKey:(NSString *)key
//{
//    return [NSDate dateFromDict:dict forKey:key];
//}

+ (id)itemFromDict:(NSDictionary *)dict
{
    if (dict==nil) return nil;
    
    if (dict) {
        if (![dict isKindOfClass:[NSDictionary class]] || dict.count <= 0) {
            return nil;
        }
    }
    
    return [self yy_modelWithDictionary:dict];
}

+ (NSArray *)itemsFromArray:(NSArray *)array
{
    if (array==nil) return nil;
    
    if (array&&![array isKindOfClass:[NSArray class]]) {
        return nil;
    }
    
    NSMutableArray *mArray = [NSMutableArray arrayWithCapacity:[array count]];
    for (NSDictionary *dict in array) {
        if ((id)dict == [NSNull null]) {
            continue;
        }
        id item = [self itemFromDict:dict];
        if (item) {
            [mArray addObject:item];
        }
    }
    return mArray;
}

+ (NSArray *)arrayFromItems:(NSArray *)array {
    if (array == nil) return nil;
    
    NSMutableArray *mArray = [NSMutableArray arrayWithCapacity:[array count]];
    for (id item in array) {
        NSDictionary *dict = [self dictFromItem:item];
        if (dict)
            [mArray addObject:dict];
    }
    return mArray;
}

+ (NSDictionary *)itemsDictFromDict:(NSDictionary *)dict
{
    if (dict == nil) return nil;
    if ([dict isKindOfClass:[NSDictionary class]]) {
        __block NSMutableDictionary *result = [NSMutableDictionary dictionary];
        [dict enumerateKeysAndObjectsUsingBlock:^(NSString * key, NSDictionary *obj, BOOL * _Nonnull stop) {
            id item = [self itemFromDict:obj];
            if (item) {
//                [result safeSetObject:item forKey:key];
                [result setObject:item forKey:key];
            }
        }];
        return result;
    }
    return nil;
}

+ (NSDictionary<NSString *,id> *)modelCustomPropertyMapper
{
    /**
     * return @{@"itemId"     : @"id",
     *          @"schoolName" : @"school.name"};
     */
    return nil;
}

+ (NSDictionary<NSString *,id> *)modelContainerPropertyGenericClass
{
    /**
     * return @{@"sign_list" : [CCTrainingSignItem class]};
     */
    return nil;
}

- (BOOL)modelCustomTransformFromDictionary:(NSDictionary *)dic
{
    /**
     * self.start_date = [self dateFromDict:dic forKey:@"start_time"];
     * self.end_date = [self dateFromDict:dic forKey:@"end_time"];
     */
    return YES;
}

- (NSDictionary *)dictItem
{
    return [self yy_modelToJSONObject];
}

+ (NSDictionary *)dictFromItem:(DTYYModelEntity *)item
{
    return [item dictItem];
}

@end

