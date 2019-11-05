//
//  U17FooterView.swift
//  YYSwiftProject
//
//  Created by Domo on 2018/7/17.
//  Copyright © 2018年 知言网络. All rights reserved.
//

import UIKit
class U17FooterView: UICollectionReusableView {
    override init(frame: CGRect) {
        super.init(frame: frame)
        self.backgroundColor = FooterViewColor
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
}
