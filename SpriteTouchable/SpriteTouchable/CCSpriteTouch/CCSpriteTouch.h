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
#include "ImageUtils.h"
using namespace std;
USING_NS_CC;

class CCSpriteTouch: public CCNodeRGBA, public CCTargetedTouchDelegate, virtual public ImageUtils
{
public:
    CCSpriteTouch();
    virtual ~CCSpriteTouch();
    virtual bool init();
    virtual bool initWithTargetAndSelector(CCObject* , SEL_CallFuncND);
    static CCSpriteTouch* create();
    static CCSpriteTouch* createWithTargetAndSelector(CCObject* , SEL_CallFuncND);
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
public:
    void setTarget(CCObject*, SEL_CallFuncND);
    void startLoading();
    void stopLoading();
    void setPenetrable(bool);
    bool isDragging();
    void setScrollFriendlyThreshold(float scrollFriendlyThreshold);
    void setTouchEnabled(bool);
    
private:
    // callback functions
private:
    CCObject* _pTarget;
    SEL_CallFuncND _pSelector;
    CCSprite* _loadingIcon;
    CCAction* _loadingAction;
    bool _bLoading;
    float _nLoadingActionFrames;
    float _loadingActionCycle;
    CCPoint _lastTouchBegan;
    CCPoint _lastTouchEnded;
    float _scrollFriendlyThreshold;
};

#endif /* defined(__SpriteTouchable__CCSpriteTouch__) */