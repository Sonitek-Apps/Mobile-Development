//
//  CCSpriteTouch.h
//  SpriteTouchable
//
//  Created by Wing on 26/7/13.
//
//

#ifndef __SpriteTouchable__CCSpriteTouch__
#define __SpriteTouchable__CCSpriteTouch__

#include <iostream>
#include "cocos2d.h"
using namespace cocos2d;

#endif /* defined(__SpriteTouchable__CCSpriteTouch__) */

class CCSpriteTouch: public CCSprite, public CCTargetedTouchDelegate
{
public:
    CCSpriteTouch();
    virtual ~CCSpriteTouch();
public:
    static CCSpriteTouch* create();
    static CCSpriteTouch* create(const char* filename);
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};