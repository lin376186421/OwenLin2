package com.mgame.amazon;

import java.util.HashSet;
import java.util.Set;
import com.mgame.common.AppConfig;

;

public enum MySKU 
{
	SKU_1(AppConfig.PURCHASE_KEY_1, 1),
	SKU_2(AppConfig.PURCHASE_KEY_2, 1),
	SKU_3(AppConfig.PURCHASE_KEY_3, 1), 
	SKU_4(AppConfig.PURCHASE_KEY_4, 1),
	SKU_5(AppConfig.PURCHASE_KEY_5, 1), 
	SKU_6(AppConfig.PURCHASE_KEY_6, 1);

	private static Set<String> _skulst = new HashSet<String>();
	static {
		_skulst.add(SKU_1.getSku());
		_skulst.add(SKU_2.getSku());
		_skulst.add(SKU_3.getSku());
		_skulst.add(SKU_4.getSku());
		_skulst.add(SKU_5.getSku());
		_skulst.add(SKU_6.getSku());

	}
	private String _sku;
	private int _quantity;
	
	private MySKU(String sku, int quantity) {
		this._sku = sku;
		this._quantity = quantity;
	}

	public static MySKU valueForSKU(String sku) {
		for (MySKU mySku : MySKU.values()) {
			if (mySku.getSku().equals(sku)) {
				return mySku;
			}
		}
		return null;
	}

	public String getSku() {
		return _sku;
	}

	public void setSku(String sku) {
		_sku = sku;
	}

	public int getQuantity() {
		return _quantity;
	}

	public void setQuantity(int quantity) {
		_quantity = quantity;
	}

	public static Set<String> getAll() {
		return _skulst;
	}
}
