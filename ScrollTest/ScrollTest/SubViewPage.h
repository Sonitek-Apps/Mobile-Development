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

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class SubViewPage: public CCLayer{
public:
    SubViewPage();
    virtual ~SubViewPage();
    virtual void onEnter();
    virtual void onExit();
    virtual bool init(CCObject* pTarget, CCArray* urls);
    static SubViewPage* create(CCObject* pTarget, CCArray* urls);
    
public:
    void createNodes();
    void cleanupNodes();
    void downloadAll();
    void downloadToNode(const string& url, CCNode* node);
private:
    void onDownloadToNodeFinished(CCNode*, void*);
private:
    CCArray* _nodes;
    CCArray* _urls;
    CCObject* _pTarget;
};

#endif /* defined(__ScrollTest__SubViewPage__) */