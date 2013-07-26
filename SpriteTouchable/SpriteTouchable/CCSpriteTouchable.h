//
//  CCSpriteTouchable.h
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#ifndef __SpriteTouchable__CCSpriteTouchable__
#define __SpriteTouchable__CCSpriteTouchable__

#include <iostream>
#include "cocos2d.h"
using namespace cocos2d;

#endif /* defined(__SpriteTouchable__CCSpriteTouchable__) */

class CCSpriteTouchable: public CCSprite, public CCTargetedTouchDelegate
{
public:
    CCSpriteTouchable();
    virtual ~CCSpriteTouchable();
public:
    static CCSpriteTouchable* create(const char* filename);
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};