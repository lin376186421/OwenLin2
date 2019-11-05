#//
//  CCDevice.mm
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#import "CDevice.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AdSupport/AdSupport.h>
#import "KeychainItemWrapper.h"

#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>

#include "md5.h"
#include "AppConfig.h"
#include "CAppPlatform.h"

static CDevice* s_pSharedDevice = NULL;
CDevice* CDevice::sharedDevice()
{
	if (s_pSharedDevice == NULL) {
		s_pSharedDevice = new CDevice();
		s_pSharedDevice->init();
	}
	return s_pSharedDevice;
}

void CDevice::init()
{
    macAddress_ = "";
    uniqueIdentifier_ = "";
}

string CDevice::getDeviceModel()
{
    NSString* deviceModel = [[UIDevice currentDevice] model];
    return [deviceModel UTF8String];
}

int CDevice::getDeviceFamily()
{
//    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];    //获取info－plist
//    NSArray* devices = [info objectForKey:@"UIDeviceFamily"];
//    
//    int deviceFamily = IPhone;
    
//    return deviceFamily;
    return 0;
}

std::string CDevice::getDeviceZone()
{
    return "";
}

std::string CDevice::macAddress()
{
    if (macAddress_ == "") {
        int					mib[6];
        size_t				len;
        char				*buf;
        unsigned char		*ptr;
        struct if_msghdr	*ifm;
        struct sockaddr_dl	*sdl;
        
        mib[0] = CTL_NET;
        mib[1] = AF_ROUTE;
        mib[2] = 0;
        mib[3] = AF_LINK;
        mib[4] = NET_RT_IFLIST;
        
        if ((mib[5] = if_nametoindex("en0")) == 0)
        {
            printf("Error: if_nametoindex error\n");
            return NULL;
        }
        
        if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
        {
            printf("Error: sysctl, take 1\n");
            return NULL;
        }
        
        buf = (char*)malloc(len);
        if (buf == NULL)
        {
            printf("Could not allocate memory. error!\n");
            return NULL;
        }
        
        if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
        {
            printf("Error: sysctl, take 2");
            free(buf);
            return NULL;
        }
        
        ifm = (struct if_msghdr *)buf;
        sdl = (struct sockaddr_dl *)(ifm + 1);
        ptr = (unsigned char *)LLADDR(sdl);
        // NSString *outstring = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
        NSString *outstring = [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
        free(buf);
        
        macAddress_ = [outstring UTF8String];
    }
    return macAddress_;
}

std::string CDevice::uniqueDeviceIdentifier()
{
    if (uniqueIdentifier_ == "") {
        NSString *version = [[UIDevice currentDevice] systemVersion];
        
        if ([version floatValue] >= 7.0) {
            
//            NSString* accessGroup = [NSString stringWithFormat:@"app.keychain.ww2"];
            
            KeychainItemWrapper *keychainIdfv = [[KeychainItemWrapper alloc] initWithIdentifier:@"device.idfv" accessGroup:nil];
            
            NSString* idfv = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            
            [keychainIdfv setObject:@"idfv" forKey:(id)kSecAttrAccount];
            [keychainIdfv setObject:idfv forKey:(id)kSecValueData];
            
            [keychainIdfv release];
            
            KeychainItemWrapper *keychainIdfa = [[KeychainItemWrapper alloc] initWithIdentifier:@"device.idfa" accessGroup:nil];
            
            NSString* idfa = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
            
            [keychainIdfa setObject:@"idfa" forKey:(id)kSecAttrAccount];
            [keychainIdfa setObject:idfa forKey:(id)kSecValueData];
            
            [keychainIdfa release];
            
            KeychainItemWrapper *keychainUUID = [[KeychainItemWrapper alloc] initWithIdentifier:@"device.uuid" accessGroup:nil];
            
            CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
            NSString* uuid = (NSString *)CFUUIDCreateString (kCFAllocatorDefault,uuidRef);
            
            [keychainUUID setObject:@"uuid" forKey:(id)kSecAttrAccount];
            [keychainUUID setObject:uuid forKey:(id)kSecValueData];
            
            [uuid release];
            [keychainUUID release];
            
            KeychainItemWrapper *keychainItem = [[KeychainItemWrapper alloc] initWithIdentifier:@"uuid" accessGroup:nil];
            
            NSString *uniqueIdentifier = [keychainItem objectForKey:(id)kSecValueData];
            if ([uniqueIdentifier isEqualToString:@""])
            {
                uniqueIdentifier = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
                [keychainItem setObject:uniqueIdentifier forKey:(id)kSecValueData];
            }
            [keychainItem release];
            
            uniqueIdentifier_ = [uniqueIdentifier UTF8String];
        }else{
            std::string macaddress = macAddress();
            
            uniqueIdentifier_ = MD5(macaddress).toString();
        }
    }
    return uniqueIdentifier_;
}

std::string CDevice::getUDID()
{
    return "";
}

std::string CDevice::getIDFA()
{
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        std::string macaddress = macAddress();
        idfa_ = string(MD5(macaddress).toString());
    }
    else
    {
        NSUUID *uuid = [[ASIdentifierManager sharedManager] advertisingIdentifier];
        NSString *uString = [uuid UUIDString];
        idfa_ = string([uString UTF8String]);
    }
    return idfa_;
}
