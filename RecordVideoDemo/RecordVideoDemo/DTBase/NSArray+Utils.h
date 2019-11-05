//
//  NSArray+Utils.h
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import <Foundation/Foundation.h>

// 合并数组，如果有相同的obj，仍会添加
extern NSArray *FFMergeArraies(NSArray *first, NSArray *second);

@interface NSArray (Utils)

// 返回符合条件的obj集合
- (NSArray *)filteredArrayUsingBlock:(BOOL(^)(id obj))block;

// 修改obj，返回修改后的数组
- (NSArray *)mappedArrayUsingBlock:(id (^)(id obj))block;

// 排序
- (NSArray *)sortedArrayWithKey:(NSString *)key ascending:(BOOL)ascending;
- (NSArray *)sortedArrayWithKeys:(NSArray *)keys ascending:(BOOL)ascending;
- (NSArray *)sortedArrayWithKeys:(NSArray *)keys ascendings:(NSArray*)ascendings;

// 将数组 以group的数量 划分 ,返回第index 个子数组
- (NSArray *)subArrayWithIndex:(NSInteger)index group:(NSInteger)group;

+ (instancetype)safeArrayWithObject:(id)object;

+ (BOOL)validArray:(NSArray *)array;
+ (BOOL)validArrayFromDict:(NSDictionary *)dict forKey:(NSString *)key;

+ (NSInteger)validCountAddOne:(NSArray *)array;
+ (NSInteger)validCount:(NSArray *)array add:(NSInteger)num;

@end


@interface NSMutableArray (Utils)

// 筛选数据
- (void)filterUsingBlock:(BOOL(^)(id obj))block;

// 交换相应位置的对象
- (void)moveObjectAtIndex:(NSUInteger)index toIndex:(NSUInteger)toIndex;

@end

