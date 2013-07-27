//
//  CCPopupWindow.h
//  PopupWindow
//
//  Created by Wing on 27/7/13.
//
//

#ifndef __PopupWindow__CCPopupWindow__
#define __PopupWindow__CCPopupWindow__

#include <iostream>
#include <string>
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

class CCPopupWindow:public CCLayer
{
private:
    CCSize winSize;
    const char* message = "You pressed the pause button";
    CCMenuItemImage *pCloseItem, *pPause, *pRestore;
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(CCPopupWindow);
    
public:
    void menuPromptCallBack(CCObject* pSender);
    void menuRestoreCallBack(CCObject* pSender);
};

#endif /* defined(__PopupWindow__CCPopupWindow__) */
