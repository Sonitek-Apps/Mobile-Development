//
//  imageIndex.h
//  ScrollView
//
//  Created by Wing on 2/8/13.
//
//

#include <string>
using namespace std;

#ifndef ScrollView_imageIndex_h
#define ScrollView_imageIndex_h

struct imageIndex{
    string date;
    int withinGroupIndex;
    
    string imageId;
    
    imageIndex(string date_t, int withinGroupIndex_t){
        date=date_t;
        withinGroupIndex=withinGroupIndex_t;
        char buf[256]; sprintf(buf, "%04d", withinGroupIndex);
        imageId = date; imageId.append("-");
        imageId.append(string(buf));
    }
    
    imageIndex(string indicatorString){
        date = indicatorString.substr(0,10);
        imageId = indicatorString.substr(0,15);
        withinGroupIndex = atoi(indicatorString.substr(11,4).c_str());
    }
    
    bool operator<(const imageIndex& obj) const{
        if(date!=obj.date) return date>obj.date;
        else return withinGroupIndex<obj.withinGroupIndex;
    }

    bool operator>(const imageIndex& obj) const{
        if(date!=obj.date) return date<obj.date;
        else return withinGroupIndex>obj.withinGroupIndex;
    }
        
    bool operator==(const imageIndex& obj) const{
        return (imageId==obj.imageId);
    }
        
    string toString(){
        return imageId;
    }
};

#endif
