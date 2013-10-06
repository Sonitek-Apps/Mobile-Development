//
//  SubViewPage.h
//  ScrollTest
//
//  Created by Wing on 5/9/13.
//
//

#ifndef __ScrollTest__SubViewPage__
#define __ScrollTest__SubViewPage__

#include <iostream>
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ImageUtils.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class SubViewPage: public CCLayer, virtual public ImageUtils{
public:
    SubViewPage();
    virtual ~SubViewPage();
    virtual void onEnter();
    virtual void onExit();
    virtual bool init(CCObject* pTarget, CCArray* urls, CCSize pageSize);
    static SubViewPage* create(CCObject* pTarget, CCArray* urls, CCSize pageSize);
    
public:
    void createNodes();
    void cleanupNodes();
    void downloadAll();
    void downloadToNode(const string& url, CCNode* node);
private:
    void onDownloadToNodeFinished(CCNode*, void*);
private:
    CCSize _gapSize;
    CCSize _iconSize;
    CCArray* _nodes;
    CCArray* _urls;
    CCObject* _pTarget;
    
    CC_SYNTHESIZE(bool, _active, Active);
};

#endif /* defined(__ScrollTest__SubViewPage__) */
