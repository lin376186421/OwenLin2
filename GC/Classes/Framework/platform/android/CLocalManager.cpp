//
//  CCLocalManager.cpp
//  RobinHood
//
//  Created by qsun on 13-1-5.
//
//

#include "CLocalManager.h"
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include "cocos2d.h"

using namespace cocos2d;

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
	const char* directoryPath = getDirectoryPath();

	if(opendir(directoryPath) == NULL){

		int result = mkdir(directoryPath, 0755);
		CCLog("mkdir result: %i", result);
	}
}

const char* CLocalManager::getDirectoryPath()
{
	string cachePath = "";//CCFileUtils::sharedFileUtils()->getWriteablePath().append("cache/");

	return cachePath.c_str();
}

const char* CLocalManager::getFileFullPath(const char* fileName)
{
	string directoryPath = getDirectoryPath();
	string filePath = directoryPath.append(fileName);

	return filePath.c_str();
}

bool CLocalManager::isFileExist(const char* fileName)
{
	const char* filePath = getFileFullPath(fileName);

	fstream fs(filePath);

	if(fs){
		return true;
	}

	return false;
}
