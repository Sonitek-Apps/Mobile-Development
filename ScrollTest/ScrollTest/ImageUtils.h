//
//  ImageUtils.h
//  Puzzle
//
//  Created by Tact Sky on 8/8/13.
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
#include "Constants.h"
#include "dirent.h"
using namespace std;
USING_NS_CC;

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
    
    bool isLeapYear(int year);
    __int64_t getDayFromOrigin(string date);
    __int64_t getDayDiffBetweenDates(string date1, string date2);
    bool isInNameMap(CCString* url);
    CCAction* buildLoadingAction();
    bool containsImage(string& url);
    
private:
    // callback functions
    
private:
    string _quotaFilePath;
    float _nLoadingActionFrames;
    float _loadingActionCycle;
};

#endif /* defined(__Puzzle__ImageUtils__) */
