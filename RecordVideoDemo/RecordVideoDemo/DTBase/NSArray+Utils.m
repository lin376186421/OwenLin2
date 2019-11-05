//
//  NSArray+Utils.m
//  FFStory
//
//  Created by PageZhang on 14/11/18.
//  Copyright (c) 2014年 FF. All rights reserved.
//

#import "NSArray+Utils.h"
#import <libkern/OSAtomic.h>

NSArray *FFMergeArraies(NSArray *first, NSArray *second) {
    if (first && second.count == 0) return [first copy];  // Fast Path.
    if (second && first.count == 0) return [second copy]; // Fast Path.
    return [first arrayByAddingObjectsFromArray:second];
}

@implementation NSArray (Utils)

- (NSArray *)filteredArrayUsingBlock:(BOOL(^)(id obj))block {
    return [self filteredArrayWithOptions:0 usingBlock:block];
}
- (NSArray *)filteredArrayWithOptions:(NSEnumerationOptions)opts usingBlock:(BOOL(^)(id obj))block {
    return [self filteredArrayWithOptions:opts failedObjects:NULL usingBlock:block];
}
- (NSArray *)filteredArrayWithFailedObjects:(NSArray **)failedObjects usingBlock:(BOOL(^)(id obj))block {
    return [self filteredArrayWithOptions:0 failedObjects:failedObjects usingBlock:block];
}
- (NSArray *)filteredArrayWithOptions:(NSEnumerationOptions)opts failedObjects:(NSArray **)failedObjects usingBlock:(BOOL(^)(id obj))block {
    NSIndexSet *successIndexes = [self indexesOfObjectsWithOptions:opts passingTest:^(id obj, NSUInteger idx, BOOL *stop) {
        return block(obj);
    }];
    
    if (opts&NSEnumerationReverse) {
        NSMutableArray *mutableSuccess = [[NSMutableArray alloc] initWithCapacity:successIndexes.count];
        
        NSMutableArray *mutableFailed = nil;
        if (failedObjects != NULL) mutableFailed = [[NSMutableArray alloc] initWithCapacity:self.count - successIndexes.count - 1];
        
        [self enumerateObjectsWithOptions:NSEnumerationReverse usingBlock:^(id obj, NSUInteger index, BOOL *stop) {
            if ([successIndexes containsIndex:index])
                [mutableSuccess addObject:obj];
            else
                [mutableFailed addObject:obj];
        }];
        
        if (failedObjects != NULL) *failedObjects = [mutableFailed copy];
        
        return [mutableSuccess copy];
    }else {
        if (failedObjects != NULL) {
            NSUInteger totalCount = self.count;
            
            NSMutableIndexSet *failedIndexes = [[NSMutableIndexSet alloc] initWithIndexesInRange:NSMakeRange(0, totalCount)];
            [failedIndexes removeIndexes:successIndexes];
            
            *failedObjects = [self objectsAtIndexes:failedIndexes];
        }
        
        return [self objectsAtIndexes:successIndexes];
    }
}

- (NSArray *)mappedArrayUsingBlock:(id (^)(id obj))block {
    return [self mappedArrayWithOptions:0 usingBlock:block];
}
- (NSArray *)mappedArrayWithOptions:(NSEnumerationOptions)opts usingBlock:(id (^)(id obj))block {
    NSUInteger originalCount = self.count;
    
    BOOL concurrent = (opts&NSEnumerationConcurrent);
    BOOL reverse = (opts&NSEnumerationReverse);
    
    __strong volatile id *objects = (__strong id *)calloc(originalCount, sizeof(*objects));
    if (objects == NULL) return nil;
    
    NSUInteger actualCount = originalCount;
    
    // if this gets incremented while enumerating, 'objects' contains some
    // (indeterminate) number of nil values, and must be compacted before
    // creating an NSArray
    volatile int32_t needsCompaction = 0;
    
    {
        // create a pointer outside of the block so that we don't have to use the
        // __block qualifier in order to pass this variable to atomic functions
        volatile int32_t *needsCompactionPtr = &needsCompaction;
        
        [self enumerateObjectsWithOptions:opts usingBlock:^(id obj, NSUInteger index, BOOL *stop) {
            id result = block(obj);
            
            if (result == nil) {
                if (concurrent) {
                    // indicate that the array will need compaction, because it now has
                    // nil values in it
                    OSAtomicIncrement32(needsCompactionPtr);
                }else {
                    *needsCompactionPtr = 1;
                }
                
                return;
            }
            
            if (reverse)
                index = originalCount - index - 1;
            
            // only need to store into the array on success, since it was filled
            // with zeroes on allocation
            objects[index] = result;
        }];
        
        if (concurrent) {
            // finish all assignments into the 'objects' array and 'needsCompaction'
            // variable
            OSMemoryBarrier();
        }
    }
    
    if (needsCompaction) {
        for (NSUInteger index = 0; index < actualCount;) {
            if (objects[index]) {
                ++index;
                continue;
            }
            
            // otherwise, move down everything above
            memmove((void *)(objects + index), (void *)(objects + index + 1), sizeof(*objects) * (originalCount - index - 1));
            --actualCount;
        }
    }
    
    NSArray *array = [NSArray arrayWithObjects:(id *)objects count:actualCount];
    
    // cleanup
    for (NSUInteger i = 0; i < actualCount; ++i) {
        // nil out everything in the array to make sure ARC releases
        // everything appropriately
        objects[i] = nil;
    }
    free((void *)objects);
    
    return array;
}

- (NSArray *)sortedArrayWithKey:(NSString *)key ascending:(BOOL)ascending {
    return [self sortedArrayUsingDescriptors:@[[NSSortDescriptor sortDescriptorWithKey:key ascending:ascending]]];
}
- (NSArray *)sortedArrayWithKeys:(NSArray *)keys ascending:(BOOL)ascending {
    NSArray *descriptors = [keys mappedArrayUsingBlock:^id(id obj) {
        return [NSSortDescriptor sortDescriptorWithKey:obj ascending:ascending];
    }];
    return [self sortedArrayUsingDescriptors:descriptors];
}
// add by owen
- (NSArray *)sortedArrayWithKeys:(NSArray *)keys ascendings:(NSArray*)ascendings {
    NSMutableArray *descriptors = [NSMutableArray array];
    for (int i = 0; i < keys.count; i ++) {
        NSSortDescriptor *sort = [NSSortDescriptor sortDescriptorWithKey:[keys objectAtIndex:i] ascending:[[ascendings objectAtIndex:i] boolValue]];
        [descriptors addObject:sort];
    }
    //    NSArray *descriptors = [keys mappedArrayUsingBlock:^id(id obj) {
    //        return [NSSortDescriptor sortDescriptorWithKey:obj ascending:ascending];
    //    }];
    return [self sortedArrayUsingDescriptors:descriptors];
}

- (id)objectPassingTest:(BOOL (^)(id obj, NSUInteger index, BOOL *stop))predicate {
    return [self objectWithOptions:0 passingTest:predicate];
}
- (id)objectWithOptions:(NSEnumerationOptions)opts passingTest:(BOOL (^)(id obj, NSUInteger index, BOOL *stop))predicate {
    NSUInteger index = [self indexOfObjectWithOptions:opts passingTest:predicate];
    return index != NSNotFound ? self[index] : nil;
}

- (NSArray *)subArrayWithIndex:(NSInteger)index group:(NSInteger)group {
    if (index<0||group<=0) {
        return nil;
    }
    NSInteger n = group;
    if ((index+1)*group>=self.count) {
        n = self.count-index*group;
    }
    if (n>0) {
        return [self subarrayWithRange:NSMakeRange(index*group, n)];
    }
    return nil;
}

+ (instancetype)safeArrayWithObject:(id)object
{
    if (object) {
        return [self arrayWithObject:object];
    }
    return nil;
}

+ (BOOL)validArray:(NSArray *)array
{
    if (array && [array isKindOfClass:[NSArray class]] && array.count) {
        return YES;
    }
    return NO;
}

+ (BOOL)validArrayFromDict:(NSDictionary *)dict forKey:(NSString *)key
{
    if (!key.length) {
        return NO;
    }
    return [dict isKindOfClass:[NSDictionary class]] && [self validArray:[dict objectForKey:key]];
}

+ (NSInteger)validCountAddOne:(NSArray *)array
{
    return [self validCount:array add:1];
}

+ (NSInteger)validCount:(NSArray *)array add:(NSInteger)num
{
    if ([self validArray:array]) {
        return array.count + num;
    }
    return 0;
}

@end


@implementation NSMutableArray (Utils)

- (void)filterUsingBlock:(BOOL(^)(id obj))block {
    [self filterUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(id evaluatedObject, NSDictionary *bindings) {
        return block(evaluatedObject);
    }]];
}

- (void)moveObjectAtIndex:(NSUInteger)index toIndex:(NSUInteger)toIndex {
    if (index == toIndex) return;
    
    id tmp = self[index];
    [self removeObjectAtIndex:index];
    [self insertObject:tmp atIndex:toIndex];
}

@end

