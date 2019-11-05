//
//  CCLocalManager.h
//  RobinHood
//
//  Created by qsun on 13-1-5.
//
//

#ifndef __Utils__CLocalManager__
#define __Utils__CLocalManager__

#include <iostream>
#include <fstream>

using namespace std;

class CLocalManager {
public:
    
    static CLocalManager* sharedLocalManager();
    
    void init();
    
    const char* getDirectoryPath();
    const char* getFileFullPath(const char* fileName);
    
    bool isFileExist(const char* fileName);
    
    //save data
    bool saveData(const char* fileName, const char* pData, int size){
        const char* filePath = getFileFullPath(fileName);
        
        fstream pFile;
        pFile.open(filePath, ios::out|ios::binary);

        if(pFile.is_open()){
            pFile.write(pData, size);

            pFile.flush();
            pFile.close();

            return true;
        }
        
        return false;
    }
    
    int getFileData(char** pData, const char* path){
        ifstream file(path,ios::binary);
        file.seekg(0, ios::end);
        int FileLength = file.tellg();
        char* str = (char *)malloc((FileLength+1)*sizeof(char));
        memset(str,0,(FileLength+1)*sizeof(char));
        file.seekg(0, ios::beg);
        file.read(str,FileLength);
        *pData = str;
        return FileLength;
    }
    
};

#endif /* defined(__Utils__CLocalManager__) */
