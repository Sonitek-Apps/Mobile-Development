#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CCScrollView.h"
#include "SubViewPage.h"
#include "ImageUtils.h"
#include "JSONReader.h"
USING_NS_CC;
USING_NS_CC_EXT;

class HelloWorld : public cocos2d::CCLayer, public CCScrollViewDelegate, virtual public ImageUtils
{
public:
    
    HelloWorld();
    virtual ~HelloWorld();
    virtual void onEnter();
    virtual void onExit();
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    static HelloWorld* create();
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();

private:
    void menuCloseCallback(CCObject* pSender);
    void JSONCallback(CCObject*);
    
public:
    void addPage(int pageOrder);
    void loadListBuffer();
private:
    bool _bLoadListBufferFinished;
    int _numberOfPhotosPerPage;
    int _numberOfPages;
    CCScrollView* _pScroll;
    CCDictionary* _urlDict;
    CCDictionary* _pageDict;
    JSONReader* _jsonReader;
    
    float _previousOffsetY;
    bool _scrollExceedingTop;
    bool _scrollExceedingBottom;
    bool _scrollBouncingFromTop;
    bool _scrollBouncingFromBottom;
    
    CCSize _sizeScrollPage;
};

#endif // __HELLOWORLD_SCENE_H__
