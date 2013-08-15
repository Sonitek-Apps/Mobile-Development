//
//  PageLayer.h
//  ScrollView
//
//  Created by Wing on 10/8/13.
//
//

#ifndef __ScrollView__PageLayer__
#define __ScrollView__PageLayer__

#include "ImageUtils.h"
#include <iostream>
class PageLayer: public CCLayer, public ImageUtils
{
public:
    PageLayer();
    virtual ~PageLayer();
    virtual bool init();
    static PageLayer* create();
    
public:
    void downloadImageToNode(string& url, CCNode* node, string& saveMask);
    
private:
    // call back functions
    void onImageDownloadFinished(CCNode* node, void* obj);

private:
    CCArray* _items;
    CCMenu* _menu;
    string _pageId;
};
#endif /* defined(__ScrollView__PageLayer__) */
