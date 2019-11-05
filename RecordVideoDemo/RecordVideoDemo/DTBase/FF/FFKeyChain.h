//
//  FFKeyChain.h
//  FFStory
//
//  Created by PageZhang on 14/11/19.
//  Copyright (c) 2014年 PageZhang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Security/Security.h>

/** Error codes that can be returned in NSError objects. */
typedef NS_ENUM(NSInteger, FFKeyChainErrorCode) {
    /** No error. */
    FFKeyChainErrorNone = noErr,
    
    /** Some of the arguments were invalid. */
    FFKeyChainErrorBadArguments = -1001,
    
    /** There was no password. */
    FFKeyChainErrorNoPassword = -1002,
    
    /** One or more parameters passed internally were not valid. */
    FFKeyChainErrorInvalidParameter = errSecParam,
    
    /** Failed to allocate memory. */
    FFKeyChainErrorFailedToAllocated = errSecAllocate,
    
    /** No trust results are available. */
    FFKeyChainErrorNotAvailable = errSecNotAvailable,
    
    /** Authorization/Authentication failed. */
    FFKeyChainErrorAuthorizationFailed = errSecAuthFailed,
    
    /** The item already exists. */
    FFKeyChainErrorDuplicatedItem = errSecDuplicateItem,
    
    /** The item cannot be found.*/
    FFKeyChainErrorNotFound = errSecItemNotFound,
    
    /** Interaction with the Security Server is not allowed. */
    FFKeyChainErrorInteractionNotAllowed = errSecInteractionNotAllowed,
    
    /** Unable to decode the provided data. */
    FFKeyChainErrorFailedToDecode = errSecDecode
};

extern NSString *const FFKeyChainErrorDomain;
extern NSString *const FFKeyChainAccountKey;
extern NSString *const FFKeyChainCreatedAtKey;
extern NSString *const FFKeyChainClassKey;
extern NSString *const FFKeyChainDescriptionKey;
extern NSString *const FFKeyChainLabelKey;
extern NSString *const FFKeyChainLastModifiedKey;
extern NSString *const FFKeyChainWhereKey;


/**
 Simple wrapper for accessing accounts, getting passwords, setting passwords, and deleting passwords using the system
 Keychain on Mac OS X and iOS.
 
 This was originally inspired by EMKeychain and SDKeychain (both of which are now gone). Thanks to the authors.
 */
@interface FFKeyChain : NSObject

/**
 Returns an array containing the Keychain's accounts, or `nil` if the Keychain doesn't have any
 accounts.
 
 See the `NSString` constants declared in TSKeyChain.h for a list of keys that can be used when accessing the
 dictionaries returned by this method.
 
 @param error If accessing the accounts fails, upon return contains an error that describes the problem.
 
 @return An array of dictionaries containing the Keychain's accounts, or `nil` if the Keychain doesn't have any
 accounts. The order of the objects in the array isn't defined.
 
 @see allAccounts
 */
+ (NSArray *)allAccounts;
+ (NSArray *)allAccounts:(NSError **)error;
+ (NSArray *)accountsForService:(NSString *)serviceName;

/**
 Returns a string containing the password for a given accounts and service, or `nil` if the Keychain doesn't have a
 password for the given parameters.
 
 @param serviceName The service for which to return the corresponding password.
 
 @param account The account for which to return the corresponding password.
 
 @param error If accessing the password fails, upon return contains an error that describes the problem.
 
 @return Returns a string containing the password for a given accounts and service, or `nil` if the Keychain doesn't
 have a password for the given parameters.
 
 @see passwordForService:account:
 */
+ (NSString *)passwordForService:(NSString *)serviceName account:(NSString *)account;
+ (NSString *)passwordForService:(NSString *)serviceName account:(NSString *)account error:(NSError **)error;

+ (BOOL)setPassword:(NSString *)password forService:(NSString *)serviceName account:(NSString *)account;
+ (BOOL)setPassword:(NSString *)password forService:(NSString *)serviceName account:(NSString *)account error:(NSError **)error;

+ (NSData *)passwordDataForService:(NSString *)serviceName account:(NSString *)account;
+ (NSData *)passwordDataForService:(NSString *)serviceName account:(NSString *)account error:(NSError **)error;

+ (BOOL)setPasswordData:(NSData *)password forService:(NSString *)serviceName account:(NSString *)account;
+ (BOOL)setPasswordData:(NSData *)password forService:(NSString *)serviceName account:(NSString *)account error:(NSError **)error;

+ (BOOL)deletePasswordForService:(NSString *)serviceName account:(NSString *)account;
+ (BOOL)deletePasswordForService:(NSString *)serviceName account:(NSString *)account error:(NSError **)error;

@end
