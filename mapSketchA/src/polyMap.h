#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"

struct MapLevel {
    MapLevel() {
        lines.clear();
        height = 0;
    }
    vector<ofPath> lines;
    int height;
    ofImage levelImg;
};
class PolyMap {
    
public:
    PolyMap();
    void loadMap(ofImage &, int);
    vector<MapLevel> levels;
    
private:
    ofxCvContourFinder contourFinder;
    ofxCvColorImage cvColorImg;
    ofxCvGrayscaleImage cvGrayImg;
    void convertBW(ofImage &);
};
