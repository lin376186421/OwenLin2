package com.mgame.ads;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;

import com.mgame.common.AppPlatform;
import com.tapjoy.TapjoyConnect;
import com.tapjoy.TapjoyFullScreenAdNotifier;
import com.tapjoy.TapjoyLog;
import com.tapjoy.TapjoyOffersNotifier;
import com.tapjoy.TapjoyVideoNotifier;
import com.tapjoy.TapjoyViewNotifier;

	// -------------------- OffersWallViewActivity ----------------------
	public class OffersWallViewActivity extends Activity implements TapjoyOffersNotifier
	{
		private TapjoyOffersNotifier offersNotifier = this;
	    public static OffersWallViewActivity  offerActivity = null;
	    public static Context context;
		// -------------------- MyTjViewNotifier ----------------------
		public final static  TapjoyViewNotifier tjViewNotifier = new  TapjoyViewNotifier()
		{
			@Override
			public void viewWillOpen(int arg0)
			{
				// TODO Auto-generated method stub
			}
			
			@Override
			public void viewWillClose(int arg0)
			{
				// TODO Auto-generated method stub
				if (TapjoyConnect.getTapjoyConnectInstance() != null) 
				{
					AppPlatform.getAppPlatform().tapjoyGetUpdatePoints();
				}
//				Intent intent = new Intent(context,GameClient.class);
//				offer.startActivity(intent);
				
				if(offerActivity!=null)
				{
					offerActivity.finish();

				}
				System.out.println("viewWillClose");
			}
			
			@Override
			public void viewDidOpen(int arg0) {
				// TODO Auto-generated method stub
			}
			
			@Override
			public void viewDidClose(int arg0) {
				// TODO Auto-generated method stub
			}
		};
		
		public final static TapjoyFullScreenAdNotifier tjFullScreenAdNotifier = new TapjoyFullScreenAdNotifier()
		{
			
			@Override
			public void getFullScreenAdResponseFailed(int arg0) {
				// TODO Auto-generated method stub
				System.out.println("getFullScreenAdResponseFailed");
				
			}
			
			@Override
			public void getFullScreenAdResponse() {
				// TODO Auto-generated method stub
				System.out.println("getFullScreenAdResponse");
				TapjoyConnect.getTapjoyConnectInstance().showFullScreenAd();


			}
		};
		
		public final static TapjoyVideoNotifier tjVideoNotifier = new TapjoyVideoNotifier()
		{

			@Override
			public void videoStart() 
			{
				System.out.println("videoStart");

			}

			@Override
			public void videoError(int statusCode)
			{
				System.out.println("videoError statusCode  === " + statusCode);

			}

			@Override
			public void videoComplete() 
			{
				
				System.out.println("videoComplete");

				// Best Practice: We recommend calling getTapPoints as often as possible so the user�s balance is always up-to-date.
			}
			
	};
		@Override
		protected void onCreate(Bundle savedInstanceState) {
			TapjoyLog.enableLogging(true);
			super.onCreate(savedInstanceState);
			if (TapjoyConnect.getTapjoyConnectInstance() != null) 
			{
				TapjoyConnect.getTapjoyConnectInstance().showOffers(offersNotifier);
				// TapjoyConnect.getTapjoyConnectInstance().showOffersWithCurrencyID("Gems",
				// false, offersNotifier);
			}
			offerActivity = this;
			context = this;
		}

		@Override
		protected void onResume() 
		{
			super.onResume();
			if (TapjoyConnect.getTapjoyConnectInstance() != null)
				TapjoyConnect.getTapjoyConnectInstance().enableDisplayAdAutoRefresh(true);
		}

		@Override
		public void onPause()
		{
			super.onPause();
			if (TapjoyConnect.getTapjoyConnectInstance() != null)
		    TapjoyConnect.getTapjoyConnectInstance().enableDisplayAdAutoRefresh(false);
		}

		@Override
		protected void onDestroy()
		{
			super.onDestroy();
			if (TapjoyConnect.getTapjoyConnectInstance() != null)
			TapjoyConnect.getTapjoyConnectInstance().enableDisplayAdAutoRefresh(false);
			finish();
		}

		@Override
		public void getOffersResponse() {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void getOffersResponseFailed(String arg0) {
			// TODO Auto-generated method stub
			
		}

		

	
}
