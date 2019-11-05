//
//  FFKeyChain.m
//  FFStory
//
//  Created by PageZhang on 14/11/19.
//  Copyright (c) 2014年 PageZhang. All rights reserved.
//

#import "FFKeyChain.h"

NSString *const FFKeyChainErrorDomain     = @"keychain";
NSString *const FFKeyChainAccountKey      = @"acct";
NSString *const FFKeyChainCreatedAtKey    = @"cdat";
NSString *const FFKeyChainClassKey        = @"labl";
NSString *const FFKeyChainDescriptionKey  = @"desc";
NSString *const FFKeyChainLabelKey        = @"labl";
NSString *const FFKeyChainLastModifiedKey = @"mdat";
NSString *const FFKeyChainWhereKey        = @"svce";

@interface FFKeyChain ()
+ (NSMutableDictionary *)_queryForService:(NSString *)service account:(NSString *)account;
@end

@implementation FFKeyChain

#pragma mark - Getting Accounts

+ (NSArray *)allAccounts {
    return [self accountsForService:nil error:NULL];
}
+ (NSArray *)allAccounts:(NSError **)error {
    return [self accountsForService:nil error:error];
}
+ (NSArray *)accountsForService:(NSString *)service {
    return [self accountsForService:service error:NULL];
}
+ (NSArray *)accountsForService:(NSString *)service error:(NSError **)error {
    OSStatus status = FFKeyChainErrorBadArguments;
    NSMutableDictionary *query = [self _queryForService:service account:nil];
    query[(__bridge id)kSecReturnAttributes] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitAll;
    
    CFTypeRef result = NULL;
    status = SecItemCopyMatching((__bridge CFDictionaryRef)query, &result);
    if (status != noErr && error != NULL) {
        *error = [NSError errorWithDomain:FFKeyChainErrorDomain code:status userInfo:nil];
        return nil;
    }
    
    return (__bridge_transfer NSArray *)result;
}

#pragma mark - Getting Passwords

+ (NSString *)passwordForService:(NSString *)service account:(NSString *)account {
    return [self passwordForService:service account:account error:NULL];
}
+ (NSString *)passwordForService:(NSString *)service account:(NSString *)account error:(NSError **)error {
    NSData *data = [self passwordDataForService:service account:account error:error];
    if (data.length > 0) {
        NSString *string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        return string;
    }
    
    return nil;
}

+ (NSData *)passwordDataForService:(NSString *)service account:(NSString *)account {
    return [self passwordDataForService:service account:account error:NULL];
}
+ (NSData *)passwordDataForService:(NSString *)service account:(NSString *)account error:(NSError **)error {
    OSStatus status = FFKeyChainErrorBadArguments;
    if (!service || !account) {
        if (error) {
            *error = [NSError errorWithDomain:FFKeyChainErrorDomain code:status userInfo:nil];
        }
        return nil;
    }
    
    CFTypeRef result = NULL;
    NSMutableDictionary *query = [self _queryForService:service account:account];
    query[(__bridge id)kSecReturnData] = (__bridge id)kCFBooleanTrue;
    query[(__bridge id)kSecMatchLimit] = (__bridge id)kSecMatchLimitOne;
    status = SecItemCopyMatching((__bridge CFDictionaryRef)query, &result);
    
    if (status != noErr && error != NULL) {
        *error = [NSError errorWithDomain:FFKeyChainErrorDomain code:status userInfo:nil];
        return nil;
    }
    
    return (__bridge_transfer NSData *)result;
}

#pragma mark - Setting Passwords

+ (BOOL)setPassword:(NSString *)password forService:(NSString *)service account:(NSString *)account {
    return [self setPassword:password forService:service account:account error:NULL];
}
+ (BOOL)setPassword:(NSString *)password forService:(NSString *)service account:(NSString *)account error:(NSError **)error {
    NSData *data = [password dataUsingEncoding:NSUTF8StringEncoding];
    return [self setPasswordData:data forService:service account:account error:error];
}
+ (BOOL)setPasswordData:(NSData *)password forService:(NSString *)service account:(NSString *)account {
    return [self setPasswordData:password forService:service account:account error:NULL];
}
+ (BOOL)setPasswordData:(NSData *)password forService:(NSString *)service account:(NSString *)account error:(NSError **)error {
    OSStatus status = FFKeyChainErrorBadArguments;
    if (password && service && account) {
        [self deletePasswordForService:service account:account];
        NSMutableDictionary *query = [self _queryForService:service account:account];
        query[(__bridge id)kSecValueData] = password;
        
        status = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
        
    }
    
    if (status != noErr && error != NULL) {
        *error = [NSError errorWithDomain:FFKeyChainErrorDomain code:status userInfo:nil];
    }
    
    return (status == noErr);
}

#pragma mark - Deleting Passwords

+ (BOOL)deletePasswordForService:(NSString *)service account:(NSString *)account {
    return [self deletePasswordForService:service account:account error:NULL];
}
+ (BOOL)deletePasswordForService:(NSString *)service account:(NSString *)account error:(NSError **)error {
    OSStatus status = FFKeyChainErrorBadArguments;
    if (service && account) {
        NSMutableDictionary *query = [self _queryForService:service account:account];
        status = SecItemDelete((__bridge CFDictionaryRef)query);
    }
    if (status != noErr && error != NULL) {
        *error = [NSError errorWithDomain:FFKeyChainErrorDomain code:status userInfo:nil];
    }
    
    return (status == noErr);
}

#pragma mark - Private

+ (NSMutableDictionary *)_queryForService:(NSString *)service account:(NSString *)account {
    NSMutableDictionary *dictionary = [NSMutableDictionary dictionaryWithCapacity:3];
    dictionary[(__bridge id)kSecClass] = (__bridge id)kSecClassGenericPassword;
    
    if (service) {
        dictionary[(__bridge id)kSecAttrService] = service;
    }
    
    if (account) {
        dictionary[(__bridge id)kSecAttrAccount] = account;
    }
    
    return dictionary;
}

@end
