//
//  GameViewController.swift
//  gameTestSwift
//
//  Created by 林 on 2019/10/29.
//  Copyright © 2019 林. All rights reserved.
//

import UIKit
import SpriteKit
import GameplayKit

class GameViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        
        if let view = self.view as! SKView? {
            // Load the SKScene from 'GameScene.sks'
//            if let scene = SKScene(fileNamed: "GameScene") {
            let scene = GameScene.init(size: CGSize.init(width: 284, height: 512))
            scene.scaleMode = .aspectFill
            
            // Present the scene
            view.presentScene(scene)
//            if  {
//                // Set the scale mode to scale to fit the window
//              
//            }
            
            view.ignoresSiblingOrder = true
            
            view.showsFPS = true
            view.showsNodeCount = true
        }
    }

    override var shouldAutorotate: Bool {
        return true
    }

    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        if UIDevice.current.userInterfaceIdiom == .phone {
            return .allButUpsideDown
        } else {
            return .all
        }
    }

    override var prefersStatusBarHidden: Bool {
        return true
    }
}
