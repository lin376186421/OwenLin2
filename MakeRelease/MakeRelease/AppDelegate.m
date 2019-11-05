//
//  AppDelegate.m
//  MakeRelease
//
//  Created by qsun on 13-8-30.
//  Copyright (c) 2013年 qsun. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

-(void)downloadAction:(id)sender
{
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"plist"]];
    [saveDlg beginSheetModalForWindow:_window completionHandler:^(NSInteger result){
        if (result == NSOKButton)
        {
            NSString* saveFilePath = [[saveDlg URL] path];
            
            NSString *plistPath = [[NSBundle mainBundle] pathForResource:@"release.plist" ofType:nil inDirectory:nil];
            
            NSFileManager* fileManager = [NSFileManager defaultManager];
           [fileManager copyItemAtPath:plistPath toPath:saveFilePath error:nil];
        }
    }];
}

// Performs the save action for the application, which is to send the save: message to the application's managed object context. Any encountered errors are presented to the user.
- (IBAction)makeAction:(id)sender
{
    NSString* log = @"";
    
    NSString* realseFile = [pInfoTextView stringValue];
    
    NSDictionary* pDict = [NSDictionary dictionaryWithContentsOfFile:realseFile];
   
    pProjectPath = [pProjectPathText stringValue];
    
    if (pProjectPath.length == 0) {
        if ([pDict valueForKey:@"PROJECT_PATH"]) {
            pProjectPath = [pDict valueForKey:@"PROJECT_PATH"];
        }
    }
    
    [self replaceGlobalKey:pDict];
    
    //info plist ios
    NSString* infoFile = [pDict objectForKey:@"INFO_PLIST"];
    if (infoFile) {
        
        NSDictionary* pInfoDict = [NSDictionary dictionaryWithContentsOfFile:infoFile];
        
        NSDictionary* _infoDict = [pDict objectForKey:@"InfoPlist"];
        
        //修改
        for (NSString *key in [_infoDict keyEnumerator]) {
            NSLog(@"%@",key);
            if([key isEqualToString:@"FacebookAppID"])
            {
                NSArray *URL_Array = (NSArray *)[pInfoDict objectForKey:@"CFBundleURLTypes"];
                NSDictionary *item0 = URL_Array[0];
                NSMutableArray *SchemesArray = (NSMutableArray *)[item0 objectForKey:@"CFBundleURLSchemes"];
                NSString *fbItem0 = [NSString stringWithFormat:@"fb%@",[_infoDict objectForKey:key]];
                [SchemesArray replaceObjectAtIndex:0 withObject:fbItem0];
            }
            NSObject* value = [_infoDict objectForKey:key];
                [pInfoDict setValue:value forKey:key];
        }
        
        [pInfoDict writeToFile:infoFile atomically:false];
    }
    
    //Android
    //values string.xml
    NSString* stringFile = [pDict objectForKey:@"VALUES_STRING"];//values_string
    if (stringFile) {
        NSData *xmlData = [[NSMutableData alloc] initWithContentsOfFile:stringFile];
        NSError *error;
        NSXMLDocument *xmlDoc = [[NSXMLDocument alloc] initWithData:xmlData options:0 error:&error];
        NSXMLElement* rootElement = [xmlDoc rootElement];
        
        NSDictionary* _stringDict = [pDict objectForKey:@"ValuesString"];
        
        NSArray* pElements = [rootElement elementsForName:@"string"];
        
        for (NSString *key in [_stringDict keyEnumerator]) {
            NSString* value = [_stringDict objectForKey:key];
            
            [[rootElement attributeForName:key] setStringValue:(NSString*)value];
            
            for (int index = 0; index < pElements.count; index++) {
                NSXMLElement* xmlElement = [pElements objectAtIndex:index];
                
                NSXMLNode* node = [xmlElement attributeForName:@"name"];
                if ([node.objectValue isEqualToString:key]) {
                    [xmlElement setStringValue:value];
                }
            }
        }
        
        xmlData = [xmlDoc XMLDataWithOptions:NSXMLNodePrettyPrint];
        [xmlData writeToFile:stringFile atomically:false];
    }
    
//    [doc release];

    //app config
    NSString* configFile = [pDict objectForKey:@"AppConfig"];
    NSMutableString* pConfig = [NSMutableString stringWithContentsOfFile:configFile encoding:NSUTF8StringEncoding error:nil];
    
    NSDictionary* pConfigDict = [pDict objectForKey:@"ConfigKeys"];
    
    //快速枚举遍历所有KEY的值
    for (NSString *key in [pConfigDict keyEnumerator]) {

        NSObject* value = [pConfigDict valueForKey:key];
        
        if ([value isKindOfClass:[NSString class]]) {
            [self replaceConfigValue:pConfig key:key value:(NSString*)value hasEqual:false];
        }
        else if([value isKindOfClass:[NSDictionary class]])
        {
            NSDictionary* pTargetDict = (NSDictionary*)value;
            
            NSString* condValue = [pConfigDict valueForKey:@"PLATFORM_TARGET"];
            
            for (NSString *subKey in [pTargetDict keyEnumerator])
            {
                if ([key isEqualToString:@"TARGET_IPHONE"]) {
                    NSString* subValue = [pTargetDict valueForKey:subKey];
                    [self replaceValueForCondition:pConfig condKey:@"PLATFORM_TARGET" condValue:condValue key:subKey value:subValue];
                }else if ([key isEqualToString:@"TARGET_IPAD"]) {
                    NSString* subValue = [pTargetDict valueForKey:subKey];
                    [self replaceValueForCondition:pConfig condKey:@"PLATFORM_TARGET" condValue:condValue key:subKey value:subValue];
                }else if ([key isEqualToString:@"TARGET_ANDROID"]) {
                    NSString* subValue = [pTargetDict valueForKey:subKey];
                    [self replaceValueForCondition:pConfig condKey:@"PLATFORM_TARGET" condValue:condValue key:subKey value:subValue];
                }
            }
        }
        
    }
    
    [pConfig writeToFile:configFile atomically:false encoding:NSUTF8StringEncoding error:nil];
    
    //xcode project
    NSString* projectFile = [pDict objectForKey:@"XCODE_PROJECT"];
    NSMutableString* projectConfig = [NSMutableString stringWithContentsOfFile:projectFile encoding:NSUTF8StringEncoding error:nil];
    NSRange configRange = NSMakeRange(0, projectConfig.length);
    
    NSString* deviceFaimily = @"1";
    
    if ([[[pDict objectForKey:@"TARGET_DEVICE"] lowercaseString] isEqualToString:@"iphone"]) {
        deviceFaimily = @"1";
    }
    else if ([[[pDict objectForKey:@"TARGET_DEVICE"] lowercaseString] isEqualToString:@"ipad"]) {
        deviceFaimily = @"2";
    }else{
        deviceFaimily = @"1,2";
    }
    
    NSRange range = NSMakeRange(0, configRange.length);
    while (range.length > 0)
    {
        range = [projectConfig rangeOfString:@"TARGETED_DEVICE_FAMILY" options:NSLiteralSearch range:range];
        
        if (range.length > 0) {
            [self replaceConfigValue:projectConfig key:@"TARGETED_DEVICE_FAMILY" value:deviceFaimily range:NSMakeRange(range.location, projectConfig.length-range.location) hasEqual:true];
            range = NSMakeRange(range.location+range.length, projectConfig.length-(range.location+range.length));
        }else{
            break;
        }
    }
    
    [projectConfig writeToFile:projectFile atomically:false encoding:NSUTF8StringEncoding error:nil];
    
    //copy file
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    NSArray* pFileArrays = [pDict objectForKey:@"Files"];
    for (int i = 0; i < pFileArrays.count; i++) {
        
        NSString* pCopyFile = [pFileArrays objectAtIndex:i];
        
        NSArray* pFilePaths = [pCopyFile componentsSeparatedByString:@","];
        
        NSString* path = [pFilePaths objectAtIndex:0];
        NSString* targetPath = [pFilePaths objectAtIndex:1];
        
        if ([fileManager contentsOfDirectoryAtPath:path error:nil].count <= 0) { //move file
            
            NSString* copyPath = targetPath;
            if ([fileManager contentsOfDirectoryAtPath:targetPath error:nil].count > 0) {
                NSString* fileName = [path lastPathComponent];
                copyPath = [targetPath stringByAppendingPathComponent:fileName];
            }
            
            [fileManager removeItemAtPath:copyPath error:nil];
            
            bool result = [fileManager copyItemAtPath:path toPath:copyPath error:nil];
            if (result == false) {
                log = [log stringByAppendingFormat:@"path -> %@ error!\n", path];
                NSLog(@"%@",log);
            }
        }else{ //move Directory
            NSArray* files = [fileManager contentsOfDirectoryAtPath:path error:nil];
            for (int index = 0; index < [files count]; index++) {
                NSString* filePath = [NSString stringWithFormat:@"%@/%@",path, [files objectAtIndex:index]];
                NSString* copyPath = [NSString stringWithFormat:@"%@/%@",targetPath, [files objectAtIndex:index]];
                
                NSRange range = [filePath rangeOfString:@".DS_Store"];
                
                if (range.length == 0) { //can read
                    [fileManager removeItemAtPath:copyPath error:nil];
                    
                    bool result = [fileManager copyItemAtPath:filePath toPath:copyPath error:nil];
                    if (result == false) {
                        log = [log stringByAppendingFormat:@"path -> %@ error!\n", filePath];
                        NSLog(@"%@",log);
                    }
                }
                
            }
            
            if (files.count <= 0) {
                log = [log stringByAppendingFormat:@"path -> %@ error!\n", path];
            }
        }
    }
    
    if (log.length > 0) {
        log = log = [log stringByAppendingString:@"Make End!"];
        [pLogText setStringValue:log];
    }else{
        [pLogText setStringValue:@"Make Successed!"];
    }

}

- (IBAction)openPathAction:(id)sender
{
    NSOpenPanel *openDlg = [NSOpenPanel openPanel];
    [openDlg setCanChooseDirectories:YES];
    [openDlg setAllowsMultipleSelection:false];
    
    [openDlg beginSheetModalForWindow:_window completionHandler:^(NSInteger result){
        if (result == NSOKButton)
        {
            NSString* filePath = [[openDlg URL] path];
            
            [pProjectPathText setStringValue:filePath];
        }
    }];

}

-(void)controlTextDidChange:(NSNotification*)notification
{
    [pLogText setStringValue:@" "];
}

-(void)textDidChange
{
    
}

- (void)replaceValueForCondition:(NSMutableString*) pConfig condKey:(NSString *)condKey condValue:(NSString *)condValue key:(NSString *)key value:(NSString *)value
{
    NSRange range = [pConfig rangeOfString:condKey];
    while (range.length > 0)
    {
        NSRange condRange = [self getConditionRange:pConfig key:condKey value:condValue range:NSMakeRange(range.location, pConfig.length - range.location)];
        if (condRange.length > 0) {
            if ([pConfig rangeOfString:key options:NSLiteralSearch range:condRange].length > 0) {
                [self replaceConfigValue:pConfig key:key value:(NSString*)value range:condRange hasEqual:false];
                break;
            }
        }
        
        range = [pConfig rangeOfString:condKey options:NSLiteralSearch range:NSMakeRange(range.location+range.length, pConfig.length-(range.location+range.length))];
    }

}

-(void)replaceConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value hasEqual:(BOOL)hasEqual
{
    [self replaceConfigValue:pConfig key:key value:value range:NSMakeRange(0, pConfig.length) hasEqual:hasEqual];
}

-(void)replaceConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value range:(NSRange)range hasEqual:(BOOL)hasEqual
{

    //match key
    NSString* origKey = [NSString stringWithString:key];
    key = [NSString stringWithFormat:@" %@ ", origKey];
    NSRange checkRange = NSMakeRange(range.location-1, range.length+1);
    if (range.location == 0) checkRange = range;
    
    NSRange keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:checkRange];
    
    if (keyRange.length <= 0) {
        key = [NSString stringWithFormat:@"\t%@ ", origKey];
        NSRange checkRange = NSMakeRange(range.location-1, range.length+1);
        if (range.location == 0) checkRange = range;
        keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:checkRange];
    }
    
    if (keyRange.length <= 0 && hasEqual) {
        key = [NSString stringWithFormat:@" %@=", origKey];
        NSRange checkRange = NSMakeRange(range.location-1, range.length+1);
        if (range.location == 0) checkRange = range;
        keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:checkRange];
    }
    
    if (keyRange.length <= 0 && hasEqual) {
        key = [NSString stringWithFormat:@"\t%@=", origKey];
        NSRange checkRange = NSMakeRange(range.location-1, range.length+1);
        if (range.location == 0) checkRange = range;
        keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:checkRange];
    }
    
    if (keyRange.length > 0) {
  
        int index = (int)(keyRange.location + keyRange.length) - 1;
        
        int start = 0;
        int len = 0;
        while (index < [pConfig length] && [pConfig characterAtIndex:index] == ' ') {
            index++;
        }
        
        if ([pConfig characterAtIndex:index] == '@') { //Ignore object-c NSString '@'
            index++;
        }
        
        if ([pConfig characterAtIndex:index] == '\"') {
            index++;
        }
        
        if (hasEqual) {
            while (index < [pConfig length] && [pConfig characterAtIndex:index] != '=') {
                index++;
            }
            index++;
            while (index < [pConfig length] && [pConfig characterAtIndex:index] == ' ') {
                index++;
            }
            if ([pConfig characterAtIndex:index] == '\"') {
                index++;
            }
        }
        
        start = index;
        
        while (index < [pConfig length] && [pConfig characterAtIndex:index] != ' ') {
            
            if ([pConfig characterAtIndex:index] == '\\' && [pConfig characterAtIndex:index+1] == '\\') {
                break;
            }
            
            if ([pConfig characterAtIndex:index] == '\"' || [pConfig characterAtIndex:index] == ';'
                || [pConfig characterAtIndex:index] == '\n') {
                break;
            }
            
            if ([pConfig characterAtIndex:index] == '\\') {
                index++;
            }
            
            index++;
            len ++;
        }
    
        if (len == 0) {
            len = 1;
            if ([pConfig characterAtIndex:index-1] == '\"' && [pConfig characterAtIndex:index] == '\"') {
                start -= 1;
                len ++;
                value = [NSString stringWithFormat:@"\"%@\"", value];
            }
        }
        
        NSString *replaceValue = [pConfig substringWithRange:NSMakeRange(start, len)];
        
        [pConfig replaceOccurrencesOfString:replaceValue withString:value options:NSLiteralSearch range:NSMakeRange(start, len)];
    }
}

-(BOOL) checkConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value hasEqual:(BOOL)hasEqual
{
    return [self checkConfigValue:pConfig key:key value:value range:NSMakeRange(0, pConfig.length) hasEqual:hasEqual];
}

-(BOOL) checkConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value range:(NSRange)range hasEqual:(BOOL)hasEqual
{
    //match key
    NSString* origKey = [NSString stringWithString:key];
    key = [NSString stringWithFormat:@"%@ ", origKey];
  
    NSRange keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:range];
    if (keyRange.length <= 0 && hasEqual) {
        key = [NSString stringWithFormat:@"%@=", origKey];
        keyRange = [pConfig rangeOfString:key options:NSLiteralSearch range:range];
    }
    
    if (keyRange.length > 0) {
        
        int index = (int)(keyRange.location + keyRange.length) - 1;
        
        int start = 0;
        int len = 0;
        while (index < [pConfig length] && [pConfig characterAtIndex:index] == ' ') {
            index++;
        }
        
        if ([pConfig characterAtIndex:index] == '@') { //Ignore object-c NSString '@'
            index++;
        }
        
        if ([pConfig characterAtIndex:index] == '\"') {
            index++;
        }
        
        if (hasEqual) {
            while (index < [pConfig length] && [pConfig characterAtIndex:index] == ' ') {
                index++;
            }
            if ([pConfig characterAtIndex:index] != '=') {
                return false;
            }
            index++;
            if ([pConfig characterAtIndex:index] == '=') { //==
                index++;
            }
            index++;
            while (index < [pConfig length] && [pConfig characterAtIndex:index] == ' ') {
                index++;
            }
            if ([pConfig characterAtIndex:index] == '\"') {
                index++;
            }
        }
        
        start = index;
        
        while (index < [pConfig length] && [pConfig characterAtIndex:index] != ' ') {
            
            if ([pConfig characterAtIndex:index] == '\\' && [pConfig characterAtIndex:index+1] == '\\') {
                break;
            }
            
            if ([pConfig characterAtIndex:index] == '\"' || [pConfig characterAtIndex:index] == ';' ||
                [pConfig characterAtIndex:index] == ')') {
                break;
            }
            
            if ([pConfig characterAtIndex:index] == '\n') {
                break;
            }
            
            if ([pConfig characterAtIndex:index] == '\\') {
                index++;
            }
            
            index++;
            len ++;
        }
        
        if (len == 0) {
            len = 1;
        }
        
        NSString *replaceValue = [pConfig substringWithRange:NSMakeRange(start, len)];
        
        if ([replaceValue isEqualToString:value]) {
            return true;
        }
        return false;
    }

    return false;
}

-(NSRange) getConditionRange:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value
{
    return [self getConditionRange:pConfig key:key value:value range:NSMakeRange(0, pConfig.length)];
}

-(NSRange) getConditionRange:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value range:(NSRange)range
{
    if ([self checkConfigValue:pConfig key:key value:value range:range hasEqual:TRUE]) {
        NSRange range1 = [pConfig rangeOfString:key options:NSLiteralSearch range:range];
        NSRange rangeElse = [pConfig rangeOfString:@"#else" options:NSLiteralSearch range:NSMakeRange(range1.location+range1.length, pConfig.length-(range1.location+range1.length))];
        NSRange rangeElif = [pConfig rangeOfString:@"#elif" options:NSLiteralSearch range:NSMakeRange(range1.location+range1.length, pConfig.length-(range1.location+range1.length))];
        NSRange rangeEndif = [pConfig rangeOfString:@"#endif" options:NSLiteralSearch range:NSMakeRange(range1.location+range1.length, pConfig.length-(range1.location+range1.length))];
        
        long endLocation = rangeElse.location;
        if (rangeElif.location < endLocation) {
            endLocation = rangeElif.location;
        }
        if (rangeEndif.location < endLocation) {
            endLocation = rangeEndif.location;
        }
        
        NSRange condRange = NSMakeRange(range1.location, endLocation-range1.location);
        
        return condRange;
    }else{
        return NSMakeRange(0, 0);
    }
    
}

-(void) replaceGlobalKey:(NSObject *)pObject
{
    if ([pObject isKindOfClass:[NSDictionary class]]) {
        NSDictionary* pDict = (NSDictionary*)pObject;
        for (NSString* pKey in [pDict allKeys]) {
            NSObject* pValue = [pDict objectForKey:pKey];
            if ([pValue isKindOfClass:[NSDictionary class]]) {
                [self replaceGlobalKey:(NSDictionary*)pValue];
            }
            else if ([pValue isKindOfClass:[NSArray class]]){
                [self replaceGlobalKey:(NSArray*)pValue];
            }
            else{
                if (pProjectPath.length > 0) {
                    pValue = [(NSString*)pValue stringByReplacingOccurrencesOfString:@"$PROJECT_PATH" withString:pProjectPath];
                    [pDict setValue:pValue forKey:pKey];
                }
            }
        }
    }else if([pObject isKindOfClass:[NSArray class]]){
        NSMutableArray* pArray = (NSMutableArray*)pObject;
        for (int i = 0; i < pArray.count; i++) {
            NSObject* pValue = [pArray objectAtIndex:i];
            
            if ([pValue isKindOfClass:[NSDictionary class]]) {
                [self replaceGlobalKey:(NSDictionary*)pValue];
            }
            else if ([pValue isKindOfClass:[NSArray class]]){
                [self replaceGlobalKey:(NSArray*)pValue];
            }
            else{
                if (pProjectPath.length > 0) {
                    NSString* pPath = [(NSString*)pValue stringByReplacingOccurrencesOfString:@"$PROJECT_PATH" withString:pProjectPath];
                    
                    [pArray replaceObjectAtIndex:i withObject:pPath];
                }
            }
        }
    }
}

@end
