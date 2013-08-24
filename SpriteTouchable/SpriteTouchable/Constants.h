//
//  Constants.h
//  SpriteTouchable
//
//  Created by Wing on 25/8/13.
//
//

#ifndef SpriteTouchable_Constants_h
#define SpriteTouchable_Constants_h

#define kFeatureIDOfSingleDayPack 1
#define kFeatureIDOfTenDaysPack 2
#define kFeatureIDOfForeverPass 3

#define kNumDayPasses 1
#define kTipsQuota 2
#define kSolverQuota 3

#define kNumberOfPurchasedCoin1 1
#define kNumberOfPurchasedCoin2 10

#define kBuySingleDayPackMessage (2048+1)
#define kBuyTenDaysPackMessage (kBuySingleDayPackMessage+1)
#define kBuyForeverPassMessage (kBuyTenDaysPackMessage+1)

#define kStringForSingleDayPack "tomlinsonpuzzle_dailypass_1day"
#define kStringForTenDaysPack "tomlinsonpuzzle_dailypass_10days"
#define kStringForForeverPass "tomlinsonpuzzle_dailypass_forever"

#define entryFree 1
#define entryFullPack 2

#define xmlKeyNumberOfLocalImages "numberOfLocalImages"
#define sharedSaveFile CCUserDefault::sharedUserDefault()
#define nameMapPrefix "$%$#$-"
#define defaultImagePrefix "default-"
#define getMoreImageName "get more.png"
#define httpPreix "http"
#define numberOfDefaultPhotos 6
#define downloadedPhotosListPath (CCFileUtils::sharedFileUtils()->getWritablePath().append("downloadedPhotosList")).c_str()

#define defaultBufferSize 1024
#define notEnoughQuotaMessage -65535

#define homeSceneName "HOME"
#define dailySceneName "DAILY"
#define previewSceneName "PREVIEW"
#define patternSelectionSceneName "PATTERSELECTION"
#define gamePlaySceneName "GAMEPLAY"
#define gameCompleteSceneName "GAMECOMPLETE"

static int nImageTypes=6;
static char* imageSuffix[] = {".jpg",".png",".jpeg",".JPG",".PNG",".JPEG"};
static int secPerDay=86400;
#endif
