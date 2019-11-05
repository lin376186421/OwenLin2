package com.mgame.common;

import java.util.HashMap;

public class AppConfig {
	
	public static int PAY_TYPE_GOOGLEPLAY = 0;
	public static int PAY_TYPE_PAYPAL = 1;
	public static int PAY_TYPE_AMAZON = 2;
	
	//app config
	public static String APP_GAME_NAME;
	public static String APP_ID;
	public static String DEVICE_ID;
	public static String APP_STORE_URL;
	public static String SERVER_URL;
	public static String WEB_URL;
	
	//ads config
	public static String ADWHIRL_APP_KEY;         
	public static String FLURRY_API_KEY;    
	public static String FLURRY_AD_SPACE;             
	public static String ADMOB_APP_ID;  
	public static String INMOBI_APP_ID;
	public static String TAPJOY_APP_ID;               
	public static String TAPJOY_SECRET_KEY;           
	public static String AARKI_APP_ID;                
	public static String AARKI_SECRET_KEY;            
	public static String CHARTBOOST_APP_ID;           
	public static String CHARTBOOST_APP_SIGNATURE;    
	public static String MDOTM_APP_KEY;
	public static String MOBILE_APP_ID;
	public static String MOBILE_APP_KEY;
	public static String SSA_APP_KEY;
	public static String NATIVE_X_KEY;
	public static String GETUI_APP_ID;
	public static String GETUI_APP_KEY;
	public static String GETUI_APP_SECRET;
	
	public static String PURCHASE_KEY_1;
	public static String PURCHASE_KEY_2;
	public static String PURCHASE_KEY_3;
	public static String PURCHASE_KEY_4;
	public static String PURCHASE_KEY_5;
	public static String PURCHASE_KEY_6;
	
	public static String    APP_DEBUG; 
	
	public static void initConfig(HashMap<String, Object> config)
	{
		
		APP_DEBUG = (String)config.get("APP_DEBUG");
		
		APP_GAME_NAME = (String)config.get("APP_GAME_NAME");
		APP_ID = (String)config.get("APP_ID");
		DEVICE_ID = (String)config.get("DEVICE_ID");
		APP_STORE_URL = (String)config.get("APP_STORE_URL");
		SERVER_URL = (String)config.get("SERVER_URL");
		WEB_URL = (String)config.get("WEB_URL");
		
		//ads config
		ADWHIRL_APP_KEY = (String)config.get("ADWHIRL_APP_KEY");       
		FLURRY_API_KEY = (String)config.get("FLURRY_API_KEY");  
		FLURRY_AD_SPACE = (String)config.get("FLURRY_AD_SPACE");             
		ADMOB_APP_ID = (String)config.get("ADMOB_APP_ID");          
		INMOBI_APP_ID = (String)config.get("INMOBI_APP_ID");
		TAPJOY_APP_ID = (String)config.get("TAPJOY_APP_ID");              
		TAPJOY_SECRET_KEY = (String)config.get("TAPJOY_SECRET_KEY");           
		AARKI_APP_ID = (String)config.get("AARKI_APP_ID");         
		AARKI_SECRET_KEY = (String)config.get("AARKI_SECRET_KEY");           
		CHARTBOOST_APP_ID = (String)config.get("CHARTBOOST_APP_ID");        
		CHARTBOOST_APP_SIGNATURE = (String)config.get("CHARTBOOST_APP_SIGNATURE"); 
		MDOTM_APP_KEY = (String)config.get("MDOTM_APP_KEY"); 
		MOBILE_APP_ID = (String)config.get("MOBILE_APP_ID"); 
		MOBILE_APP_KEY = (String)config.get("MOBILE_APP_KEY"); 
		SSA_APP_KEY = (String)config.get("SSA_APP_KEY"); 	
		NATIVE_X_KEY = (String)config.get("NATIVE_X_KEY");
		GETUI_APP_ID = (String)config.get("GETUI_APP_ID");
		GETUI_APP_KEY = (String)config.get("GETUI_APP_KEY");
		GETUI_APP_SECRET = (String)config.get("GETUI_APP_SECRET");
		
		//purchase key
		PURCHASE_KEY_1 = (String)config.get("PURCHASE_KEY_1"); 
		PURCHASE_KEY_2 = (String)config.get("PURCHASE_KEY_2"); 
		PURCHASE_KEY_3 = (String)config.get("PURCHASE_KEY_3"); 
		PURCHASE_KEY_4 = (String)config.get("PURCHASE_KEY_4"); 
		PURCHASE_KEY_5 = (String)config.get("PURCHASE_KEY_5"); 
		PURCHASE_KEY_6 = (String)config.get("PURCHASE_KEY_6");
	}
	
	
}
