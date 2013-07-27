//
//  CCPopupWindow.cpp
//  PopupWindow
//
//  Created by Wing on 27/7/13.
//
//

#include "CCPopupWindow.h"
using namespace cocos2d;

CCScene* CCPopupWindow::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCPopupWindow *layer = CCPopupWindow::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool CCPopupWindow::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    winSize = CCDirector::sharedDirector()->getWinSize();
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object
    pCloseItem =
        CCMenuItemImage::create("CloseNormal.png",
                                "CloseSelected.png",
                                this,
                                menu_selector(CCPopupWindow::menuCloseCallback)
                                );
    
    pCloseItem->setPosition( ccp(winSize.width - 20, 20) );
    
    pPause =
        CCMenuItemImage::create(
                                "pause.jpeg",
                                "pause.jpeg",
                                this,
                                menu_selector(CCPopupWindow::menuPromptCallBack)
                                );
    
    pPause->setPosition(ccp(winSize.width/2, pPause->getContentSize().height));
    pPause->setScale(0.5);
    pPause->setVisible(true);
    
    pRestore = CCMenuItemImage::create(
                                                        "restore.png",
                                                        "restore.png",
                                                        this,
                                                        menu_selector(CCPopupWindow::menuRestoreCallBack)
                                                        );
    
    pRestore->setPosition(ccp(winSize.width/2,winSize.height/2));
    
    pRestore->setScale(0.3);
    pRestore->setVisible(false);
    
    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, pPause, pRestore, NULL);
    
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    
    
    return true;
}

void CCPopupWindow::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//=========================================================================
void CCPopupWindow::menuPromptCallBack(CCObject* pSender)
{
    pRestore->setVisible(true);
}
//=========================================================================
void CCPopupWindow::menuRestoreCallBack(CCObject* pSender)
{
    CCNode* pButton = (CCNode*) pSender;
    pRestore->setVisible(false);
}
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================