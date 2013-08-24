//
//  GenxiumDownloader.h
//  SpriteTouchable
//
//  Created by Wing on 24/8/13.
//
//

#ifndef __SpriteTouchable__GenxiumDownloader__
#define __SpriteTouchable__GenxiumDownloader__

#include <iostream>
#include <pthread.h>
#include <queue>

#include "Macros.h"
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class downloadTask:public CCObject{
public:
    CCNode* _node;
    CCString* _url;
public:
    downloadTask():_node(NULL), _url(NULL){}
    downloadTask(CCNode* node, CCString* url){
        _node=node;
        _url=url;
    }
    ~downloadTask(){}
};

class GenxiumDownloader : public CCObject{
public:
    CCObject* _pTarget;
    bool _bInitFinished;
    pthread_t _checkDownloadQueueThread;
    
public:
    static GenxiumDownloader* sharedDownloader(); // return singleton instance
    static void purgeSharedDownloader();
    GenxiumDownloader();
    ~GenxiumDownloader();
    
public:
    
    void initDownloaderWithTarget(CCObject* pTarget);
    bool addTaskToDownloadQueue(downloadTask* task);
    void invokeCheckingDownloadQueue();
    
public:
    void downloadToNode(CCNode* node, CCString* url);
    void onDownloadToNodeFinished(CCNode*, void*);
};

#endif /* defined(__SpriteTouchable__GenxiumDownloader__) */
