//
//  GameScene.m
//  gameTest
//
//  Created by 林 on 2019/10/23.
//  Copyright © 2019 林. All rights reserved.
//

#import "GameScene.h"
#import <AVFoundation/AVFoundation.h>

//一些全局常量
const int BIRD_RADIUS = 15; //小鸟半径
const int PIPE_HEIGHT = 320;//半根管道长度
const int PIPE_WIDTH = 52; //管道宽度
const int PIPE_SPACE = 100; //上下管之间的缝隙
const int PIPE_INTERVAL = 170;//横向两根管子之间的间距,288/2+52/2
const int WAIT_DISTANCE = 380;//等待距离

typedef enum : NSUInteger {
    GAME_READY,
    GAME_START,
    GAME_OVER,
} GAME_STATUS;

@interface GameScene()<SKPhysicsContactDelegate>
{
    
}

@property (nonatomic, strong)   AVAudioPlayer *musicPlayer;//背景音乐;

@end

@implementation GameScene {
    SKShapeNode *_spinnyNode;
    SKLabelNode *_label;

    //game
    SKSpriteNode *birdSprite;//小鸟
    SKLabelNode *scoreLabel;//计分
    SKSpriteNode *land1, *land2;//地板
    SKAction *swingAction;
    
    NSMutableArray *pipes; //管道，用容器装起来
    GAME_STATUS gameStatus; //游戏状态变量
    int score,bestScore;   //游戏当前分数和最好分数
    int touchX;  //触摸点横坐标
    
    NSTimer *_displayLink;
    NSTimer *_scrollLandLink;
    
    NSMutableDictionary *_musicDic;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
}

- (instancetype)initWithSize:(CGSize)size
{
    if (self = [super initWithSize:size]) {
        
        //物理世界 设置重力加速度
        self.physicsWorld.gravity = CGVectorMake(0, -9);
        self.physicsWorld.contactDelegate = self;//物理碰撞检测回调代理
        
        //设置边界
//        self.physicsBody = [SKPhysicsBody bodyWithEdgeLoopFromRect:CGRectMake(self.frame.origin.x, self.frame.origin.y, self.size.width, self.size.height)];

    }
    return self;
}

- (void)didMoveToView:(SKView *)view {

    // Setup your scene here
    
    // Get label node from scene and store it for use later
//    _label = (SKLabelNode *)[self childNodeWithName:@"//helloLabel"];
    
    CGSize visibleSize = self.size;
    CGPoint visibleOrigin = CGPointZero;
    
    gameStatus = GAME_READY;
    score = 0;
    
    SKSpriteNode *backGround = [SKSpriteNode spriteNodeWithImageNamed:@"bg.png"];
    [backGround setPosition:CGPointMake(visibleOrigin.x + visibleSize.width/2, visibleOrigin.y + visibleSize.height/2)];
    [self addChild:backGround];
    
    
    SKSpriteNode *gameLogo = [SKSpriteNode spriteNodeWithImageNamed:@"bird_logo.png"];
    gameLogo.position = CGPointMake(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2+100);
    gameLogo.name = @"logo";
    [self addChild:gameLogo];
    
    [self createPipes];
    
    birdSprite = [SKSpriteNode spriteNodeWithImageNamed:@"bird2.png"];
    birdSprite.position = CGPointMake(visibleOrigin.x + visibleSize.width / 3, visibleOrigin.y + visibleSize.height / 2);
    [self addChild:birdSprite];
    
    NSMutableArray *textures = [NSMutableArray array];
    [textures addObject:[SKTexture textureWithImageNamed:@"bird1.png"]];
    [textures addObject:[SKTexture textureWithImageNamed:@"bird2.png"]];
    [textures addObject:[SKTexture textureWithImageNamed:@"bird3.png"]];
    SKAction *birdAnim = [SKAction animateWithTextures:textures timePerFrame:0.2];
    [birdSprite runAction:[SKAction repeatActionForever:birdAnim]];//挥翅动画
    
    SKAction *up = [SKAction moveBy:CGVectorMake(0, 8) duration:0.4];
    SKAction *upBack = up.reversedAction;
    
    if (gameStatus == GAME_READY) {
        swingAction = [SKAction repeatActionForever:[SKAction sequence:@[up,upBack]]];
        [birdSprite runAction:swingAction withKey:@"swingAction"];//上下晃动动画
    }
    
    SKPhysicsBody *birdBody = [SKPhysicsBody bodyWithCircleOfRadius:BIRD_RADIUS];//将小鸟当成一个圆，懒得弄精确的轮廓线了
    [birdBody setDynamic:true];//设置为可以被物理场所作用而动作
    [birdBody setContactTestBitMask:1];//必须设置这项为1才能检测到不同的物体碰撞
    [birdBody setAffectedByGravity:NO];//设置是否被重力影响,准备画面中不受重
    [birdSprite setPhysicsBody:birdBody];//为小鸟设置刚体
    
    //添加两个land
    land1 = [SKSpriteNode spriteNodeWithImageNamed:@"land.png"];
    [land1 setAnchorPoint:CGPointZero];
    [land1 setPosition:CGPointZero];
    [self addChild:land1];
//    land1.zPosition = 10;
    
    land2 = [SKSpriteNode spriteNodeWithImageNamed:@"land.png"];
    [land2 setAnchorPoint:CGPointZero];
    [land2 setPosition:CGPointZero];
    [self addChild:land2];
//    land1.zPosition = 10;
    
    //设置地板刚体
    
    SKPhysicsBody *groundBody = [SKPhysicsBody bodyWithRectangleOfSize:CGSizeMake(visibleSize.width, land1.size.height)];
    groundBody.dynamic = NO;
    [groundBody setContactTestBitMask:1];
    
    SKSpriteNode *groundNode = [SKSpriteNode new];
    [groundNode setAnchorPoint:CGPointMake(0.5, 0.5)];
    [groundNode setPosition:CGPointMake(visibleOrigin.x+visibleSize.width/2,land1.size.height/2)];
    [groundNode setPhysicsBody:groundBody];
    [self addChild:groundNode];
    
    scoreLabel = [SKLabelNode labelNodeWithFontNamed:@"MarkerFelt"];
    scoreLabel.fontSize = 35;
    scoreLabel.text = @"0";
    [scoreLabel setPosition:CGPointMake(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2 + 200)];
    scoreLabel.hidden = YES;
    [self addChild:scoreLabel];
    
    CGFloat w = (self.size.width + self.size.height) * 0.05;
    // Create shape node to use during mouse interaction
    _spinnyNode = [SKShapeNode shapeNodeWithRectOfSize:CGSizeMake(w, w) cornerRadius:w * 0.3];
    _spinnyNode.lineWidth = 2.5;
    
    [_spinnyNode runAction:[SKAction repeatActionForever:[SKAction rotateByAngle:M_PI duration:0.1]]];
    [_spinnyNode runAction:[SKAction sequence:@[
                                                [SKAction waitForDuration:0.1],
                                                [SKAction fadeOutWithDuration:0.1],
                                                [SKAction removeFromParent],
                                                ]]];
    
    
}

//创建上下柱子
- (void)createPipes
{
    if (pipes == nil) {
        pipes = [NSMutableArray array];
    }
    
    for (int i = 0; i < 2; i++) {
        SKSpriteNode *pipeUp = [SKSpriteNode spriteNodeWithImageNamed:@"pipe_up.png"];
        
        SKNode *singlePipe = [SKNode new];
        
        SKPhysicsBody *pipeUpBody = [SKPhysicsBody bodyWithRectangleOfSize:pipeUp.size];
        pipeUpBody.dynamic = NO;
        pipeUpBody.contactTestBitMask = 1;
        
        pipeUp.anchorPoint = CGPointMake(0.5, 0.5);
        pipeUp.physicsBody = pipeUpBody;
        pipeUp.position = CGPointMake(0, PIPE_HEIGHT + PIPE_SPACE);
        
        SKSpriteNode *pipeDown = [SKSpriteNode spriteNodeWithImageNamed:@"pipe_down.png"];
        SKPhysicsBody *pipeDownBody = [SKPhysicsBody bodyWithRectangleOfSize:pipeDown.size];
        pipeDownBody.dynamic = NO;
        pipeDownBody.contactTestBitMask = 1;
        
        pipeDown.anchorPoint = CGPointMake(0.5, 0.5);
        pipeDown.physicsBody = pipeDownBody;

        [singlePipe addChild:pipeUp];
        [singlePipe addChild:pipeDown];
        
        singlePipe.position = CGPointMake(i*PIPE_INTERVAL + WAIT_DISTANCE, [self getRandomHeigh] );
        singlePipe.name = @"newPipe";
        
        [self addChild:singlePipe];
        [pipes addObject:singlePipe];
    }
}

//柱子初始化随机高度
- (int)getRandomHeigh
{
    CGSize size = self.size;
    //使得单根管子纵向坐标在屏幕中心点-(40~270)中间随机波动,这个是自己试出来的
    float height = size.height / 2 - 40 - (arc4random()%1000/1000.f) * (270 - 40);
    return height;
}

- (void)gameStart
{
    gameStatus = GAME_START;
    score = 0;//重置分数
    scoreLabel.text = [@(score) stringValue];
//    scoreLabel->setString(StringUtils::format("%d", score));
    
    [self childNodeWithName:@"logo"].hidden = YES;
    scoreLabel.hidden = NO;
    

    [_displayLink invalidate];
    _displayLink = nil;
    _displayLink = [NSTimer timerWithTimeInterval:1.0/60 target:self selector:@selector(updateTime:) userInfo:nil repeats:YES];
    
    [[NSRunLoop currentRunLoop] addTimer:_displayLink forMode:NSRunLoopCommonModes];
//    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    
    
    [_scrollLandLink invalidate];
    _scrollLandLink = nil;
//    _scrollLandLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(scrollLandUpdateTime:)];
    _scrollLandLink =  [NSTimer timerWithTimeInterval:0.01 target:self selector:@selector(scrollLandUpdateTime:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_scrollLandLink forMode:NSRunLoopCommonModes];
//    [_scrollLandLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    
    
//    [_displayLink invalidate];
//    _displayLink = nil;
//    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(update:)];

//    this->scheduleUpdate();//启动默认更新
//    this->schedule(schedule_selector(GameScene::scrollLand), 0.01f); //启动管子和地板滚动
    [birdSprite removeActionForKey:@"swingAction"];
    birdSprite.physicsBody.affectedByGravity = YES;
}

- (void)updateTime:(id)link
{
    //当游戏开始时，判断得分，这个其实也可以写在其他地方，比如管子滚动的更新函数里面或者触摸监测里面
    if (gameStatus == GAME_START)
    {
        for (SKNode *pipe in pipes)
        {
            if ([pipe.name isEqualToString:@"newPipe"]) //新来一根管子就判断
            {
                if (pipe.position.x < birdSprite.position.x)
                {
                    score++;
                    scoreLabel.text = [@(score) stringValue];
//                    scoreLabel->setString(StringUtils::format("%d", score));
//                    SimpleAudioEngine::getInstance()->playEffect("point.mp3");
//                    [SKAction playSoundFileNamed:@"point.mp3" waitForCompletion:NO];
                    [self playMusicWithName:@"point.mp3"];
                    pipe.name = @"passed"; //标记已经过掉的管子
                }
            }
        }
    }
    
    CGVector curVelocity = birdSprite.physicsBody.velocity; //根据竖直方向的速度算出旋转角度，逆时针为负
    NSLog(@"curVelocity:%.f",curVelocity.dy);
    [birdSprite setZRotation:-(-curVelocity.dy*0.1 - 20)/110];
    
    
    if (birdSprite.position.y <= land1.size.height+BIRD_RADIUS)
    {
        [birdSprite removeAllActions]; //小鸟挂了就不能再扇翅了
        birdSprite.zRotation = -70;  //设置为嘴朝下，顺时针70度旋转
        birdSprite.physicsBody.dynamic = NO;  //设置为不动了
//        SimpleAudioEngine::getInstance()->playEffect("die.mp3");
        [self playMusicWithName:@"die.mp3"];
//        [_displayLink setPaused:YES];

        
//        this->unscheduleUpdate();  //在小鸟掉到地面上再停止默认更新
//        gamePanelAppear();  //弹出记分牌
        [self gamePanelAppear];
    }
    
}

- (void)scrollLandUpdateTime:(id)link
{
    CGSize visibleSize = self.size;
    //两个图片循环移动
    land1.position = CGPointMake(land1.position.x - 1.1f, land1.position.y);
    land2.position = CGPointMake(land1.position.x + land1.size.width - 2.0f, land2.position.y);
    if (land2.position.x <= 0)
        land1.position = CGPointZero;
    
    //管子滚动
    for (SKNode *singlePipe in pipes)
    {
//        singlePipe->setPositionX(singlePipe->getPositionX() - 1.0f);
        singlePipe.position = CGPointMake(singlePipe.position.x-1.1f, singlePipe.position.y);
        if (singlePipe.position.x < -PIPE_WIDTH/2)
        {
            singlePipe.position = CGPointMake(visibleSize.width+PIPE_WIDTH/2, [self getRandomHeigh]);
//            singlePipe->setPositionX(visibleSize.width+PIPE_WIDTH/2);
//            singlePipe->setPositionY(getRandomHeight());
            singlePipe.name = @"newPipe";  //每次重设一根管子，标为new
        }
    }
}

- (void)gamePanelAppear
{
    [_displayLink invalidate];
    _displayLink = nil;
    
    [_scrollLandLink invalidate];
    _scrollLandLink = nil;
    
    CGSize size = self.size;
    CGPoint origin = CGPointZero;
    //用node将gameoverlogo和记分板绑在一起
    SKNode *gameOverPanelNode = [SKNode new];
    
    SKSpriteNode *gameOverLabel = [SKSpriteNode spriteNodeWithImageNamed:@"gameover.png"];
    [gameOverPanelNode addChild:gameOverLabel];
    
    SKSpriteNode *panel = [SKSpriteNode spriteNodeWithImageNamed:@"board.PNG"];//注意这里是大写PNG，原图片用什么后缀这里就用什么，区分大小写
    gameOverLabel.position = CGPointMake(gameOverLabel.position.x, panel.size.height);
    
    [gameOverPanelNode addChild:panel];
    
    SKLabelNode *curScoreTTF = [SKLabelNode labelNodeWithFontNamed:@"MarkerFelt"];
    curScoreTTF.fontSize = 20;
    curScoreTTF.text = [@(score) stringValue];
    curScoreTTF.color = [UIColor redColor];
//    curScoreTTF.position = CGPointMake(panel.size.width-40, panel.size.height-45);
    curScoreTTF.position = CGPointMake(70, 5);
    [panel addChild:curScoreTTF];
    
    SKLabelNode *bestScoreTTF = [SKLabelNode labelNodeWithFontNamed:@"MarkerFelt"];
    bestScoreTTF.fontSize = 20;
    bestScoreTTF.text = [@(bestScore) stringValue];
    bestScoreTTF.color = [UIColor greenColor];
//    bestScoreTTF.position = CGPointMake(panel.size.width - 40, panel.size.height - 90);
    bestScoreTTF.position = CGPointMake(70, -40);
    [panel addChild:bestScoreTTF];
    
    [self addChild:gameOverPanelNode];
    gameOverPanelNode.position = CGPointMake(origin.x + size.width / 2, origin.y + size.height );
    
    SKAction *action = [SKAction moveTo:CGPointMake(origin.x + size.width / 2, origin.y + size.height / 2) duration:0.5f];
    [gameOverPanelNode runAction:action];
    
    [self playMusicWithName:@"swooshing.mp3"];
    
//    auto gameOverLabel = Sprite::createWithSpriteFrameName("gameover.png");
//    gameOverPanelNode->addChild(gameOverLabel);
//    auto panel = Sprite::createWithSpriteFrameName("board.PNG");//注意这里是大写PNG，原图片用什么后缀这里就用什么，区分大小写
//    gameOverLabel->setPositionY(panel->getContentSize().height); //设置一下坐标
//    gameOverPanelNode->addChild(panel);
//    //记分板上添加两个分数
//    auto curScoreTTF = Label::createWithTTF(StringUtils::format("%d", score), "MarkerFelt.ttf", 20);
//    curScoreTTF->setPosition(panel->getContentSize().width-40, panel->getContentSize().height-45);
//    curScoreTTF->setColor(Color3B(255, 0, 0));
//    panel->addChild(curScoreTTF);
//    auto bestScoreTTF = Label::createWithTTF(StringUtils::format("%d", bestScore), "MarkerFelt.ttf", 20);
//    bestScoreTTF->setPosition(panel->getContentSize().width - 40, panel->getContentSize().height - 90);
//    bestScoreTTF->setColor(Color3B(0, 255, 0));
//    panel->addChild(bestScoreTTF);
//    this->addChild(gameOverPanelNode);
//    gameOverPanelNode->setPosition(origin.x + size.width / 2, origin.y + size.height );
//    //滑入动画
//    gameOverPanelNode->runAction(MoveTo::create(0.5f, Vec2(origin.x + size.width / 2, origin.y + size.height / 2)));
//    SimpleAudioEngine::getInstance()->playEffect("swooshing.mp3");
//    //添加菜单
//    //    MenuItemImage *restartItem = MenuItemImage::create("start_btn.png", "start_btn_pressed.png", this,menu_selector(GameScene::gameRestart));
//    MenuItemImage *restartItem = MenuItemImage::create("start_btn.png", "start_btn_pressed.png",CC_CALLBACK_1(GameScene:: gameRestart, this));
//    auto menu = Menu::createWithItem(restartItem);
//    menu->setPosition(origin.x + size.width / 2, 150);
//    this->addChild(menu);
}

- (void)playMusicWithName:(NSString *)musicName
{
    if ([musicName containsString:@".mp3"]) {
        musicName = [musicName stringByReplacingOccurrencesOfString:@".mp3" withString:@""];
    }
    if (_musicDic == nil) {
        _musicDic = [NSMutableDictionary dictionary];
    }
    if ([_musicDic objectForKey:musicName]) {
        SystemSoundID soundId = (SystemSoundID)[[_musicDic objectForKey:musicName] integerValue];
        AudioServicesPlaySystemSound(soundId);
    } else {
        NSString *path = [[NSBundle mainBundle] pathForResource:musicName ofType:@"mp3"];
        if (path.length) {
            NSURL *pathUrl = [NSURL fileURLWithPath:path];
            SystemSoundID soundId = 0;
            AudioServicesCreateSystemSoundID((__bridge CFURLRef _Nonnull)(pathUrl), &soundId);
            [_musicDic setObject:@(soundId) forKey:musicName];
            AudioServicesPlaySystemSound(soundId);
        }
    }
    
//    if ([musicName containsString:@".mp3"]) {
//        musicName = [musicName stringByReplacingOccurrencesOfString:@".mp3" withString:@""];
//    }
//    NSString *path = [[NSBundle mainBundle] pathForResource:musicName ofType:@"mp3"];
//    AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:nil];
//    [player prepareToPlay];
//    [player play];
//    _musicPlayer = player;
}

- (void)gameOver
{
    
    gameStatus = GAME_OVER;
    bestScore = (int)[[NSUserDefaults standardUserDefaults] integerForKey:@"BEST"];
    if (score > bestScore) {
        bestScore = score;//更新最好分数
        [[NSUserDefaults standardUserDefaults] setInteger:bestScore forKey:@"BEST"];
    }
    [self playMusicWithName:@"hit.mp3"];
    
    //游戏结束后停止地板和管道的滚动
//    [_scrollLandLink setPaused:YES];
    [_scrollLandLink invalidate];
    _scrollLandLink = nil;
}

- (void)gameRestart
{
    
    [_scrollLandLink invalidate];
    _scrollLandLink = nil;
    
    [_displayLink invalidate];
    _displayLink = nil;
    
    GameScene *scene = [GameScene sceneWithSize:CGSizeMake(284, 512)];
    // Set the scale mode to scale to fit the window
    scene.scaleMode = SKSceneScaleModeFill;
    SKView *skView = (SKView *)self.view;
    // Present the scene
    [skView presentScene:scene];
}

- (void)touchDownAtPoint:(CGPoint)pos {
    SKShapeNode *n = [_spinnyNode copy];
    n.position = pos;
    n.strokeColor = [SKColor greenColor];
    [self addChild:n];
}

- (void)touchMovedToPoint:(CGPoint)pos {
    SKShapeNode *n = [_spinnyNode copy];
    n.position = pos;
    n.strokeColor = [SKColor greenColor];
    [self addChild:n];
}

- (void)touchUpAtPoint:(CGPoint)pos {
    SKShapeNode *n = [_spinnyNode copy];
    n.position = pos;
    n.strokeColor = [SKColor greenColor];
    [self addChild:n];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    
    UITouch *touch = touches.anyObject;
    switch (gameStatus)
    {
        case GAME_OVER:
        {
            [self gameRestart];
        }
            break;
        case GAME_READY:
        {
            [self gameStart];
//            gameStart();
//            auto curVelocity = birdSprite->getPhysicsBody()->getVelocity();
//            [birdSprite.physicsBody applyImpulse:CGVectorMake(0, 15)];
            birdSprite.physicsBody.velocity = CGVectorMake(0, 350); //给一个向上的初速度
//            [birdSprite.physicsBody applyForce:CGVectorMake(0, 250)];
            [self playMusicWithName:@"wing.mp3"];
            touchX = [touch locationInView:touch.view].x;
        }
            break;
        case GAME_START:
        {
            CGVector curVelocity = birdSprite.physicsBody.velocity;
            birdSprite.physicsBody.velocity = CGVectorMake(0, 300>(curVelocity.dy + 600) ? (curVelocity.dy + 600):300); //向上的速度受下降影响
//            [birdSprite.physicsBody applyImpulse:CGVectorMake(0, 15)];
//            [birdSprite.physicsBody applyImpulse:CGVectorMake(0, 25>(curVelocity.dy + 50) ? (curVelocity.dy + 50):25)];
//            [birdSprite.physicsBody applyForce:CGVectorMake(0, 250>(curVelocity.dy + 500) ? (curVelocity.dy + 500):250)];
//            SimpleAudioEngine::getInstance()->playEffect("wing.mp3");
             [self playMusicWithName:@"wing.mp3"];
            //开上帝视角，留个后门，嘿嘿
            touchX = [touch locationInView:touch.view].x;
        }
            break;
        default:
            break;
    }
    
    // Run 'Pulse' action from 'Actions.sks'
    [_label runAction:[SKAction actionNamed:@"Pulse"] withKey:@"fadeInOut"];
    
    for (UITouch *t in touches) {[self touchDownAtPoint:[t locationInNode:self]];}
}
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    for (UITouch *t in touches) {[self touchMovedToPoint:[t locationInNode:self]];}
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    
    UITouch *touch = touches.anyObject;

    //当触摸点滑动超过100，分数瞬间涨100
    if ([touch locationInView:touch.view].x - touchX > 100)
    {
        score += 100;
        scoreLabel.text = [@(score) stringValue];
         [self playMusicWithName:@"point.mp3"];
//        SimpleAudioEngine::getInstance()->playEffect("point.mp3");
    }
    for (UITouch *t in touches) {[self touchUpAtPoint:[t locationInNode:self]];}
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *t in touches) {[self touchUpAtPoint:[t locationInNode:self]];}
}

#pragma mark SKPhysicsContactDelegate

- (void)didBeginContact:(SKPhysicsContact *)contact
{
    if (gameStatus == GAME_OVER)  //当游戏结束后不再监控碰撞
        return;
    
    SKNode *spriteA = contact.bodyA.node;
    SKNode *spriteB = contact.bodyB.node;
    if (spriteA == birdSprite || spriteB == birdSprite) {//fix 当有一个是小鸟参与的碰撞 游戏结束
        
//        self.physicsWorld;
        if (gameStatus != GAME_OVER) {
            [self gamePanelAppear];
            [self gameOver];
        }
    }
}


@end
