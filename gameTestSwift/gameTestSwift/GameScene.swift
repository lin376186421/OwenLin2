//
//  GameScene.swift
//  gameTestSwift
//
//  Created by 林 on 2019/10/29.
//  Copyright © 2019 林. All rights reserved.
//

//创建物体小鸟  执行动画 （单个  多个  有序  永久  反向等）
//背景 障碍物的 循环移动
//创建物理世界  重力加速度  给物体某一个方向的速度然后自由落体  自由落体添加旋转角度达到更优的体验
//检测碰撞  精确的碰撞检测需要给每一个带有物理属性的物体设置描边路径  简单点就是矩形 圆形等等
//音效播放  注意cpu  需要缓冲 不能每次都读取音效文件
//OC swift zPosition 逻辑不一致  swift需要严格设置层级  OC 按添加顺序也可以

import SpriteKit
import GameplayKit
import AVFoundation
//一些全局常量
let BIRD_RADIUS:CGFloat = 15; //小鸟半径
let PIPE_HEIGHT:CGFloat = 320;//半根管道长度
let PIPE_WIDTH:CGFloat = 52; //管道宽度
let PIPE_SPACE:CGFloat = 100; //上下管之间的缝隙
let PIPE_INTERVAL:CGFloat = 170;//横向两根管子之间的间距,288/2+52/2
let WAIT_DISTANCE:CGFloat = 380;//等待距离

enum GAME_STATUS:Int {
    case GAME_READY=0
    case GAME_START
    case GAME_OVER
}

class GameScene: SKScene {
    
    private var birdSprite:SKSpriteNode!
    private var scoreLabel:SKLabelNode!
    private var land1:SKSpriteNode!
    private var land2:SKSpriteNode!
    private var swingAction:SKAction?
    
    private var pipes:[SKNode]?
    private var gameStatus:GAME_STATUS
    private var score:Int
    private var bestScore:Int
    private var touchX:CGFloat
    
    private var displayLink:Timer?
    private var scrollLandLink:Timer?
    
    private var label : SKLabelNode?
    private var spinnyNode : SKShapeNode?
    
    private var musicDic:Dictionary<String, SystemSoundID>!
    
    override init(size: CGSize) {
        
        score = 0
        bestScore = 0
        touchX = 0
        gameStatus = GAME_STATUS.GAME_READY //todo  不能写在 uper.init(size: size) 之后

        super.init(size: size)
    
        setup()
    }
    
    func setup() -> Void {
        self.physicsWorld.gravity = CGVector.init(dx: 0, dy: -9)
        self.physicsWorld.contactDelegate = self;
    }
    
    
    required init?(coder aDecoder: NSCoder) {

        fatalError("init(coder:) has not been implemented")
    }
    
    override func didMove(to view: SKView) {
        
        
        let visibleSize = self.size;
        let visibleOrigin = CGPoint.zero;
        
        
        if pipes == nil {
            pipes = []
        }
        
        if musicDic == nil {
            musicDic = Dictionary<String, SystemSoundID>()
        }
        
        
        gameStatus = GAME_STATUS.GAME_READY
        score = 0;
        
        let backGround:SKSpriteNode = SKSpriteNode.init(imageNamed:"bg.png");
        backGround.position = CGPoint.init(x: visibleOrigin.x + visibleSize.width/2, y: visibleOrigin.y + visibleSize.height/2)
        backGround.zPosition = 0;
        self.addChild(backGround)
        
        let gameLogo:SKSpriteNode = SKSpriteNode.init(imageNamed:"bird_logo.png");
        gameLogo.position = CGPoint.init(x:visibleOrigin.x + visibleSize.width / 2, y: visibleOrigin.y + visibleSize.height / 2+100)
        gameLogo.name = "logo";
        gameLogo.zPosition = 100;
        self.addChild(gameLogo)
        
        createPipes()
        
        birdSprite = SKSpriteNode.init(imageNamed:"bird2.png");
        birdSprite.position = CGPoint.init(x: visibleOrigin.x + visibleSize.width / 3, y: visibleOrigin.y + visibleSize.height / 2)
        birdSprite.zPosition = 100;
        self.addChild(birdSprite)
        
        let textures:[SKTexture] = [SKTexture.init(imageNamed: "bird1.png"),SKTexture.init(imageNamed: "bird2.png"),SKTexture.init(imageNamed: "bird3.png")]
        
        let birdAnim:SKAction = SKAction.animate(with: textures, timePerFrame: 0.2)
        birdSprite.run(SKAction.repeatForever(birdAnim))
        
        let up = SKAction.move(by: CGVector.init(dx: 0, dy: 8), duration: 0.4)
        let down = up.reversed()
        
        swingAction = SKAction.repeatForever(SKAction.sequence([up,down]))
        birdSprite.run(swingAction!, withKey: "swingAction")
        
        let birdBody = SKPhysicsBody.init(circleOfRadius: BIRD_RADIUS)//将小鸟当成一个圆，懒得弄精确的轮廓线了
        birdBody.isDynamic = true //设置为可以被物理场所作用而动作
        birdBody.contactTestBitMask = 1 //必须设置这项为1才能检测到不同的物体碰撞
        birdBody.affectedByGravity = false //设置是否被重力影响,准备画面中不受重
        birdSprite.physicsBody = birdBody  //为小鸟设置刚体
        
        //添加两个land
        land1 = SKSpriteNode.init(imageNamed: "land.png")
        land1.anchorPoint = CGPoint.zero
        land1.position = CGPoint.zero
        land1.zPosition = 10
        self.addChild(land1!)
        
        land2 = SKSpriteNode.init(imageNamed: "land.png")
        land2.anchorPoint = CGPoint.zero
        land2.position = CGPoint.zero
        land2.zPosition = 10
        self.addChild(land2!)
        
        //设置地板刚体
        let groundBody = SKPhysicsBody.init(rectangleOf: CGSize.init(width: visibleSize.width, height: land1!.size.height))
        groundBody.isDynamic = false
        groundBody.contactTestBitMask = 1
        
        let groundNode = SKSpriteNode()
        groundNode.anchorPoint = CGPoint.init(x: 0.5, y: 0.5)
        groundNode.position = CGPoint.init(x: visibleOrigin.x+visibleSize.width/2, y: land1!.size.height/2)
        groundNode.physicsBody = groundBody
        self.addChild(groundNode)
        
        scoreLabel = SKLabelNode.init(fontNamed: "MarkerFelt")
        scoreLabel?.fontSize = 35
        scoreLabel?.text = "0"
        scoreLabel?.position = CGPoint.init(x: visibleOrigin.x + visibleSize.width / 2, y: visibleOrigin.y + visibleSize.height / 2 + 200)
        scoreLabel.zPosition = 101;
        self.addChild(scoreLabel!)
        
//        // Get label node from scene and store it for use later
//        self.label = self.childNode(withName: "//helloLabel") as? SKLabelNode
//        if let label = self.label {
//            label.alpha = 0.0
//            label.run(SKAction.fadeIn(withDuration: 2.0))
//        }
        
        // Create shape node to use during mouse interaction
        let w = (self.size.width + self.size.height) * 0.05
        self.spinnyNode = SKShapeNode.init(rectOf: CGSize.init(width: w, height: w), cornerRadius: w * 0.3)
        
        if let spinnyNode = self.spinnyNode {
            spinnyNode.lineWidth = 2.5
            
            spinnyNode.run(SKAction.repeatForever(SKAction.rotate(byAngle: CGFloat(Double.pi), duration: 1)))
            spinnyNode.run(SKAction.sequence([SKAction.wait(forDuration: 0.1),
                                              SKAction.fadeOut(withDuration: 0.1),
                                              SKAction.removeFromParent()]))
        }
    }
    
    func createPipes() {
        
    
        for i in 0..<2 {
            let pipeUp = SKSpriteNode.init(imageNamed: "pipe_up.png")
            let singlePipe = SKNode()
            
            let pipeUpBody = SKPhysicsBody.init(rectangleOf: pipeUp.size)
            pipeUpBody.isDynamic = false
            pipeUpBody.contactTestBitMask = 1
            
            pipeUp.anchorPoint = CGPoint.init(x: 0.5, y: 0.5)
            pipeUp.physicsBody = pipeUpBody
            pipeUp.position = CGPoint.init(x: 0, y: PIPE_HEIGHT + PIPE_SPACE)
            
            let pipeDown = SKSpriteNode.init(imageNamed: "pipe_down.png")
            let pipeDownBody = SKPhysicsBody.init(rectangleOf: pipeDown.size)
            pipeDownBody.isDynamic = false
            pipeDownBody.contactTestBitMask = 1

            pipeDown.anchorPoint = CGPoint.init(x: 0.5, y: 0.5)
            pipeDown.physicsBody = pipeDownBody
            
            singlePipe.addChild(pipeUp)
            singlePipe.addChild(pipeDown)
            
            singlePipe.position = CGPoint.init(x: CGFloat(i)*PIPE_INTERVAL + WAIT_DISTANCE, y: getRandomHeigh())
            singlePipe.name = "newPipe"
            
            singlePipe.zPosition = 5
            self.addChild(singlePipe)
            
//            pipes?.add(singlePipe)
            pipes?.append(singlePipe)
        }
    }
    
    func getRandomHeigh() -> CGFloat {
        let size = self.size
        let height = size.height / 2 - 40 - (CGFloat(arc4random()%1000)/1000.0) * (270 - 40)
        return height
    }
    
    func gameStart() {
        gameStatus = GAME_STATUS.GAME_START
        score = 0
        scoreLabel?.text = "\(score)"
        
        let node = self.childNode(withName: "logo")
        node?.isHidden = true;
        scoreLabel.isHidden = false
        
        displayLink?.invalidate()
        displayLink = nil
        
        displayLink = Timer.init(timeInterval: 1.0/60, target: self, selector: #selector(GameScene.updateTime(link:)), userInfo: nil, repeats: true)
        RunLoop.current.add(displayLink!, forMode: RunLoop.Mode.common)
        
        scrollLandLink?.invalidate()
        scrollLandLink = nil
        
        scrollLandLink = Timer.init(timeInterval: 0.01, target: self, selector: #selector(GameScene.scrollLandUpdateTime(link:)), userInfo: nil, repeats: true)
        RunLoop.current.add(scrollLandLink!, forMode: RunLoop.Mode.common)
        
        birdSprite.removeAction(forKey: "swingAction")
        birdSprite.physicsBody?.affectedByGravity = true
    }
    
    @objc func updateTime(link:Timer) {
        if gameStatus == GAME_STATUS.GAME_START {
            for pipe in pipes! {
                if pipe.name == "newPipe" {
                    if pipe.position.x < (birdSprite.position.x) {
                        score += 1
                        scoreLabel?.text = "\(score)"
                        playMusicWithName(musicName: "point.mp3")
                        pipe.name = "passed"
                    }
                }
            }
        }
        
        let curVelocity = (birdSprite.physicsBody?.velocity)!
        NSLog("curVelocity:%.f", curVelocity.dy)
        
        birdSprite.zRotation = -(-curVelocity.dy*0.1 - 20)/100;
        if (birdSprite.position.y) <= ((land1.size.height)+BIRD_RADIUS) {
            birdSprite.removeAllActions()
            birdSprite.zRotation = -70;
            birdSprite.physicsBody?.isDynamic = false
            playMusicWithName(musicName: "die.mp3")
            gamePanelAppear()
            
        }
    }
    
    @objc func scrollLandUpdateTime(link:Timer) {
        
        let visibleSize = self.size
        
        land1.position = CGPoint.init(x: land1.position.x - 1.1, y: land1.position.y)
        land2.position = CGPoint.init(x: land1.position.x + land1.size.width - 2.0, y: land2.position.y)
        
        if land2.position.x <= 0 {
            land1.position = CGPoint.zero
        }
        
        
        for singlePipe in pipes! {
            
            singlePipe.position = CGPoint.init(x: singlePipe.position.x-1.1, y: singlePipe.position.y)
            if singlePipe.position.x < -PIPE_WIDTH/2 {
                singlePipe.position = CGPoint.init(x: visibleSize.width+PIPE_WIDTH/2, y: getRandomHeigh())
                singlePipe.name = "newPipe"
            }
        }
    }
    
    func playMusicWithName(musicName:String) {
        var newMusicName = musicName;
        if musicName.contains(".mp3") {
            newMusicName = musicName.replacingOccurrences(of: ".mp3", with: "")
        }

        if let sId = musicDic[newMusicName] {
            AudioServicesPlaySystemSound(sId)
        } else {
            let path = Bundle.main.path(forResource: newMusicName, ofType: ".mp3")
            if path != nil {
                let newPath = URL.init(fileURLWithPath: path!)
                var soundId:SystemSoundID = 0
                AudioServicesCreateSystemSoundID(newPath as CFURL, &soundId)
                musicDic[newMusicName] = soundId
                AudioServicesPlaySystemSound(soundId)
            }
        }
    }
    
    func gamePanelAppear() {
        displayLink?.invalidate()
        displayLink = nil
        
        scrollLandLink?.invalidate()
        scrollLandLink = nil
        
        let size = self.size
        let origin = CGPoint.zero
        
        let gameOverPanelNode = SKNode()
        let gameOverLabel = SKSpriteNode.init(imageNamed: "gameover.png")

        gameOverPanelNode.addChild(gameOverLabel)
        
        let panel = SKSpriteNode.init(imageNamed: "board.PNG")
     
        gameOverLabel.position = CGPoint.init(x: gameOverLabel.position.x, y: panel.size.height)

        
        gameOverPanelNode.addChild(panel)
        
        let curScoreTTF = SKLabelNode.init(fontNamed: "MarkerFelt")
        curScoreTTF.fontSize = 20
        curScoreTTF.text = "\(score)"
        curScoreTTF.color = UIColor.red
        curScoreTTF.position = CGPoint.init(x: 70, y: 5)
        curScoreTTF.zPosition = 5
//        curScoreTTF.position = CGPoint.init(x: 0, y: 0)
        panel.addChild(curScoreTTF)
        
        let bestScoreTTF = SKLabelNode.init(fontNamed: "MarkerFelt")
        bestScoreTTF.fontSize = 20
        bestScoreTTF.text = "\(bestScore)"
        bestScoreTTF.color = UIColor.green
        bestScoreTTF.position = CGPoint.init(x: 70, y: -40)
        bestScoreTTF.zPosition = 5
        panel.addChild(bestScoreTTF)
        
        gameOverPanelNode.zPosition = 200;
        self.addChild(gameOverPanelNode)
        gameOverPanelNode.position = CGPoint.init(x: origin.x + size.width / 2, y: origin.y + size.height)
        
        let action = SKAction.move(to: CGPoint.init(x: origin.x + size.width / 2, y: origin.y + size.height / 2), duration: 0.5)
        gameOverPanelNode.run(action)
        
        playMusicWithName(musicName: "swooshing.mp3")
        
    }
    
    func gameOver(){
        gameStatus = .GAME_OVER
        bestScore = UserDefaults.standard.integer(forKey: "BEST")
        if score > bestScore {
            bestScore = score
            UserDefaults.standard.set(bestScore, forKey: "BEST")
        }
        playMusicWithName(musicName: "hit.mp3")
        
        scrollLandLink?.invalidate()
        scrollLandLink = nil
    }
    
    func gameRestart() {
        
        scrollLandLink?.invalidate()
        scrollLandLink = nil
        
        displayLink?.invalidate()
        displayLink = nil
        
        let scene = GameScene.init(size:CGSize.init(width: 284, height: 512))
        scene.scaleMode = .aspectFill
        let skView = self.view
        skView?.presentScene(scene)
    }
    
    func touchDown(atPoint pos : CGPoint) {
        if let n = self.spinnyNode?.copy() as! SKShapeNode? {
            n.position = pos
            n.strokeColor = SKColor.green
            n.zPosition = 1000;
            self.addChild(n)
        }
    }
    
    func touchMoved(toPoint pos : CGPoint) {
        if let n = self.spinnyNode?.copy() as! SKShapeNode? {
            n.position = pos
            n.strokeColor = SKColor.green
            n.zPosition = 1000;
            self.addChild(n)
        }
    }
    
    func touchUp(atPoint pos : CGPoint) {
        if let n = self.spinnyNode?.copy() as! SKShapeNode? {
            n.position = pos
            n.strokeColor = SKColor.green
            n.zPosition = 1000;
            self.addChild(n)
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        
        
//        for touch:AnyObject in touches {
//            let t:UITouch = touch as! UITouch
//            //当在屏幕上连续拍动两下时，背景回复为白色
//            if t.tapCount == 2
//            {
//                self.view.backgroundColor = UIColor.white
//            }else if t.tapCount == 1
//            {
//                self.view.backgroundColor = UIColor.blue
//            }
//        }
        
        let touch = (touches as NSSet).anyObject() as! UITouch
        switch gameStatus {
        case .GAME_OVER:
            gameRestart()
            break
        case .GAME_READY:
            gameStart()
            birdSprite.physicsBody?.velocity = CGVector.init(dx: 0, dy: 350)
            playMusicWithName(musicName: "wing.mp3")
            touchX = touch.location(in: touch.view).x
            break
        case .GAME_START:
            let curVelocity = (birdSprite.physicsBody?.velocity)!
            birdSprite.physicsBody?.velocity = CGVector.init(dx: 0, dy: 300>(curVelocity.dy + 600) ? (curVelocity.dy + 600):300)
            playMusicWithName(musicName: "wing.mp3")
            touchX = touch.location(in: touch.view).x
            break
//        default:
//            break
            
        }
        

        if let label = self.label {
            label.run(SKAction.init(named: "Pulse")!, withKey: "fadeInOut")
        }
        
        for t in touches { self.touchDown(atPoint: t.location(in: self)) }
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        for t in touches { self.touchMoved(toPoint: t.location(in: self)) }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        
        let touch = (touches as NSSet).anyObject() as! UITouch
        let newTouchX = touch.location(in: touch.view).x
        if newTouchX - touchX > 100.0 {
            score += 100
            scoreLabel.text = "\(score)"
            playMusicWithName(musicName: "point.mp3")
        }
        
        for t in touches { self.touchUp(atPoint: t.location(in: self)) }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        for t in touches { self.touchUp(atPoint: t.location(in: self)) }
    }
    
    
    override func update(_ currentTime: TimeInterval) {
        // Called before each frame is rendered
    }
    

}


extension GameScene:SKPhysicsContactDelegate {
    
    func didBegin(_ contact: SKPhysicsContact) {
        
        let spriteA = contact.bodyA.node
        let spriteB = contact.bodyB.node
        
        if spriteA == birdSprite || spriteB == birdSprite {//fix 当有一个是小鸟参与的碰撞 游戏结束
            if gameStatus != .GAME_OVER {
                gamePanelAppear()
                gameOver()
            }
            
        }
        
    }
    
}
