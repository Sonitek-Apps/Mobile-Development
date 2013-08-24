//
//  GenxiumDownloader.h
//  SpriteTouchable
//
//  Created by Wing on 24/8/13.
//
//

#ifndef __GenxiumDownloader__
#define __GenxiumDownloader__

#include <iostream>
#include <pthread.h>
#include <queue>
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "ImageUtils.h"

using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

struct downloadTask{
    
    CCNode* _node;
    CCString* _url;
    
    downloadTask():_node(NULL), _url(NULL){}
    downloadTask(CCNode* node, CCString* url){
        _node=node;
        _url=url;
    }
};

class GenxiumDownloader : public CCObject, virtual public ImageUtils{
public:
    pthread_t _checkDownloadQueueThread;
    
public:
    static GenxiumDownloader* sharedDownloader(); // return singleton instance
    static void purgeSharedDownloader();
    GenxiumDownloader();
    ~GenxiumDownloader();
    
public:
    
    bool addTaskToDownloadQueue(downloadTask task);
    void invokeCheckingDownloadQueue();
    
public:
    void downloadToNode(CCNode* node, CCString* url);
    void onDownloadToNodeFinished(CCNode*, void*);
};

#endif /* defined(__SpriteTouchable__GenxiumDownloader__) */
