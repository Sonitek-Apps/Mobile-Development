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
using namespace std;
USING_NS_CC;

class CCSpriteTouch: public CCNodeRGBA, public CCTargetedTouchDelegate
{
public:
    CCSpriteTouch();
    virtual ~CCSpriteTouch();
    virtual bool init();
    virtual bool initWithSpriteAndTarget(CCSprite*, CCObject* , SEL_CallFuncND);
    static CCSpriteTouch* create();
    static CCSpriteTouch* createWithSpriteAndTarget(CCSprite*, CCObject* , SEL_CallFuncND);
    void setMainSprite(CCSprite*);
    void setTarget(CCObject*, SEL_CallFuncND);
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
public:
    CCSprite* _mainSpr;
    CCObject* _pTarget;
    SEL_CallFuncND _pSelector;
    CCAction* buildLoadingAction();
    void startLoading();
    void stopLoading();
private:
    // callback functions
private:
    CCSprite* _loadingIcon;
    CCAction* _loadingAction;
    bool _bLoading;
    float _nLoadingActionFrames;
    float _loadingActionCycle;
};

#endif /* defined(__SpriteTouchable__CCSpriteTouch__) */