/*Very basic helper addon for mapzen data, loads just terrain tiles at the
 * moment*/

#pragma once
#include "ofMain.h"
#define MZKEY "your api"
using namespace std;

class mapZenLoader : public ofThread {
    
public:
    mapZenLoader();
    ~mapZenLoader();
    
    void createMapImage(float, float, int, int, int, string, int);
    void prepareContainer(int);
    vector<ofImage *> *getMapImagesPtr();
    
private:
    // Entry to load.
    struct ofImageLoaderEntry {
        ofImageLoaderEntry() { image = NULL; }
        ofImageLoaderEntry(ofImage &pImage) { image = &pImage; }
        ofImage *image;
        string filename;
        string url;
        string name;
        ofVec2f pos;
        int index;
    };
    
    // mapZenData
    struct mapZenBlock {
        ofImage mapImage;
        vector<ofImage> tiles;
        ofVec2f pos;
        int zoom;
        int channels;
    };
    
    void loadTiles(int, string);
    void addTile(ofImageLoaderEntry &tile);
    
    vector<mapZenBlock> maps;
    vector<ofImage *> mapImages;
    ofVec2f to3857(ofVec2f, float);
    string terrariumUrl, normalUrl;
    
    // ofxThreadedImageLoader
    void loadFromURL(ofVec2f pos, ofImage &image, string url, int index);
    void update(ofEventArgs &a);
    virtual void threadedFunction();
    void urlResponse(ofHttpResponse &response);
    typedef map<string, ofImageLoaderEntry>::iterator entry_iterator;
    int nextID;
    int lastUpdate;
    map<string, ofImageLoaderEntry>
    images_async_loading; // keeps track of images which are loading async
    ofThreadChannel<ofImageLoaderEntry> images_to_load_from_disk;
    ofThreadChannel<ofImageLoaderEntry> images_to_update;
};
