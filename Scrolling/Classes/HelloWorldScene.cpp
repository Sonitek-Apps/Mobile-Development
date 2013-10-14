#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Constants.h"
#include "math.h"

HelloWorld::HelloWorld(){
    _bLoadListBufferFinished=false;
    _numberOfPhotosPerPage=3;
    _numberOfPages=500;
    _pScroll=NULL;
    _urlDict=NULL;
    _pageDict=NULL;
    _jsonReader=NULL;
    _previousOffsetY=0;
    _scrollExceedingTop=false;
    _scrollExceedingBottom=false;
    _scrollBouncingFromTop=false;
    _scrollBouncingFromBottom=false;
    _sizeScrollPage=CCSize(0,0);
    _pageLabel=NULL;
    _previousPageNumber=0;
    _currentPageNumber=0;
}

HelloWorld::~HelloWorld(){
    CC_SAFE_RELEASE_NULL(_pScroll);
    CC_SAFE_RELEASE_NULL(_urlDict);
    CC_SAFE_RELEASE_NULL(_pageDict);
}

void HelloWorld::onEnter(){
    CCLayer::onEnter();
    CCLOG("HelloWorld::onEnter");
    _jsonReader = JSONReader::create(this, callfuncO_selector(HelloWorld::JSONCallback));
    CC_SAFE_RETAIN(_jsonReader);
}

void HelloWorld::onExit(){
    this->removeAllChildrenWithCleanup(true);
    CCLayer::onExit();
}

void HelloWorld::scrollViewDidScroll(cocos2d::extension::CCScrollView *view){
    CCPoint offset = view->getContentOffset();
    
    if(offset.y<view->minContainerOffset().y){
        // exceeding or bouncing back states
        if(_previousOffsetY>=offset.y){
            // exceeding top
            _scrollExceedingTop=true; _scrollExceedingBottom=false;
            _scrollBouncingFromTop=false; _scrollBouncingFromBottom=false;
            
            CCLog("Exceeding Top!");
        }
        else{
            // bouncing from top
            _scrollExceedingTop=false; _scrollExceedingBottom=false;
            _scrollBouncingFromTop=true; _scrollBouncingFromBottom=false;
            CCLog("Bouncing from Top!");
        }
    }
    
    else if(offset.y>view->maxContainerOffset().y){
        // exceeding or bouncing back states
        if(_previousOffsetY<=offset.y){
            // exceeding bottom
            _scrollExceedingTop=false; _scrollExceedingBottom=true;
            _scrollBouncingFromTop=false; _scrollBouncingFromBottom=false;
            CCLog("Exceeding Bottom!");
        }
        else{
            // bouncing from bottom
            _scrollExceedingTop=false; _scrollExceedingBottom=false;
            _scrollBouncingFromTop=false; _scrollBouncingFromBottom=true;
            CCLog("Bouncing from Bottom");
        }
    }
    else{
        // returning to normal
        if(_scrollBouncingFromTop){
            _scrollBouncingFromTop=false;
            CCLog("Returned by bouncing from top");
        }
        else if(_scrollBouncingFromBottom){
            _scrollBouncingFromBottom=false;
            CCLog("Returned by bouncing from bottom");
        }
        // normal
        else{
            if(_bLoadListBufferFinished){
                
                int currentPage = (int)floor((offset.y - view->minContainerOffset().y)/_sizeScrollPage.height);
                int previousPage = (int)floor((_previousOffsetY - view->minContainerOffset().y)/_sizeScrollPage.height);
                
                if ( currentPage==previousPage ) {
                    return;
                }
                _previousPageNumber=previousPage;
                _currentPageNumber=currentPage;
                
                int cleanOrder[]={currentPage-2, currentPage+2};
                int addOrder[]={currentPage+1, currentPage-1, currentPage};
                int cleanOrderLength=sizeof(cleanOrder)/sizeof(int);
                int addOrderLength=sizeof(addOrder)/sizeof(int);
                
                for(int i=0;i<cleanOrderLength;++i){
                    bool bPageAvailable = (cleanOrder[i]>=0 && cleanOrder[i]<_numberOfPages);
                    if(bPageAvailable){
                        intptr_t key = cleanOrder[i];
                        SubViewPage* page = (SubViewPage*)_pageDict->objectForKey(key);
                        if(page){
                            page->cleanupNodes();
                        } else{
                            CCLOG("Page not found!");
                        }
                    }
                }
                
                for(int i=0;i<addOrderLength;++i){
                    bool bPageAvailable = (addOrder[i]>=0 && addOrder[i]<_numberOfPages);
                    if(bPageAvailable){
                        intptr_t key = addOrder[i];
                        SubViewPage* page = (SubViewPage*)_pageDict->objectForKey(key);
                        if(page){
                            page->downloadAll();
                        }else{
                            CCLOG("Page not found!");
                        }
                    }
                }
                
                CCLog("Currently at page# %d", currentPage);
            }
        }
        _previousOffsetY=offset.y; //  store the previous position of the scroll offset
    }
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

    CCSize size=CCDirector::sharedDirector()->getWinSize();
    _sizeScrollPage.setSize( size.width, size.height*0.5);
    _pScroll = CCScrollView::create(CCSize(size.width, size.height));
    _pScroll->setContentSize(CCSize(size.width, _numberOfPages*_sizeScrollPage.height));
    _pScroll->setAnchorPoint(CCPointZero);
    _pScroll->setDirection(kCCScrollViewDirectionVertical);
    _pScroll->setDelegate(this);

    this->addChild(_pScroll);
    
    _pageLabel=CCLabelTTF::create("page 0", "Georgia", 24);
    _pageLabel->setColor(ccRED);
    _pageLabel->setPosition(ccp(size.width*0.1,size.height*0.9));
    this->addChild(_pageLabel, 128);
    
    scheduleUpdate();
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
    CCSize pageSize = _sizeScrollPage;
    SubViewPage* page = SubViewPage::create(this, urls, pageSize);
    CCPoint position = ccp(0, _pScroll->getContentSize().height-(pageOrder+1)*pageSize.height);
    page->setPosition(position);
    _pageDict->setObject(page, key);
    _pScroll->addChild(page,this->getZOrder()+1);
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
    
        _pScroll->setContentOffset(_pScroll->minContainerOffset());
        _previousOffsetY = _pScroll->minContainerOffset().y;
        _pageDict = CCDictionary::create();
        CC_SAFE_RETAIN(_pageDict);
        
        int numberOfPreloadPages=3;
        // add pages
        for(int pageOrder = 0; pageOrder < _numberOfPages; ++pageOrder){
            // addPageOfDate will add a CCLayer both to the scroll and _pagesDict
            addPage(pageOrder);
        }
        
        // preloading
        for(int pageOrder = 0; pageOrder<numberOfPreloadPages ; ++pageOrder){
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

void HelloWorld::update(float dt){
    CCString* pageLabelStr=CCString::createWithFormat("page %d", _currentPageNumber);
    _pageLabel->setString(pageLabelStr->getCString());
}