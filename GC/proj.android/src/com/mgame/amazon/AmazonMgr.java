package com.mgame.amazon;


import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.json.JSONException;
import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.util.Log;

import com.amazon.inapp.purchasing.GetUserIdResponse.GetUserIdRequestStatus;
import com.amazon.inapp.purchasing.Item;
import com.amazon.inapp.purchasing.ItemDataResponse.ItemDataRequestStatus;
import com.amazon.inapp.purchasing.PurchaseResponse.PurchaseRequestStatus;
import com.amazon.inapp.purchasing.PurchaseUpdatesResponse.PurchaseUpdatesRequestStatus;
import com.amazon.inapp.purchasing.PurchasingManager;
import com.mgame.amazon.AppPurchasingObserver.PurchaseData;
import com.mgame.amazon.AppPurchasingObserver.PurchaseDataStorage;
import com.mgame.amazon.AppPurchasingObserver.SKUData;
import com.mgame.common.AppActivity;
import com.mgame.common.AppPlatform;

@SuppressLint("SdCardPath")
public class AmazonMgr implements AppPurchasingObserverListener
{
	private static final String TAG = "AmazonListener";

	private PurchaseDataStorage purchaseDataStorage;

	public AmazonMgr()
	{
		try {
			copyDataBase();
			} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			}
		
		setupIAPOnCreate();
	}
	
	private   void  copyDataBase ()  throws  IOException{

        InputStream myInput = AppActivity.getAppContext().getAssets ().open ( "amazon.sdktester.json" );
        String outFileName =  "/mnt/sdcard/amazon.sdktester.json" ;

        OutputStream myOutput =  new  FileOutputStream (outFileName);

         byte [] buffer =  new   byte [1024];

         int  length;

         while  ((length = myInput.read (buffer)) > 0){

        myOutput.write (buffer, 0, length);

    }
    myOutput.flush ();
    myOutput.close ();
    myInput.close ();
}
	private void setupIAPOnCreate() 
	{
		purchaseDataStorage = new PurchaseDataStorage(AppActivity.getAppContext());
		AppPurchasingObserver purchasingObserver = new AppPurchasingObserver(
				AppActivity.getAppContext(), purchaseDataStorage);
		purchasingObserver.setListener(this);
		Log.i(TAG, "onCreate: registering AppPurchasingObserver");
		PurchasingManager.registerObserver(purchasingObserver);
	}
	
	public void onResume()
	{
		Log.i(TAG, "onResume: call initiateGetUserIdRequest");
		PurchasingManager.initiateGetUserIdRequest();

		Log.i(TAG,
				"onResume: call initiateItemDataRequest for skus: "
						+ MySKU.getAll());
		PurchasingManager.initiateItemDataRequest(MySKU.getAll());
	}
	
	
	public void makePurchaseRequest(String sku) 
	{
		String requestId = PurchasingManager.initiatePurchaseRequest(sku);
		PurchaseData purchaseData = purchaseDataStorage.newPurchaseData(requestId);
		Log.i(TAG, "onBuyOrangeClick: requestId (" + requestId
				+ ") requestState (" + purchaseData.getRequestState() + ")");
	}
	
	@Override
	public void onGetUserIdResponseSuccessful(String userId, boolean userChanged) 
	{
		Log.i(TAG,"onGetUserIdResponseSuccessful: update display based on current userId");

		Set<String> requestIds = purchaseDataStorage.getAllRequestIds();
		Log.i(TAG, "onGetUserIdResponseSuccessful: (" + requestIds.size()
				+ ") saved requestIds");
		for (String requestId : requestIds) {
			PurchaseData purchaseData = purchaseDataStorage
					.getPurchaseData(requestId);
			if (purchaseData == null) {
				Log.i(TAG,"onGetUserIdResponseSuccessful: could NOT find purchaseData for requestId ("
								+ requestId + "), skipping");
				continue;
			}
			if (purchaseDataStorage.isRequestStateSent(requestId)) {
				Log.i(TAG,
						"onGetUserIdResponseSuccessful: have not received purchase response for requestId still in SENT status: requestId ("
								+ requestId + "), skipping");
				continue;
			}

			Log.d(TAG, "onGetUserIdResponseSuccessful: requestId (" + requestId
					+ ") " + purchaseData);
			String sku = purchaseData.getSKU();
			SKUData skuData = purchaseDataStorage.getSKUData(sku);

			if (!purchaseData.isPurchaseTokenFulfilled()) {
				Log.i(TAG, "onGetUserIdResponseSuccessful: purchaseToken ("
						+ purchaseData.getPurchaseToken()
						+ ") was NOT fulfilled, fulfilling purchase now");

				purchaseDataStorage.setPurchaseTokenFulfilled(purchaseData
						.getPurchaseToken());
				purchaseDataStorage.setRequestStateFulfilled(requestId);
			} else {
				Log.i(TAG, "onGetUserIdResponseSuccessful: for purchaseToken ("
						+ purchaseData.getPurchaseToken()
						+ ") call fulfillSKU on SKU: " + purchaseData.getSKU());
				final int haveQuantity = skuData.getHaveQuantity();
				final int consumedQuantity = skuData.getConsumedQuantity();

				Log.i(TAG,
						"onGetUserIdResponseSuccessful: call updateOrangesInView, have ("
								+ haveQuantity + ") oranges and consumed ("
								+ consumedQuantity + ") oranges");
			}
		}
	}

	/**
	 * Callback for a failed get user id response
	 * {@link GetUserIdRequestStatus#FAILED}
	 * 
	 * @param requestId
	 *            returned from {@link GetUserIdResponsee#getRequestId()} that
	 *            can be used to correlate with original request sent with
	 *            {@link PurchasingManager#initiateGetUserIdRequest()}.
	 */
	@Override
	public void onGetUserIdResponseFailed(String requestId) {
		Log.i(TAG, "onGetUserIdResponseFailed for requestId (" + requestId
				+ ")");
	}

	/**
	 * Callback for item data response with unavailable SKUs. This means that
	 * these unavailable SKUs are NOT accessible in developer portal. In this
	 * sample app, we would disable the buy button for these SKUs
	 * 
	 * @param unavailableSkus
	 */
	@Override
	public void onItemDataResponseSuccessfulWithUnavailableSkus(
			Set<String> unavailableSkus) {
	}

	/**
	 * Callback for successful item data response
	 * {@link ItemDataRequestStatus#SUCCESSFUL} with item data
	 * 
	 * @param itemData
	 *            - map of valid SKU->Items
	 */
	@Override
	public void onItemDataResponseSuccessful(Map<String, Item> itemData) {
		for (Entry<String, Item> entry : itemData.entrySet()) {
			String sku = entry.getKey();
			Item item = entry.getValue();
			Log.i(TAG, "onItemDataResponseSuccessful: sku (" + sku + ") item ("
					+ item + ")");
		}
	}

	/**
	 * Callback for failed item data response
	 * {@link ItemDataRequestStatus#FAILED}.
	 * 
	 * @param requestId
	 */
	public void onItemDataResponseFailed(String requestId) {
		Log.i(TAG, "onItemDataResponseFailed: for requestId (" + requestId
				+ ")");
	}

	/**
	 * Callback on successful purchase of SKU. In this sample app, we show level
	 * 2 as enabled
	 * 
	 * @param sku
	 */
	@Override
	public void onPurchaseResponseSuccess(String userId, String sku,
			String purchaseToken) {
		Log.i(TAG, "onPurchaseResponseSuccess: for userId (" + userId
				+ ") sku (" + sku + ")");
		String purchaseInfo = "";
		JSONObject receipt = new JSONObject();
			try {
				receipt.put("userId", userId);
				receipt.put("sku", sku);
				receipt.put("purchaseToken", purchaseToken);
			    purchaseInfo = receipt.toString();
				Log.d(TAG, receipt.toString());
			} catch (JSONException e) {
				e.printStackTrace();
			}
		
		AppPlatform.nativePaymentResult(sku, true, purchaseInfo);
		SKUData skuData = purchaseDataStorage.getSKUData(sku);
		if (skuData == null)
			return;
	}

	/**
	 * Callback when user is already entitled
	 * {@link PurchaseRequestStatus#ALREADY_ENTITLED} to sku passed into
	 * initiatePurchaseRequest.
	 * 
	 * @param userId
	 */
	@Override
	public void onPurchaseResponseAlreadyEntitled(String userId, String sku) {
		// This will not be called for consumables
		Log.i(TAG, "onPurchaseResponseAlreadyEntitled: for userId (" + userId
				+ ") sku ("+sku+")");
	}

	/**
	 * Callback when sku passed into
	 * {@link PurchasingManager#initiatePurchaseRequest} is not valid
	 * {@link PurchaseRequestStatus#INVALID_SKU}.
	 * 
	 * @param userId
	 * @param sku
	 */
	@Override
	public void onPurchaseResponseInvalidSKU(String userId, String sku) {
		Log.i(TAG, "onPurchaseResponseInvalidSKU: for userId (" + userId + ") sku ("+sku+")");
		AppPlatform.nativePaymentResult(sku, false, null);

	}

	/**
	 * Callback on failed purchase response {@link PurchaseRequestStatus#FAILED}
	 * .
	 * 
	 * @param requestId
	 * @param sku
	 */
	@Override
	public void onPurchaseResponseFailed(String requestId, String sku) {
		Log.i(TAG, "onPurchaseResponseFailed: for requestId (" + requestId
				+ ") sku ("+sku+")");
		AppPlatform.nativePaymentResult(sku, false, null);
	}

	/**
	 * Callback on successful purchase updates response
	 * {@link PurchaseUpdatesRequestStatus#SUCCESSFUL} for each receipt.
	 * 
	 * @param userId
	 * @param sku
	 * @param purchaseToken
	 */
	@Override
	public void onPurchaseUpdatesResponseSuccess(String userId, String sku,
			String purchaseToken) {
		// Not called for consumables
		Log.i(TAG, "onPurchaseUpdatesResponseSuccess: for userId (" + userId
				+ ") sku (" + sku + ") purchaseToken (" + purchaseToken + ")");
	}

	/**
	 * Callback on successful purchase updates response
	 * {@link PurchaseUpdatesRequestStatus#SUCCESSFUL} for revoked SKU.
	 * 
	 * @param userId
	 * @param revokedSKU
	 */
	@Override
	public void onPurchaseUpdatesResponseSuccessRevokedSku(String userId,
			String revokedSku) {
		// Not called for consumables
		Log.i(TAG, "onPurchaseUpdatesResponseSuccessRevokedSku: for userId ("
				+ userId + ")");
	}

	/**
	 * Callback on failed purchase updates response
	 * {@link PurchaseUpdatesRequestStatus#FAILED}
	 * 
	 * @param requestId
	 */
	public void onPurchaseUpdatesResponseFailed(String requestId) {
		 // Not called for consumables
		Log.i(TAG, "onPurchaseUpdatesResponseFailed: for requestId ("
				+ requestId + ")");
	}
	

}
