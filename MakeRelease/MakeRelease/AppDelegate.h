//
//  AppDelegate.h
//  MakeRelease
//
//  Created by qsun on 13-8-30.
//  Copyright (c) 2013年 qsun. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTextFieldDelegate>
{
    IBOutlet NSTextField *pInfoTextView;
    IBOutlet NSTextField *pLogText;
    
    IBOutlet NSTextField *pProjectPathText;
    
    NSString* pProjectPath;
}
@property (assign) IBOutlet NSWindow *window;

@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;
@property (readonly, strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (readonly, strong, nonatomic) NSManagedObjectContext *managedObjectContext;

- (IBAction)makeAction:(id)sender;
- (IBAction)openPathAction:(id)sender;
- (IBAction)downloadAction:(id)sender;

- (void)replaceConfigValue:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value hasEqual:(BOOL)hasEqual;
- (void)replaceConfigValue:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value range:(NSRange)range hasEqual:(BOOL)hasEqual;

- (void)replaceValueForCondition:(NSMutableString*) pConfig condKey:(NSString*) condKey condValue:(NSString*) condValue key:(NSString*) key value:(NSString*) value;

- (BOOL)checkConfigValue:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value hasEqual:(BOOL)hasEqual;
- (BOOL)checkConfigValue:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value range:(NSRange)range hasEqual:(BOOL)hasEqual;

- (NSRange) getConditionRange:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value;
- (NSRange) getConditionRange:(NSMutableString*) pConfig key:(NSString*) key value:(NSString*) value range:(NSRange)range;

- (void) replaceGlobalKey:(NSObject*) pObject;

@end
