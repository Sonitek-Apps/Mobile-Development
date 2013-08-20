#include "CCMenuTouch.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

// on "init" you need to initialize your instance
bool CCMenuTouch::init()
{
    return initWithArray(NULL);
}

bool CCMenuTouch::initWithArray(CCArray* pItems, int zOrder)
{
    if (CCLayer::init())
    {
        setTouchPriority(-zOrder);
        setTouchMode(kCCTouchesOneByOne);
        setTouchEnabled(true);
        
        // menu in the center of the screen
        CCSize s = CCDirector::sharedDirector()->getWinSize();
        
        this->ignoreAnchorPointForPosition(true);
        setAnchorPoint(ccp(0.5f, 0.5f));
        this->setContentSize(s);
        
        setPosition(ccp(s.width/2, s.height/2));
        
        if (pItems != NULL)
        {
            int z=0;
            CCObject* pObj = NULL;
            CCARRAY_FOREACH(pItems, pObj)
            {
                CCMenuItem* item = (CCMenuItem*)pObj;
                this->addChild(item, z);
            }
        }
    
        // enable cascade color and opacity on menus
        setCascadeColorEnabled(true);
        setCascadeOpacityEnabled(true);
        
        return true;
    }
    return false;
}

CCMenuTouch* CCMenuTouch::create()
{
    return createWithArray(NULL);
}

CCMenuTouch* CCMenuTouch::createWithArray(CCArray* pItems, int zOrder)
{
    CCMenuTouch *pRet = new CCMenuTouch();
    if (pRet && pRet->initWithArray(pItems,zOrder))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}


CCScene* CCMenuTouch::scene()
{
    return sceneWithArray(NULL);
}

CCScene* CCMenuTouch::sceneWithArray(CCArray* pItems, int zOrder)
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCMenuTouch *layer = CCMenuTouch::createWithArray(pItems, zOrder);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

void CCMenuTouch::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
