#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

void HelloWorld::onEnter(){
    
    
    CCLayer::onEnter();
    
    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                                          "CloseNormal.png",
                                                          "CloseSelected.png",
                                                          this,
                                                          menu_selector(HelloWorld::menuCloseCallback) );
    
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );
    
    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);
    
    
    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);
    
    // ask director the window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint winMiddle(winSize.width*0.5, winSize.height*0.5);
    // position the label on the center of the screen
    pLabel->setPosition( ccp(winSize.width / 2, winSize.height - 20) );
    
    // add the label as a child to this layer
    this->addChild(pLabel, 1);
    
    CCSprite* spr=CCSprite::create("iTunesArtwork");
    CCSpriteTouch* pTouchTest = CCSpriteTouch::createWithSpriteAndTarget(spr, this, callfuncND_selector(HelloWorld::onSpriteTouchClicked));
    pTouchTest->setAnchorPoint(ccp(0.5,0.5));
    pTouchTest->setPosition(ccp(winSize.width*0.5,winSize.height*0.5));
    pTouchTest->setScrollFriendlyThreshold(0.5);
    
    CCString* url=CCString::create("http://www1.pictures.zimbio.com/pc/Louis+Tomlinson+One+Direction+Measured+Madame+D0oQ1dWL4xax.jpg");
    
    downloadTask task(pTouchTest,url);
    GenxiumDownloader* sharedDownloader=GenxiumDownloader::sharedDownloader();
    sharedDownloader->addTaskToDownloadQueue(task);
    sharedDownloader->invokeCheckingDownloadQueue();
    
    this->addChild(pTouchTest);
}

void HelloWorld::onExit(){
    CCLayer::onExit();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet=false;
    
    do {
        CC_BREAK_IF(!CCLayer::init());
        bRet=true;
    } while (false);
    
    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::onSpriteTouchClicked(CCNode* node, void* arg){
    CCLOG("SpriteTouch clicked!");
}