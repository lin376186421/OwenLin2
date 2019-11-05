//
//  ViewController.m
//  RelaeaseConfigTool
//
//  Created by 林 on 2017/6/12.
//  Copyright © 2017年 林. All rights reserved.
//

#import "ViewController.h"
#import "NSDate+Utils.h"

@interface ViewController()
{
    IBOutlet NSTextField *pInfoTextView;
    IBOutlet NSTextField *pLogText;
    
    IBOutlet NSTextField *pProjectPathText;
    NSString* pProjectPath;
    NSArray *resultArr;
    NSMutableArray *filesArr;
    
    NSMutableString *loginfo;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    filesArr = [NSMutableArray array];
    loginfo = [NSMutableString string];
  // Do any additional setup after loading the view.
}

- (void)addLog:(NSString *)log
{
    if (loginfo == nil) {
        loginfo = [NSMutableString string];
    }
    
    if (loginfo.length) {
        [loginfo appendString:@" , "];
    }
    
    [loginfo appendString:log];
}

- (IBAction)openPathAction:(id)sender
{
    NSOpenPanel *openDlg = [NSOpenPanel openPanel];
    [openDlg setCanChooseDirectories:YES];
    [openDlg setAllowsMultipleSelection:false];
    
    [openDlg beginSheetModalForWindow:self.view.window completionHandler:^(NSInteger result){
        if (result == NSModalResponseOK) {
            NSString* filePath = [[openDlg URL] path];
            [pProjectPathText setStringValue:filePath];
        }
    }];
}

-(IBAction)downloadAction:(id)sender
{
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"plist"]];
    [saveDlg beginSheetModalForWindow:self.view.window completionHandler:^(NSInteger result){
        if (result == NSModalResponseOK) {
            NSString* saveFilePath = [[saveDlg URL] path];
            NSString *plistPath = [[NSBundle mainBundle] pathForResource:@"release.plist" ofType:nil inDirectory:nil];
            NSFileManager* fileManager = [NSFileManager defaultManager];
            [fileManager copyItemAtPath:plistPath toPath:saveFilePath error:nil];
        }
    }];
}

- (void)checkSourceWithStartStr:(NSString *)startStr endStr:(NSString *)endStr projectPath:(NSString *)path
{
    [filesArr removeAllObjects];
    NSMutableString *proStr = [NSMutableString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    
    NSRange rang1 =  [proStr rangeOfString:startStr options:NSLiteralSearch range:NSMakeRange(0, proStr.length)];
    NSRange rang2 = [proStr rangeOfString:endStr options:NSLiteralSearch range:NSMakeRange(0, proStr.length)];
    NSString *targetStr = [proStr substringWithRange:NSMakeRange(rang1.location+rang1.length, (rang2.location - (rang1.location+rang1.length)))];
    
    NSArray *totalArr = [targetStr componentsSeparatedByString:@"\n"];
    resultArr = totalArr;
    if (resultArr.count) {
        [self searchArrWithStartIndex:0 endEndIndex:resultArr.count];
        NSMutableArray *twoArr = [NSMutableArray array];
        [filesArr enumerateObjectsUsingBlock:^(NSArray *obj, NSUInteger idx, BOOL * _Nonnull stop) {
            if (obj.count > 100) {
                NSMutableArray *arr = [NSMutableArray arrayWithCapacity:obj.count];
                [obj enumerateObjectsUsingBlock:^(NSString *str, NSUInteger idx, BOOL * _Nonnull stop) {
                    NSArray *comArr = [str componentsSeparatedByString:@"*"];
                    if (comArr && comArr.count > 2) {
                        [arr addObject:comArr[1]];
                    } else {
                        //有错误
                        [self addLog:[NSString stringWithFormat:@"文件匹配有误 第%zd行",__LINE__]];
                    }
                }];
                if (arr.count) {
                    [twoArr addObject:arr];
                }
            }
        }];
        
        if (twoArr.count == 2) {
            [self checkFirstArr:twoArr[0] secondArr:twoArr[1]];
        } else {
            //出错
            [self addLog:[NSString stringWithFormat:@"工程文件数有异常 第%zd行",__LINE__]];
        }
    } else {
        //出错
        [self addLog:[NSString stringWithFormat:@"工程文件有异常 第%zd行",__LINE__]];
    }
}

- (void)checkFirstArr:(NSArray*)arr1 secondArr:(NSArray *)arr2
{
    
    NSMutableArray *difArr = [NSMutableArray array];
    [arr1 enumerateObjectsUsingBlock:^(NSString *obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (![arr2 containsObject:obj]) {
            [difArr addObject:obj];
        }
    }];
    
    [arr2 enumerateObjectsUsingBlock:^(NSString *obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (![arr1 containsObject:obj]) {
            [difArr addObject:obj];
        }
    }];
    
    
    if (difArr.count) {
        NSString *diffStr = [difArr componentsJoinedByString:@" *|* "];
        [self addLog:[NSString stringWithFormat:@"文件比对:%@",diffStr]];
    }
}

- (void)searchArrWithStartIndex:(NSUInteger)startIndex endEndIndex:(NSUInteger)endIndex
{
    BOOL ret = NO;
    BOOL findStart = NO;
    for (NSUInteger i = startIndex; i < endIndex; i++) {
        NSString *str = resultArr[i];
        if ([str containsString:@"files ="]) {
            startIndex = i+1;
            findStart = YES;
        }
        
        if (findStart && ([str containsString:@");"])) {
            endIndex = i-1;
            ret = YES;
            break;
        }
    }
    
    if (ret) {
        NSArray *fileArr = [resultArr subarrayWithRange:NSMakeRange(startIndex, endIndex-startIndex+1)];
        [filesArr addObject:fileArr];
        [self searchArrWithStartIndex:endIndex+1 endEndIndex:resultArr.count];
    }
}

- (IBAction)makeAction:(id)sender
{
    
//    pProjectPath = [pProjectPathText stringValue];
//    NSPropertyListFormat format;
//    NSError *errorDesc = nil;
//    NSData *data = [NSData dataWithContentsOfFile:pProjectPath];
//    NSMutableDictionary *obj = (NSMutableDictionary *)[NSPropertyListSerialization propertyListWithData:data options:NSPropertyListMutableContainersAndLeaves format:&format error:&errorDesc];
//    if (obj && [obj isKindOfClass:[NSDictionary class]]) {
//        [obj setValue:@(111) forKey:@"aaaaaa"];
//        NSData *plistData = [NSPropertyListSerialization dataWithPropertyList:obj format:NSPropertyListXMLFormat_v1_0 options:0 error:&errorDesc];
//        
//        [plistData writeToFile:pProjectPath atomically:YES];
//    }
//    [self checkSourceWithStartStr:@"/* Begin PBXResourcesBuildPhase section */" endStr:@"/* End PBXResourcesBuildPhase section */"];
//    [self checkSourceWithStartStr:@"/* Begin PBXSourcesBuildPhase section */" endStr:@"/* End PBXSourcesBuildPhase section */"];
//    [pLogText setStringValue:_diffStr];
//    return;
    
//    NSString* log = @"";

    
    loginfo = nil;
    
    NSString* realseFile = [pInfoTextView stringValue];
    
    NSDictionary* pDict = [NSDictionary dictionaryWithContentsOfFile:realseFile];
    
    pProjectPath = [pProjectPathText stringValue];
    
    if (pProjectPath.length == 0) {
        if ([pDict valueForKey:@"PROJECT_PATH"]) {
            pProjectPath = [pDict valueForKey:@"PROJECT_PATH"];
        }
    }
    
    
    [self replaceGlobalKey:pDict];
    [self replaceUserPath:pDict];
    
    
    
    
    //CFBundleShortVersionString
    //info plist ios
    NSString* infoFile = [pDict objectForKey:@"INFO_PLIST"];
    if (infoFile) {
        NSDictionary* pInfoDict = [NSDictionary dictionaryWithContentsOfFile:infoFile];
        NSDictionary* _infoDict = [pDict objectForKey:@"InfoPlist"];
        for (NSString *key in [_infoDict keyEnumerator]) {
            NSObject* value = [_infoDict objectForKey:key];
            [pInfoDict setValue:value forKey:key];
        }
        [pInfoDict writeToFile:infoFile atomically:false];
    }
    
    //app define
    NSString* defineFile = [pDict objectForKey:@"APP_DEFINE"];
    NSMutableString* pDefine = [NSMutableString stringWithContentsOfFile:defineFile encoding:NSUTF8StringEncoding error:nil];
    NSDictionary* pDefineDict = [pDict objectForKey:@"DefineKeys"];
    //快速枚举遍历所有KEY的值
    for (NSString *key in [pDefineDict keyEnumerator]) {
        
        NSString* value = [pDefineDict valueForKey:key];
        
        if ([value isKindOfClass:[NSString class]]) {
            [self replaceConfigValue:pDefine key:key value:(NSString*)value hasEqual:false];
        }
        else if([value isKindOfClass:[NSDictionary class]])
        {
            //目前未涉及到该功能
        }
    }
    [pDefine writeToFile:defineFile atomically:false encoding:NSUTF8StringEncoding error:nil];

    //app config
    NSString* configFile = [pDict objectForKey:@"APP_CONFIGS"];
    NSMutableString* pConfig = [NSMutableString stringWithContentsOfFile:configFile encoding:NSUTF8StringEncoding error:nil];
    NSDictionary* pConfigDict = [pDict objectForKey:@"ConfigKeys"];
    //快速枚举遍历所有KEY的值
    for (NSString *key in [pConfigDict keyEnumerator]) {
        NSString* value = [pConfigDict valueForKey:key];
        if ([value isKindOfClass:[NSString class]]) {
            if ([key isEqualToString:@"APP_PRODUCTION_VERIFY_DATE"]) {
                NSUInteger day = [value integerValue];
                NSDate *date = [NSDate datePlus:(day * 24 * 3600)];
                NSString *dateStr = [date stringValue:yyyyMMdd];
                [self replaceConfigValue:pConfig key:key value:dateStr hasEqual:false];
            }else {
                [self replaceConfigValue:pConfig key:key value:(NSString*)value hasEqual:false];
            }
        } else if ([value isKindOfClass:[NSNumber class]]) {
            BOOL isVaild = [value boolValue];
            [self repleaseRange:pConfig key:key isVaild:isVaild];
        } else if([value isKindOfClass:[NSDictionary class]])
        {
            //目前未涉及到该功能
        }
        
    }
    [pConfig writeToFile:configFile atomically:false encoding:NSUTF8StringEncoding error:nil];
    
    //xcode project
    NSString* projectFile = [pDict objectForKey:@"XCODE_PROJECT"];
    
    [self checkSourceWithStartStr:@"/* Begin PBXResourcesBuildPhase section */" endStr:@"/* End PBXResourcesBuildPhase section */" projectPath:projectFile];
    [self checkSourceWithStartStr:@"/* Begin PBXSourcesBuildPhase section */" endStr:@"/* End PBXSourcesBuildPhase section */" projectPath:projectFile];
    
    NSMutableString* projectConfig = [NSMutableString stringWithContentsOfFile:projectFile encoding:NSUTF8StringEncoding error:nil];
    NSRange configRange = NSMakeRange(0, projectConfig.length);
    NSDictionary* pProjectDict = [pDict objectForKey:@"XcodeKeys"];
    for (NSString *key in [pProjectDict keyEnumerator]) {
        
        NSString* value = [pProjectDict valueForKey:key];
        
        //目前只处理了 设备选择
        if ([value isKindOfClass:[NSString class]] && [key isEqualTo:@"TARGET_DEVICE"]) {
            NSString* deviceFaimily = @"1";
            
            if ([[value lowercaseString] isEqualToString:@"iphone"]) {
                deviceFaimily = @"1";
            }
            else if ([[value lowercaseString] isEqualToString:@"ipad"]) {
                deviceFaimily = @"2";
            }else{
                deviceFaimily = @"1,2";
            }
            NSRange range = NSMakeRange(0, configRange.length);
            while (range.length > 0)
            {
                range = [projectConfig rangeOfString:@"TARGETED_DEVICE_FAMILY" options:NSLiteralSearch range:range];
                
                if (range.length > 0) {
                    [self replaceConfigValue:projectConfig key:@"TARGETED_DEVICE_FAMILY" value:deviceFaimily range:NSMakeRange(range.location, projectConfig.length-range.location) hasEqual:true needQuotation:YES];
                    range = NSMakeRange(range.location+range.length, projectConfig.length-(range.location+range.length));
                }else{
                    break;
                }
            }
            
            [projectConfig writeToFile:projectFile atomically:false encoding:NSUTF8StringEncoding error:nil];
        }
    }
    
    
    //copy file
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    NSArray* pFileArrays = [pDict objectForKey:@"Files"];
    for (int i = 0; i < pFileArrays.count; i++) {
        
        NSString* pCopyFile = [pFileArrays objectAtIndex:i];
        
        NSArray* pFilePaths = [pCopyFile componentsSeparatedByString:@","];
        
        NSString* path = [pFilePaths objectAtIndex:0];
        NSString* targetPath = [pFilePaths objectAtIndex:1];
        
        if ([fileManager contentsOfDirectoryAtPath:path error:nil].count <= 0) { //move file
            
//            NSString* copyPath = targetPath;
//            if ([fileManager contentsOfDirectoryAtPath:targetPath error:nil].count > 0) {
//                NSString* fileName = [path lastPathComponent];
//                copyPath = [targetPath stringByAppendingPathComponent:fileName];
//            }
//            
//            [fileManager removeItemAtPath:copyPath error:nil];
//            
//            bool result = [fileManager copyItemAtPath:path toPath:copyPath error:nil];
//            if (result == false) {
//                log = [log stringByAppendingFormat:@"path -> %@ error!\n", path];
//                NSLog(@"%@",log);
//            }
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
                        [self addLog:[NSString stringWithFormat:@"path -> %@ error!\n 第%zd行",filePath,__LINE__]];
                    }
                }
                
            }
            
            if (files.count <= 0) {
               [self addLog:[NSString stringWithFormat:@"path -> %@ error!\n 第%zd行",path,__LINE__]];
            }
        }
    }
    
    //匹配文件数
    
    
    if (loginfo.length > 0) {
        [pLogText setStringValue:loginfo];
    }else{
        [pLogText setStringValue:@"Make Successed!"];
    }
    
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
                [self replaceConfigValue:pConfig key:key value:(NSString*)value range:condRange hasEqual:false needQuotation:NO];
                break;
            }
        }
        
        range = [pConfig rangeOfString:condKey options:NSLiteralSearch range:NSMakeRange(range.location+range.length, pConfig.length-(range.location+range.length))];
    }
    
}

-(void)replaceConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value hasEqual:(BOOL)hasEqual
{
    [self replaceConfigValue:pConfig key:key value:value range:NSMakeRange(0, pConfig.length) hasEqual:hasEqual needQuotation:NO];
}

-(void)replaceConfigValue:(NSMutableString *)pConfig key:(NSString *)key value:(NSString *)value range:(NSRange)range hasEqual:(BOOL)hasEqual needQuotation:(BOOL)needQuotation
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
        
        if (needQuotation) {
            //即将替换的字符串没有引号 强制加引号
            if ([pConfig characterAtIndex:start-1] != '\"' ) {
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

- (void)repleaseRange:(NSMutableString *)pConfig key:(NSString *)key isVaild:(BOOL)isVaild
{
    NSRange allRange = NSMakeRange(0, pConfig.length);
    NSRange range = [pConfig rangeOfString:key options:NSLiteralSearch range:allRange];
    if (range.length > 0) {
        
        NSInteger startIndex = range.location;
        while (startIndex > 0) {
            NSRange range1 = [pConfig rangeOfString:@"\n" options:NSLiteralSearch range:NSMakeRange(startIndex, 1)];
            if (range1.length) {
                startIndex = range1.location+range1.length;
                break;
            }
            startIndex -=1;
        }
        
        NSRange defineRange = [pConfig rangeOfString:@"#define" options:NSLiteralSearch range:NSMakeRange(startIndex, range.location-startIndex)];
        if (startIndex == defineRange.length) {
            //说明第二行一开始就是#define 也就是说没有//
        } else if (startIndex > defineRange.location) {
            [self addLog:[NSString stringWithFormat:@"检查代码 第%zd行",__LINE__]];
        } else {
            [pConfig replaceCharactersInRange:NSMakeRange(startIndex, defineRange.location-startIndex) withString:@""];
        }
        
        NSInteger endIndex = startIndex;
        while (endIndex < pConfig.length) {
            NSRange range2 = [pConfig rangeOfString:@"\n" options:NSLiteralSearch range:NSMakeRange(endIndex, 1)];
            if (range2.length) {
                endIndex = range2.location;
                break;
            }
            endIndex +=1;
        }
        
        NSRange curLineRange = NSMakeRange(startIndex, endIndex-startIndex);
        if (!isVaild) {
            [pConfig replaceOccurrencesOfString:@"#define" withString:@"//#define" options:NSLiteralSearch range:curLineRange];
        } else {
            //无需处理
        }
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
            else if ([pValue isKindOfClass:[NSString class]]) {
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
            else if ([pValue isKindOfClass:[NSString class]]){
                if (pProjectPath.length > 0) {
                    NSString* pPath = [(NSString*)pValue stringByReplacingOccurrencesOfString:@"$PROJECT_PATH" withString:pProjectPath];
                    
                    [pArray replaceObjectAtIndex:i withObject:pPath];
                }
            }
        }
    }
}


- (void)replaceUserPath:(NSObject *)pObject
{
    NSString *userDeskPath = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES)[0];
    NSString *userPath = [userDeskPath stringByDeletingLastPathComponent];
    
    if ([pObject isKindOfClass:[NSDictionary class]]) {
        NSDictionary* pDict = (NSDictionary*)pObject;
        for (NSString* pKey in [pDict allKeys]) {
            NSObject* pValue = [pDict objectForKey:pKey];
            if ([pValue isKindOfClass:[NSDictionary class]]) {
                [self replaceUserPath:(NSDictionary*)pValue];
            }
            else if ([pValue isKindOfClass:[NSArray class]]){
                [self replaceUserPath:(NSArray*)pValue];
            }
            else if ([pValue isKindOfClass:[NSString class]]) {
                if (userPath.length > 0) {
                    pValue = [(NSString*)pValue stringByReplacingOccurrencesOfString:@"~" withString:userPath];
                    [pDict setValue:pValue forKey:pKey];
                }
            }
        }
    }else if([pObject isKindOfClass:[NSArray class]]){
        NSMutableArray* pArray = (NSMutableArray*)pObject;
        for (int i = 0; i < pArray.count; i++) {
            NSObject* pValue = [pArray objectAtIndex:i];
            
            if ([pValue isKindOfClass:[NSDictionary class]]) {
                [self replaceUserPath:(NSDictionary*)pValue];
            }
            else if ([pValue isKindOfClass:[NSArray class]]){
                [self replaceUserPath:(NSArray*)pValue];
            }
            else if ([pValue isKindOfClass:[NSString class]]){
                if (userPath.length > 0) {
                    NSString* pPath = [(NSString*)pValue stringByReplacingOccurrencesOfString:@"~" withString:userPath];
                    [pArray replaceObjectAtIndex:i withObject:pPath];
                }
            }
        }
    }
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
