#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Constants.h"

HelloWorld::HelloWorld(){
    _bLoadListBufferFinished=false;
    _numberOfPhotosPerPage=3;
    _numberOfPages=3;
    _pScorll=NULL;
    _urlDict=NULL;
    _pageDict=NULL;
    _jsonReader=NULL;
}

HelloWorld::~HelloWorld(){
    CC_SAFE_RELEASE_NULL(_pScorll);
    CC_SAFE_RELEASE_NULL(_urlDict);
    CC_SAFE_RELEASE_NULL(_pageDict);
}

void HelloWorld::onEnter(){
    CCLayer::onEnter();
    
    _jsonReader = JSONReader::create(this, callfuncO_selector(HelloWorld::JSONCallback));
    CC_SAFE_RETAIN(_jsonReader);
}

void HelloWorld::onExit(){
    this->removeAllChildrenWithCleanup(true);
    CCLayer::onExit();
}

void HelloWorld::scrollViewDidScroll(cocos2d::extension::CCScrollView *view){
    CCPoint offset = view->getContentOffset();
    CCLOG("%.2lf", offset.y);
}

void HelloWorld::scrollViewDidZoom(cocos2d::extension::CCScrollView *view){
    
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    
    _pScorll = CCScrollView::create(CCSize(size.width, size.height));
    _pScorll->setContentSize(CCSize(size.width, 3*size.height));
    _pScorll->setAnchorPoint(CCPointZero);
    _pScorll->setDirection(kCCScrollViewDirectionVertical);
    _pScorll->setDelegate(this);
    
    this->addChild(_pScorll);
    
    return true;
}

HelloWorld* HelloWorld::create(){
    HelloWorld* instance = new HelloWorld();
    if(instance && instance->init()){
        instance->autorelease(); // add to autorelease pool
        return instance;
    }
    delete instance;
    instance = NULL;
    return NULL;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::addPage(int pageOrder){
    intptr_t key = pageOrder;
    CCArray* urls = (CCArray*)(_urlDict->objectForKey(key));
    CCSize pageSize = CCSize(_pScorll->getViewSize().width, _pScorll->getViewSize().height);
    SubViewPage* page = SubViewPage::create(this, urls, pageSize);
    CCPoint position = ccp(0, _pScorll->getContentSize().height-(pageOrder+1)*pageSize.height);
    page->setPosition(position);
    _pageDict->setObject(page, key);
    _pScorll->addChild(page);
}

void HelloWorld::loadListBuffer(){
    do{
        CC_BREAK_IF(!this->isRunning());
        
        _urlDict = CCDictionary::create();
        CC_SAFE_RETAIN(_urlDict);
        
        string path = kFeedListPath;
        FILE* fp=fopen(path.c_str(),"r");
        if(fp){
            
            char buf[kDefaultBufferSize];
            memset(buf,0,sizeof(buf));
        
            int limit = _numberOfPhotosPerPage*_numberOfPages;
            int count = 0;
            while( count<limit ){
        
                CCArray* urls = CCArray::create();
                
                for(int i=0;i<_numberOfPhotosPerPage;++i){
                    
                    char* fRet=NULL;
                    // the first line is date
                    memset(buf,0,sizeof(buf));
                    fRet=fgets(buf,1024,fp);
                    // the second line is url
                    memset(buf,0,sizeof(buf));
                    fRet=fgets(buf,1024,fp);
                    
                    std::string url(buf);
                    url.erase(url.size()-1); // erase the new line token
                    
                    // check if the url contains valid image formats
                    if(containsImage(url)){
                        CCString* str = CCString::create(url);
                        urls->addObject(str);
                        ++count;
                    }
                }
                intptr_t key = (count-1)/_numberOfPhotosPerPage;
                _urlDict->setObject(urls, key);
                
            }
            CCLog("Save to list buffer finished:");
        }
        else{
            CCLog("Read file %s error.", path.c_str());
        }
        fclose(fp);
    
        _pScorll->setContentOffset(_pScorll->minContainerOffset());
        
        _pageDict = CCDictionary::create();
        CC_SAFE_RETAIN(_pageDict);
        // add pages
        for(int pageOrder = 0; pageOrder < _numberOfPages; ++pageOrder){
            // addPageOfDate will add a CCLayer both to the scroll and _pagesDict
            addPage(pageOrder);
        }
        
        // preloading
        for(int pageOrder = 0; pageOrder<_numberOfPages ; ++pageOrder){
            intptr_t key = pageOrder;
            SubViewPage* pPage= (SubViewPage*) _pageDict->objectForKey(key);
            if(pPage){
                pPage->downloadAll();
            }else{
                CCLOG("Page not found !");
            }
        }
        
        _bLoadListBufferFinished = true;
        
    }while(false);
}

void HelloWorld::JSONCallback(cocos2d::CCObject* pSender){
    CC_SAFE_RELEASE_NULL(_jsonReader);
    loadListBuffer();
}