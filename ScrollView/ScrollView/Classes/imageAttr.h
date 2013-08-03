//
//  imageAttr.h
//  ScrollView
//
//  Created by Wing on 3/8/13.
//
//

#ifndef ScrollView_imageAttr_h
#define ScrollView_imageAttr_h

#include <string>
using namespace std;

struct imageAttr{
    
    int posInQueue;
    string url;
    
    imageAttr(){
        posInQueue=-1;
        url="\0";
    }
    
    imageAttr(int posInQueue_t, string url_t){
        posInQueue = posInQueue_t;
        url=url_t;
    }
};

#endif
