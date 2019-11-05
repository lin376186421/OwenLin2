//
//  CCLocalManager.cpp
//  RobinHood
//
//  Created by qsun on 13-1-5.
//
//

#include "CLocalManager.h"

#import <Foundation/Foundation.h>

static CLocalManager* s_pSharedLocalManager = NULL;

CLocalManager* CLocalManager::sharedLocalManager()
{
    if (s_pSharedLocalManager == NULL) {
        s_pSharedLocalManager = new CLocalManager();
        s_pSharedLocalManager->init();
    }
    return s_pSharedLocalManager;
}

void CLocalManager::init()
{
    NSString *directoryPath = [NSString stringWithUTF8String:getDirectoryPath()];
    BOOL isDirectory = NO;
    BOOL folderExists = [[NSFileManager defaultManager] fileExistsAtPath:directoryPath isDirectory:&isDirectory] && isDirectory;
    
    if (!folderExists){
        NSError *error = nil;
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath withIntermediateDirectories:YES attributes:nil error:&error];
        [error release];
    }

}

const char* CLocalManager::getDirectoryPath()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *cacheDirectoryName = [documentsDirectory stringByAppendingPathComponent:@"cache"];
    return [cacheDirectoryName UTF8String];
}

const char* CLocalManager::getFileFullPath(const char* fileName)
{
    NSString *directoryPath = [NSString stringWithUTF8String:getDirectoryPath()];
    NSString *filePath = [directoryPath stringByAppendingPathComponent:[NSString stringWithUTF8String:fileName]];
    return [filePath UTF8String];
}

bool CLocalManager::isFileExist(const char* fileName)
{
    NSString *filePath = [NSString stringWithUTF8String:getFileFullPath(fileName)];
    return [[NSFileManager defaultManager] fileExistsAtPath:filePath];
}
