//
//  CJsonParse.h
//  RobinHood
//
//  Created by qsun on 13-4-24.
//
//

#ifndef __Utils__CJsonParse__
#define __Utils__CJsonParse__

void JsonValueToDictionary(Json::Value jsonValue, CCDictionary* pDict);

inline string JsonValueToString(Json::Value value)
{
    Json::FastWriter writer;
    string valueToString = writer.write(value);
    string valueStr;
    if (value.type() == Json::stringValue) {
        valueStr = valueToString.substr(1, valueToString.length()-2-1); //去掉追加的 \"和/n
    }else{
        valueStr = valueToString.substr(0, valueToString.length()-2+1); //去掉追加的 /n
    }
    
    return valueStr;
}

inline void JsonArrayToArray(Json::Value jsonArray, CCArray* pArray)
{
    CCAssert(jsonArray.type() == Json::arrayValue, "Error: Json type error!");
    
    int size = jsonArray.size();
    for (int index =0; index < size; ++index)
    {
        if (jsonArray[index].type() == Json::objectValue) {
            CCDictionary* subDict = CCDictionary::create();
            JsonValueToDictionary(jsonArray[index], subDict);
            pArray->addObject(subDict);
        }else if(jsonArray[index].type() == Json::arrayValue){
            CCArray* subArray = CCArray::create();
            JsonArrayToArray(jsonArray[index], subArray);
            pArray->addObject(subArray);
        }
        else{
            string value = JsonValueToString(jsonArray[index]);
            pArray->addObject(CCString::create(value));
        }
    }
}

inline void JsonValueToDictionary(Json::Value jsonValue, CCDictionary* pDict)
{
    Json::Value::Members members(jsonValue.getMemberNames());
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
    {
        std::string &name = *it;
        if(jsonValue[name].type() == Json::objectValue)
        {
            CCDictionary* subDict = CCDictionary::create();
            JsonValueToDictionary(jsonValue[name], subDict);
            
            pDict->setObject(subDict, name);
        }
        else if(jsonValue[name].type() == Json::arrayValue)
        {
            CCArray* array = CCArray::create();
            
            JsonArrayToArray(jsonValue[name], array);
            
            pDict->setObject(array, name);
        }
        else
        {
            string value = JsonValueToString(jsonValue[name]);
            pDict->setObject(CCString::create(value), name);
        }
    }
}

void DictionaryToJsonValue(CCDictionary* pDict,Json::Value &jsonValue);

inline void ArrayToJsonArray(CCArray* pArray,Json::Value &jsonArray)
{
    CCObject* it = NULL;
    CCARRAY_FOREACH(pArray, it)
	{
        CCObject *pObject = dynamic_cast<CCObject*>(it);
        if ( dynamic_cast<CCDictionary*>(pObject) )
        {
            Json::Value value;
            DictionaryToJsonValue(dynamic_cast<CCDictionary*>(pObject),value);
            
            jsonArray.append(value);
        }
        else if ( dynamic_cast<CCArray*>(pObject) )
        {
            Json::Value value;
            
            ArrayToJsonArray(dynamic_cast<CCArray*>(pObject),value);
            jsonArray.append(value);
        }
        else
        {
            jsonArray.append( ((CCString*)pObject)->getCString());
        }
    }
}

inline void DictionaryToJsonValue(CCDictionary* pDict,Json::Value &jsonValue)
{
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(pDict, pElement)
    {
        CCObject *pObject = (CCObject*)pElement->getObject();
        string key = pElement->getStrKey();
        
        if ( dynamic_cast<CCDictionary*>(pObject) )
        {
            Json::Value value;
            DictionaryToJsonValue(dynamic_cast<CCDictionary*>(pObject),value);
            
            jsonValue[key] = value;
        }
        else if ( dynamic_cast<CCArray*>(pObject) )
        {
            Json::Value value;
            
            ArrayToJsonArray(dynamic_cast<CCArray*>(pObject),value);
            jsonValue[key] = value;
        }
        else
        {
            jsonValue[key] = ( (CCString*)pObject )->getCString();
        }
    }
}

#endif /* defined(__Utils__CJsonParse__) */
