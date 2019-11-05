/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.mgame.activity;

import static com.mgame.gcm.CommonUtilities.*;

import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.Intent.ShortcutIconResource;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Parcelable;
import android.util.Base64;
import android.util.Log;
import android.view.ViewGroup;
import android.view.WindowManager;

import com.aarki.Aarki;
import com.chartboost.sdk.Chartboost;
import com.facebook.AppEventsLogger;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.flurry.android.FlurryAdSize;
import com.flurry.android.FlurryAds;
import com.flurry.android.FlurryAgent;
import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.ads.identifier.AdvertisingIdClient.Info;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;
import com.mgstudio.bg.R;
import com.mgame.ads.ChartboostAdDelegate;
import com.mgame.ads.FlurryAdDelegate;
import com.mgame.ads.SSADelegate;
import com.mgame.common.ActivityService;
import com.mgame.common.AppActivity;
import com.mgame.common.AppConfig;
import com.mgame.common.AppPlatform;
import com.mgame.common.AppRes;
import com.mgame.common.DeviceInfo;
import com.mgame.facebook.FacebookMgr;
import com.mgame.gcm.CommonUtilities;
import com.mgame.gcm.GCMRegistrar;
import com.mgame.gcm.ServerUtilities;
import com.mgstudio.bg.R;
import com.mobileapptracker.MobileAppTracker;
import com.nativex.monetization.MonetizationManager;
import com.nativex.monetization.listeners.SessionListener;
import com.supersonicads.sdk.android.SupersonicAdsPublisherAgent;
import com.tapjoy.TapjoyConnectFlag;

public class GameClient extends AppActivity{

	Chartboost chartboost;
	SupersonicAdsPublisherAgent mSupersonicAdsAgent;
	final static long chartboostInterval = 300000; // in milli seconds

	Intent mActivityService = null;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);// full screen

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,  
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		
		s_appActivity = this;

		AppPlatform.getAppPlatform().onLoad();
		
		// Flurry requestTapjoyConnect
		FlurryAdDelegate flurryAdDelegate = new FlurryAdDelegate();
		FlurryAds.setAdListener(flurryAdDelegate);

		Hashtable<String, String> flags = new Hashtable<String, String>();
		flags.put(TapjoyConnectFlag.ENABLE_LOGGING, "true");
		
		// Aarki
		Aarki.registerApp(getAppContext(), AppConfig.AARKI_SECRET_KEY);
		Aarki.setUserId(DeviceInfo.getAndroidID());

		// Chartboost
		this.chartboost = Chartboost.sharedChartboost();
		chartboost.onCreate(this, AppConfig.CHARTBOOST_APP_ID,
				AppConfig.CHARTBOOST_APP_SIGNATURE, new ChartboostAdDelegate());
		chartboost.startSession();		
		
		
		FacebookMgr.getFacebookPlatform().m_uiHelper = new UiLifecycleHelper(AppActivity.getAppContext(), callback);
		FacebookMgr.getFacebookPlatform().fbInit();
		
		
		try{
			PackageInfo info = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
			for(Signature signature : info.signatures)
			{
			MessageDigest md = MessageDigest.getInstance("SHA");
			md.update(signature.toByteArray());
			String keyHash =  Base64.encodeToString(md.digest(), Base64.DEFAULT);
			System.out.println("keyHash:" + keyHash);
		}
		}catch (NameNotFoundException e) {
		// TODO: handle exception
		}catch (NoSuchAlgorithmException e) {
		// TODO: handle exception
		}		

		// Session.openActiveSession(this, true, callback);

		//21031 fix
		try {
			MonetizationManager.enableLogging(true);
			MonetizationManager.setCurrencyListener(AppPlatform.getAppPlatform().getNativeXCurrencyListener());
			MonetizationManager.createSession(AppActivity.getAppContext(), AppConfig.NATIVE_X_KEY , sessionListener);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		// MobileAppTracker
		// Initialize MAT
		MobileAppTracker.init(getAppContext(), AppConfig.MOBILE_APP_ID,
				AppConfig.MOBILE_APP_KEY);
		mobileAppTracker = MobileAppTracker.getInstance();
		mobileAppTracker.setReferralSources(this);
		mobileAppTracker.setSiteId(AppPlatform.getAppPlatform()
				.getPackageName());
		// For existing users prior to MAT SDK implementation, call
		// setExistingUser(true) before measureSession.
		// Otherwise, existing users will be counted as new installs the first
		// time they run your app.
		boolean isExistingUser = MobileAppTracker.getInstance()
				.getExistingUser();
		if (isExistingUser) 
		{
			mobileAppTracker.setExistingUser(true);
		}
		
	   mSupersonicAdsAgent = SupersonicAdsPublisherAgent.getInstance(getAppContext());
       mSupersonicAdsAgent.initializeBrandConnect(getAppContext(), AppConfig.SSA_APP_KEY, AppConfig.DEVICE_ID,new SSADelegate(),null);

	   if(!isAddShortCut()){
           addShortCut();
       }
	   
		// Collect Google Play Advertising ID
		new Thread(new Runnable() {
			@Override
			public void run() {
				// See sample code at
				// http://developer.android.com/google/play-services/id.html
				try {
					Info adInfo = AdvertisingIdClient
							.getAdvertisingIdInfo(getApplicationContext());
					mobileAppTracker.setGoogleAdvertisingId(adInfo.getId(),
							adInfo.isLimitAdTrackingEnabled());
				} catch (Exception e) {
					e.printStackTrace();
				} 
			}
		}).start();

		// MdotM
		// MdotMAdView adView = new MdotMAdView(this.getApplicationContext());
		// MdotMAdRequest request = new MdotMAdRequest();
		// request.setAppKey(AppConfig.MDOTM_APP_KEY);
		// request.setAdSize(MdotMAdSize.BANNER_300_250);
		// request.setTestMode("0");
		// request.setEnableCaching(true);
		// adView.loadBannerAd(new MdotMAndroidSDKActivity(), request);

		try {
		
			GCM_SENDER_ID = AppRes.getRString(R.string.GCM_SENDER_ID);
			
			// Make sure the device has the proper dependencies.
			GCMRegistrar.checkDevice(this);
			// Make sure the manifest was properly set - comment out this line
			// while developing the app, then uncomment it when it's ready.
			GCMRegistrar.checkManifest(this);

			registerReceiver(mHandleMessageReceiver, new IntentFilter(
					DISPLAY_MESSAGE_ACTION));
			final String regId = GCMRegistrar.getRegistrationId(this);
			if (regId.equals("")) {
				// Automatically registers application on startup.
				GCMRegistrar.register(this, GCM_SENDER_ID);
			} else {
				// Device is already registered on GCM, check server.
				if (GCMRegistrar.isRegisteredOnServer(this)) {
					// Skips registration.

					Log.d("GCM", "gcm already registered!");
				} else {
					// Try to register again, but not in the UI thread.
					// It's also necessary to cancel the thread onDestroy(),
					// hence the use of AsyncTask instead of a raw thread.
					final Context context = this;
					mRegisterTask = new AsyncTask<Void, Void, Void>() {

						@Override
						protected Void doInBackground(Void... params) {
							boolean registered = ServerUtilities.register(
									context, regId);
							// At this point all attempts to register with the
							// app
							// server failed, so we need to unregister the
							// device
							// from GCM - the app will try to register again
							// when
							// it is restarted. Note that GCM will send an
							// unregistered callback upon completion, but
							// GCMIntentService.onUnregistered() will ignore it.
							if (!registered) {
								GCMRegistrar.unregister(context);
							}
							return null;
						}

						@Override
						protected void onPostExecute(Void result) {
							mRegisterTask = null;
						}

					};
					mRegisterTask.execute(null, null, null);
				}
			}
		} catch (Exception e) {
			Log.d(TAG, "gcm register failed!");
		}

		// //native debug
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	private SessionListener sessionListener = new SessionListener() {
	    @Override
	    public void createSessionCompleted(boolean success, boolean isOfferWallEnabled, String sessionId) {
	        if (success) {
	            // A session with our servers was established successfully.
	            // The app is ready to show ads.

	        	AppPlatform.getAppPlatform().nativeXCreated();
	        	
	        } else {
	            // Establishing a session with our servers failed.
	            // The app will be unable to show ads until a session is established.
	        }
	    }
	};
	
	private Session.StatusCallback callback = new Session.StatusCallback() {
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			System.out.println("SessionState" + state);
			if (session.isOpened()) {
				// Request.newMeRequest(session, new Request.GraphUserCallback()
				// {
				//
				// // callback after Graph API response with user object
				// @Override
				// public void onCompleted(GraphUser user, Response response) {
				// if (user != null) {
				// }
				// }
				// });
			}
		}
	};

	private MobileAppTracker mobileAppTracker = null;
	
	public boolean isAddShortCut() {
		SharedPreferences sp = getAppContext().getSharedPreferences("DataEditor", MODE_PRIVATE);
        boolean ret = sp.getBoolean("SHOT_CUT", false);
        return ret; 
	}
	 
	public void addShortCut(){
		 
		Intent shortcut = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");  
        
		shortcut.putExtra(Intent.EXTRA_SHORTCUT_NAME, getResources().getString(R.string.app_name));  
		ShortcutIconResource iconRes = Intent.ShortcutIconResource.fromContext(this.getApplicationContext(), R.drawable.icon);  
		shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON,iconRes);  
          
        shortcut.putExtra("duplicate", false);   
        Intent intent = new Intent(Intent.ACTION_MAIN);  
        intent.setFlags(Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);  
        intent.addFlags(Intent.FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY);  
        intent.addCategory(Intent.CATEGORY_LAUNCHER);  
        intent.setClass(GameClient.this, GameClient.class);  
        
        Parcelable icon = Intent.ShortcutIconResource.fromContext(this,R.drawable.icon);       
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE,icon);
        
        System.out.println("createIcon");  
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, intent);  
        GameClient.this.sendBroadcast(shortcut);  
        
        SharedPreferences sp = getAppContext().getSharedPreferences("DataEditor", MODE_PRIVATE);
        Editor editor = sp.edit();
        editor.putBoolean("SHOT_CUT", true);
        editor.commit();
    }
	
	@Override
	protected void onStart() {
		super.onStart();
		chartboost.onStart(this);
		chartboost.startSession();
		FlurryAgent.onStartSession(getAppContext(), AppConfig.FLURRY_API_KEY);
		ViewGroup view = (ViewGroup) ((Activity) AppActivity.getAppContext())
				.getWindow().getDecorView();
		FlurryAds.fetchAd(AppActivity.getAppContext(),
				AppConfig.FLURRY_AD_SPACE, view, FlurryAdSize.FULLSCREEN);

//		Session.getActiveSession().addCallback(callback);
		
	}
	
	@Override
	protected void onRestart() {
		super.onRestart();
		
		if (mSupersonicAdsAgent != null) {
			mSupersonicAdsAgent.applicationRestarted(AppActivity.getAppContext());
    		}		
//		AppPlatform.getAppPlatform().getNativeXPoint();
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
//		Session session = Session.getActiveSession();
//		Session.saveSession(session, outState);
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		
		FacebookMgr.getFacebookPlatform().m_uiHelper.onActivityResult(requestCode, resultCode, data,FacebookMgr.getFacebookPlatform().getDialogCallback());
		 
	}

	@Override
	protected void onResume() {
				
		super.onResume();
		new Timer().schedule(new TimerTask() {
			public void run() {
				// Chartboost.sharedChartboost().showInterstitial();
				// TapjoyConnect.getTapjoyConnectInstance().showOffers();

			}
		}, 500);
		
		//21031 fix
		try {
			MonetizationManager.enableLogging(true);
			MonetizationManager.setCurrencyListener(AppPlatform.getAppPlatform().getNativeXCurrencyListener());
			MonetizationManager.createSession(AppActivity.getAppContext(), AppConfig.NATIVE_X_KEY , sessionListener);
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (mActivityService != null) {
			stopService(mActivityService);
			mActivityService = null;
		}

		
		FacebookMgr.getFacebookPlatform().m_uiHelper.onResume();
		AppEventsLogger.activateApp(this,AppRes.getRString(R.string.facebook_app_id));
		

		mobileAppTracker.measureSession();

		if (mSupersonicAdsAgent != null) 
		{
			mSupersonicAdsAgent.applictionResumed();
		} 
	
		AppPlatform.getAppPlatform().onResume();	
		
	}

	@Override
	protected void onPause() {
		super.onPause();

		FlurryAgent.onEndSession(this);

		// background service
		try {
			mActivityService = new Intent(this, ActivityService.class);
			startService(mActivityService);
			bindService(mActivityService, new ServiceConnection() {

				@Override
				public void onServiceDisconnected(ComponentName name) {

				}

				@Override
				public void onServiceConnected(ComponentName name,
						IBinder service) {

				}
			}, 0);
		} catch (Exception e) {
			System.out.println("bindService error.");
		}

		if (mSupersonicAdsAgent != null) 
		{
			mSupersonicAdsAgent.applictionPaused();
		} 
		
		FacebookMgr.getFacebookPlatform().m_uiHelper.onPause();
		
	}

	@Override
	protected void onStop()
	{
		super.onStop();

		ViewGroup view = (ViewGroup) ((Activity) AppActivity.getAppContext())
				.getWindow().getDecorView();
		FlurryAds.removeAd(AppActivity.getAppContext(),
				AppConfig.FLURRY_AD_SPACE, view);

		FlurryAgent.onEndSession(this);
		chartboost.onStop(this);
//		Session.getActiveSession().removeCallback(callback);

		if (mSupersonicAdsAgent != null) 
		{
			mSupersonicAdsAgent.applictionStopped(AppActivity.getAppContext());
		} 		
		
		FacebookMgr.getFacebookPlatform().m_uiHelper.onStop();
		
	}

	@Override
	protected void onDestroy() {

		GCMRegistrar.onDestroy(this);

		chartboost.onDestroy(this);
		mSupersonicAdsAgent.release();
		
		stopService(mActivityService);
		mActivityService = null;
		
		
		FacebookMgr.getFacebookPlatform().m_uiHelper.onDestroy();
		
		
		MonetizationManager.release();

		super.onDestroy();
	}

	@Override
	public void onBackPressed() {
		Log.d("appactivity", "on backpressed");

		// If an interstitial is on screen, close it. Otherwise continue as
		// normal.
		if (chartboost.onBackPressed())
			return;
		else {
			super.onBackPressed();
		}
	}

	// public static String getDeviceID()
	// {
	// final TelephonyManager tm = (TelephonyManager)
	// getBaseContext().getSystemService(Context.TELEPHONY_SERVICE);
	//
	// final String tmDevice, tmSerial, androidId;
	// tmDevice = "" + tm.getDeviceId();
	// tmSerial = "" + tm.getSimSerialNumber();
	// androidId = "" +
	// android.provider.Settings.Secure.getString(getContentResolver(),
	// android.provider.Settings.Secure.ANDROID_ID);
	//
	// UUID deviceUuid = new UUID(androidId.hashCode(),
	// ((long)tmDevice.hashCode() << 32) | tmSerial.hashCode());
	// String deviceId = deviceUuid.toString();
	//
	// return deviceId;
	// }

	static {
		System.loadLibrary("game");
	}
}
