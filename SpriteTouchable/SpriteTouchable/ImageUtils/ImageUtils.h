//
//  ImageUtils.h
//  SpriteTouchable
//
//  Created by Wing on 25/8/13.
//
//

#ifndef __ImageUtils__
#define __ImageUtils__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include "cocos2d.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Constants.h"
#include "dirent.h"
using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

class ImageUtils{
    
public:
    ImageUtils();
    virtual ~ImageUtils();
    
public:
    void cropRegionOfSpriteBySizeRatio(CCSprite* src, CCSize targetSize);
    CCImage* cropRegionOfTextureToImageBySizeRatio(CCTexture2D* srcTexture, CCSize targetSize);
    CCSprite* getSpriteFromImage(CCImage* pCCImage);
    void addNodeByCoordinateRatio(CCNode* canvas, CCNode* node, CCSize anchorRatio);
    string filenameFromUrlAndDate(const string& url, const string& date);
    
    bool initQuotaFile(const int& numDayPasses);
    bool readQuotaFile(int& tipsQuota, int& solverQuota);
    bool writeQuotaFile(int& tipsQuota, int& solverQuota);
    
    bool isLeapYear(int year);
    __int64_t getDayFromOrigin(string date);
    __int64_t getDayDiffBetweenDates(string date1, string date2);
    bool isInNameMap(CCString* url);
    CCAction* buildLoadingAction();
    
private:
    // callback functions
    
private:
    string _quotaFilePath;
    float _nLoadingActionFrames;
    float _loadingActionCycle;
};

#endif /* defined(__Puzzle__ImageUtils__) */
