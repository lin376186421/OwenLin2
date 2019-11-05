//
//  CCDevice.cpp
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#include "CDevice.h"
#include <stdlib.h>
#include "../Utils/md5.h"

using namespace std;
using namespace cocos2d;

CDevice* CDevice::sharedDevice()
{
	static CDevice s_device;

	return &s_device;
}

std::string CDevice::macAddress() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/DeviceInfo", "getMacAddress","()Ljava/lang/String;")) {
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		string macaddress = JniHelper::jstring2string(jstr);
		minfo.env->DeleteLocalRef(minfo.classID);

		CCLog("Mac Address: %s", macaddress.c_str());

		return macaddress;
	}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    NCB ncb;     //����һ��NCB(������ƿ�)���͵Ľṹ�����ncb  
    typedef struct _ASTAT_     //�Զ���һ���ṹ��_ASTAT_  
    {  
        ADAPTER_STATUS   adapt;   
        NAME_BUFFER   NameBuff   [30];       
    }ASTAT, *PASTAT;  
    ASTAT Adapter;     
  
    typedef struct _LANA_ENUM     //�Զ���һ���ṹ��_ASTAT_  
    {  
        UCHAR length;   
        UCHAR lana[MAX_LANA];     //�������MAC��ַ   
    }LANA_ENUM;       
    LANA_ENUM lana_enum;     
  
    //   ȡ��������Ϣ�б�       
    UCHAR uRetCode;       
    memset(&ncb, 0, sizeof(ncb));     //���ѿ����ڴ�ռ�ncb ��ֵ����Ϊֵ 0  
    memset(&lana_enum, 0, sizeof(lana_enum));     //���һ���ṹ���͵ı���lana_enum����ֵΪ0  
    //�Խṹ�����ncb��ֵ  
    ncb.ncb_command = NCBENUM;     //ͳ��ϵͳ������������  
    ncb.ncb_buffer = (unsigned char *)&lana_enum; //ncb_buffer��Աָ����LANA_ENUM�ṹ���Ļ�����  
    ncb.ncb_length = sizeof(LANA_ENUM);     
    //����������NCBENUM����Ի�ȡ��ǰ������������Ϣ�����ж��ٸ�������ÿ�������ı�ţ�MAC��ַ��   
    uRetCode = Netbios(&ncb); //����netbois(ncb)��ȡ�������к�      
    if(uRetCode != NRC_GOODRET)       
	    return "";       
  
    //��ÿһ�������������������Ϊ�����ţ���ȡ��MAC��ַ     
    for(int lana=0; lana<lana_enum.length; lana++)       
    {  
        ncb.ncb_command = NCBRESET;   //����������NCBRESET������г�ʼ��  
        ncb.ncb_lana_num = lana_enum.lana[lana];   
        uRetCode = Netbios(&ncb);     
    }   
    if(uRetCode != NRC_GOODRET)  
		return "";       
      
    //   ׼��ȡ�ýӿڿ���״̬��ȡ��MAC��ַ  
    memset(&ncb, 0, sizeof(ncb));   
    ncb.ncb_command = NCBASTAT;    //����������NCBSTAT�����ȡ������Ϣ  
    ncb.ncb_lana_num = lana_enum.lana[0];     //ָ�������ţ��������ָ����һ��������ͨ��Ϊ��������   
    strcpy((char*)ncb.ncb_callname, "*");     //Զ��ϵͳ����ֵΪ*  
    ncb.ncb_buffer = (unsigned char *)&Adapter; //ָ�����ص���Ϣ��ŵı���  
    ncb.ncb_length = sizeof(Adapter);  
    //���ŷ���NCBASTAT�����Ի�ȡ��������Ϣ  
    uRetCode = Netbios(&ncb);   
    //   ȡ����������Ϣ����������������������Ļ������ر�׼��ð�ŷָ���ʽ��     
    if(uRetCode != NRC_GOODRET)     
		return "";     
    //������MAC��ַ��ʽת��Ϊ���õ�16������ʽ,������ַ���mac��   

	char mac[128];
    sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",       
    Adapter.adapt.adapter_address[0],       
    Adapter.adapt.adapter_address[1],       
    Adapter.adapt.adapter_address[2],       
    Adapter.adapt.adapter_address[3],       
    Adapter.adapt.adapter_address[4],       
    Adapter.adapt.adapter_address[5]   
    );   
	std::string strMac = mac;
	return strMac;
#endif
}

std::string CDevice::uniqueDeviceIdentifier() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string macaddress = macAddress();

	string uniqueIdentifier = MD5(macaddress).toString();

	return uniqueIdentifier;
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::string macaddress = macAddress();

	string uniqueIdentifier = MD5(macaddress).toString();

	return uniqueIdentifier;
#endif // 
}

std::string CDevice::getDeviceModel()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::string device = "win";

	return device;
#endif // 
}
