//
//  FFJSONHelper.h
//  FFStory
//
//  Created by PageZhang on 15/7/15.
//  Copyright (c) 2015年 PG. All rights reserved.
//

#import <Foundation/Foundation.h>

// JSON对象
@interface NSString (JSON)
- (id)JSONObject;
@end

@interface NSData (JSON)
- (id)JSONObject;
@end


// JSON数据
@interface NSArray (JSON)
- (NSData *)JSONData;
- (NSString *)JSONString;
@end

@interface NSDictionary (JSON)
- (NSData *)JSONData;
- (NSString *)JSONString;
@end