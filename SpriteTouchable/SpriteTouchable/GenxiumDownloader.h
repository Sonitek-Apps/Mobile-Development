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

#include "Macros.h"
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class downloadTask: public CCObject{
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

class GenxiumDownloader {
    
    CCObject* _pTarget;
    pthread_mutex_t _downloadQuotaMutex;
    pthread_mutex_t _downloadQueueMutex;
    int _downloadQuota;
    CCArray* _downloadQueue;
    CCScheduler* _scheduler;
    bool _bInitFinished;
    
public:
    GenxiumDownloader():
        _pTarget(NULL),
        _downloadQuotaMutex(PTHREAD_MUTEX_INITIALIZER),
        _downloadQueueMutex(PTHREAD_MUTEX_INITIALIZER),
        _downloadQuota(5),
        _downloadQueue(NULL),
        _scheduler(NULL),
        _bInitFinished(false){
    }
    
    ~GenxiumDownloader(){
        CC_SAFE_RELEASE_NULL(_pTarget);
        CC_SAFE_RELEASE_NULL(_downloadQueue);
        CC_SAFE_RELEASE_NULL(_scheduler);
    };
    
public:
    void initDownloaderWithTarget(CCObject* pTarget);
    bool addTaskToDownloadQueue(downloadTask* task);
    
private:
    void checkDownloadQueue(float dt);
    void downloadToNode(CCNode* node, CCString* url);
    void onDownloadToNodeFinished(CCNode*, void*);
};

#endif /* defined(__SpriteTouchable__GenxiumDownloader__) */
