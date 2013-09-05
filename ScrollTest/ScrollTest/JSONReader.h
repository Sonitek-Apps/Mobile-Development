//
//  JSONReader.h
//  Puzzle
//
//  Created by Tact Sky on 5/8/13.
//
//

#ifndef __Puzzle__JSONReader__
#define __Puzzle__JSONReader__

#include <iostream>
#include <string>
#include "cJSON.h"
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <ctime>

using namespace std;
using namespace cocos2d;
using namespace extension;

class JSONReader : public CCObject
{
    
public:
    JSONReader();
    virtual ~JSONReader();
    static JSONReader* create(CCObject* pTarget, SEL_CallFuncO pSelector);
    bool init(CCObject* pTarget, SEL_CallFuncO pSelector);
    
public:
    void downloadJSON();
    bool isListFileUpToDate();
    bool updateListFile();
    
private:
    // callback functions
    void onJSONDownloadFinished(CCNode*, void*);
    
private:
    
    string m_feed;
    string m_listFilePath;
    string m_settingFilePath;
    
    CCObject*          m_pTarget;        /// callback target of pSelector function
    SEL_CallFuncO     m_pSelector;      /// callback function, e.g. MyLayer::onHttpResponse(CCObject *sender, void *data)
    //string m_res;
    time_t m_start;
};

#endif /* defined(__Puzzle__JSONReader__) */
