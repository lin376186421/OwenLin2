package com.mgame.common;


import java.util.UUID;

import android.content.Context;
import android.content.SharedPreferences;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;

public class DeviceInfo {

	protected static final String PREFS_FILE = "device_id.xml";
    protected static final String PREFS_DEVICE_ID = "device_id";
	public static UUID uuid = null;
	public static String deviceId = null;
	
	public static String getMacAddress()
	{
		WifiManager wifi = (WifiManager)AppActivity.getAppContext().getSystemService(Context.WIFI_SERVICE);
		WifiInfo info = wifi.getConnectionInfo();
		
		String macAddress = info.getMacAddress();
				
		return macAddress;
	}
	
	public static String getAndroidID()
	{
		String androidId = Secure.getString(AppActivity.getAppContext().getContentResolver(), Secure.ANDROID_ID);
		return androidId;
	}
	
	public static String getDeviceModel()
	{
		String model = android.os.Build.MODEL;
		return model;
	}
	
	public static String getDeviceId()
	{
        if(deviceId == null) {
        	
            final SharedPreferences prefs = AppActivity.getAppContext().getSharedPreferences(PREFS_FILE, 0);
            deviceId = prefs.getString(PREFS_DEVICE_ID, null);

            if (deviceId != null) {
                // Use the ids previously computed and stored in the prefs file
                uuid = UUID.fromString(deviceId);
            } else {

            		String type = null;
                final String androidId = Secure.getString(AppActivity.getAppContext().getContentResolver(), Secure.ANDROID_ID);

                // Use the Android ID unless it's broken, in which case fallback on deviceId,
                // unless it's not available, then fallback on a random number which we store
                // to a prefs file
                try {
                    if (!"9774d56d682e549c".equals(androidId)) {
                    	    type = "d01";
                        uuid = UUID.nameUUIDFromBytes(androidId.getBytes("utf8"));
                    } else {
                        final String deviceid = ((TelephonyManager)AppActivity.getAppContext().getSystemService(Context.TELEPHONY_SERVICE)).getDeviceId();
                        
                        if (deviceid != null) {
                        		type = "d02";
							uuid = UUID.nameUUIDFromBytes(deviceid.getBytes("utf8"));
						}
                        else {
                        		type = "d03";
                        		uuid = UUID.randomUUID();
                        }
                    }
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
                
                deviceId = type + uuid.toString();
                
                // Write the value out to the prefs file
                prefs.edit().putString(PREFS_DEVICE_ID, deviceId).commit();
            }
        }
        
        return deviceId;
	}
}
