//
//  U17TodayCell.swift
//  YYSwiftProject
//
//  Created by Domo on 2018/7/23.
//  Copyright © 2018年 知言网络. All rights reserved.
//

import UIKit

class U17TodayCell: UITableViewCell {
    lazy var picImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.layer.masksToBounds = true
        imageView.isUserInteractionEnabled = true
        imageView.layer.cornerRadius = 20.0
        return imageView
    }()
    
    lazy var button: UIButton = {
        let button = UIButton.init(type: UIButtonType.custom)
        button.backgroundColor = DominantColor
        button.setTitle("阅读漫画", for: UIControlState.normal)
        button.setTitleColor(UIColor.white, for: UIControlState.normal)
        button.titleLabel?.font = UIFont.systemFont(ofSize: 18)
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 15
        button.addTarget(self, action:#selector(btnClick(button:)) , for: UIControlEvents.touchUpInside)
        return button
    }()
    
    override init(style: UITableViewCellStyle, reuseIdentifier: String?) {
        super.init(style: .default, reuseIdentifier: reuseIdentifier)
        setUpUI()
    }
    
    func incrementBy(amount:Int ,numberOftimes:Int ) {
        print("总和：\(amount+numberOftimes)")
    }
    
    @objc func btnClick(button:UIButton) -> Void {
        print("打几只")
        button.isSelected = !button.isSelected
        self.button.snp.updateConstraints({ (make) in
            if button.isSelected {
                make.size.equalTo(CGSize.init(width: 150, height: 60))
            }else{
                make.size.equalTo(CGSize.init(width: 100, height: 40))
            }
        })
    }
    func setUpUI(){
        self.incrementBy(amount: 6, numberOftimes: 6)

        self.addSubview(self.picImageView)
       
        self.picImageView.snp.makeConstraints { (make) in
            make.left.top.equalToSuperview().offset(15)
            make.right.bottom.equalToSuperview().offset(-15)
        }
        
        self.picImageView.addSubview(self.button)
        self.button.snp.makeConstraints { (make) in
            make.width.equalTo(100)
            make.right.bottom.equalToSuperview().offset(-25)
            make.height.equalTo(30)
        }
        
    }
    
    var model: DayItemModel? {
        didSet{
            guard let model = model else { return }
            self.picImageView.kf.setImage(with: URL(string:model.cover!))
        }
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
