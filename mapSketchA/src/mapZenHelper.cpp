#include "mapZenHelper.h"

//TERRARIUM with extention png in Web Mercator projection, TERRARIUMxTERRARIUM tiles
#define TERRARIUM 256


mapZenLoader::mapZenLoader() {
    nextID = 0;
    ofAddListener(ofEvents().update, this, &mapZenLoader::update);
    ofAddListener(ofURLResponseEvent(), this, &mapZenLoader::urlResponse);
    startThread();
    lastUpdate = 0;
    maps.clear();
    mapImages.clear();
    
    terrariumUrl = "http://tile.mapzen.com/mapzen/terrain/v1/terrarium/";
    normalUrl = "https://tile.mapzen.com/mapzen/terrain/v1/normal/";
}

void mapZenLoader::prepareContainer(int size_){
    maps.resize(size_);
}

void mapZenLoader::createMapImage(float lat_, float lon_, int zoom_, int width_, int heigth_, string type_, int iterator_) {
    mapZenBlock temp;
    if (type_ == "Terrarium") {
        temp.mapImage.allocate((width_ / TERRARIUM) * TERRARIUM, (heigth_ / TERRARIUM) * TERRARIUM, OF_IMAGE_COLOR);
    }
    else if (type_ == "Normal") {
        temp.mapImage.allocate((width_ / TERRARIUM) * TERRARIUM, (heigth_ / TERRARIUM) * TERRARIUM, OF_IMAGE_COLOR_ALPHA);
    }
    temp.tiles.resize((width_ / TERRARIUM) * (heigth_ / TERRARIUM));
    temp.pos = to3857(ofVec2f(lat_, lon_), zoom_);
    temp.zoom = zoom_;
    temp.channels = temp.mapImage.getImageType()+2;
    cout << temp.pos << endl;
    maps.at(iterator_) = temp;
    mapImages.clear();
    for (int i = 0; i < maps.size(); i++) {
        mapImages.push_back(&maps.at(i).mapImage);
    }
    
    loadTiles(iterator_, type_);
}

void mapZenLoader::loadTiles(int index_, string type_) {
    string loadUrl;
    if (type_ == "Terrarium") {
        loadUrl = terrariumUrl;
    }
    else if (type_ == "Normal") {
        loadUrl = normalUrl;
    }
    
    int w = maps.at(index_).mapImage.getWidth();
    int h = maps.at(index_).mapImage.getHeight();
    for (int x = 0; x < w / TERRARIUM; x++) {
        for (int y = 0; y < h / TERRARIUM; y++) {
            loadFromURL(ofVec2f(x, y),
                        maps.at(index_).tiles[(y* (w / TERRARIUM)) + x],
                        loadUrl +
                        ofToString(maps.at(index_).zoom) + "/" +
                        ofToString(maps.at(index_).pos.x + x) + "/" +
                        ofToString(maps.at(index_).pos.y + y) +
                        ".png?api_key="+MZKEY, index_);
            //insert your key ;)
        }
    }
}

void mapZenLoader::addTile(ofImageLoaderEntry& entry_) {
    int channels = maps.at(entry_.index).channels;
    unsigned char *pixelData = maps.at(entry_.index).mapImage.getPixels().getData();
    int imgR, imgG, imgB, imgA, imgArr;
    int pixSize = TERRARIUM * TERRARIUM * channels;
    
    imgArr = (pixSize*(int(maps.at(entry_.index).mapImage.getWidth() / TERRARIUM)*entry_.pos.y)) + ((TERRARIUM * channels)*entry_.pos.x);
    
    unsigned char *tileData = entry_.image->getPixels().getData();
    for (int i = 0; i < TERRARIUM; i++) {
        for (int j = 0; j < TERRARIUM; j++) {
            imgR = (j * TERRARIUM * channels) + (i * channels);
            imgG = imgR + 1;
            imgB = imgR + 2;
            imgA = imgR + 3;
            
            int offsetRow = imgR % (TERRARIUM * channels);
            int offsetCol = (imgR / (TERRARIUM * channels))*(maps.at(entry_.index).mapImage.getWidth() * channels);
            pixelData[imgArr + offsetCol + offsetRow] = tileData[imgR];
            pixelData[imgArr + offsetCol + offsetRow + 1] = tileData[imgG];
            pixelData[imgArr + offsetCol + offsetRow + 2] = tileData[imgB];
            if (channels == 4) {
                pixelData[imgArr + offsetCol + offsetRow + 3] = tileData[imgA];
            }
        }
    }
    maps.at(entry_.index).mapImage.update();
}

ofVec2f mapZenLoader::to3857(ofVec2f in_, float zoom_) {
    ofVec2f temp;
    
    temp.x = floor((in_.y + 180) / 360 * pow(2, zoom_));
    temp.y = floor((1 - log(tan(in_.x*PI / 180) + 1 / cos(in_.x*PI / 180)) / PI) / 2 * pow(2, zoom_));
    
    return temp;
}

vector<ofImage*>* mapZenLoader::getMapImagesPtr() {
    return &mapImages;
}


mapZenLoader::~mapZenLoader() {
    images_to_load_from_disk.close();
    images_to_update.close();
    waitForThread(true);
    ofRemoveListener(ofEvents().update, this, &mapZenLoader::update);
    ofRemoveListener(ofURLResponseEvent(), this, &mapZenLoader::urlResponse);
}



// Load an url asynchronously from an url.
//--------------------------------------------------------------
void mapZenLoader::loadFromURL(ofVec2f pos, ofImage& image, string url, int index_) {
    nextID++;
    ofImageLoaderEntry entry(image);
    entry.url = url;
    entry.image->setUseTexture(false);
    entry.name = "image" + ofToString(nextID);
    entry.pos = pos;
    entry.index = index_;
    images_async_loading[entry.name] = entry;
    ofLoadURLAsync(entry.url, entry.name);
}


// Reads from the queue and loads new images.
//--------------------------------------------------------------
void mapZenLoader::threadedFunction() {
    thread.setName("mapZenLoader " + thread.name());
    ofImageLoaderEntry entry;
    while (images_to_load_from_disk.receive(entry)) {
        if (entry.image->load(entry.filename)) {
            images_to_update.send(entry);
        }
        else {
            ofLogError("mapZenLoader") << "couldn't load file: \"" << entry.filename << "\"";
        }
    }
    ofLogVerbose("mapZenLoader") << "finishing thread on closed queue";
}


// When we receive an url response this method is called;
// The loaded image is removed from the async_queue and added to the
// update queue. The update queue is used to update the texture.
//--------------------------------------------------------------
void mapZenLoader::urlResponse(ofHttpResponse & response) {
    // this happens in the update thread so no need to lock to access
    // images_async_loading
    entry_iterator it = images_async_loading.find(response.request.name);
    if (response.status == 200) {
        if (it != images_async_loading.end()) {
            it->second.image->load(response.data);
            images_to_update.send(it->second);
        }
    }
    else {
        // log error.
        ofLogError("mapZenLoader") << "couldn't load url, response status: " << response.status;
        ofRemoveURLRequest(response.request.getID());
    }
    
    // remove the entry from the queue
    if (it != images_async_loading.end()) {
        images_async_loading.erase(it);
    }
}


// Check the update queue and update the texture
//--------------------------------------------------------------
void mapZenLoader::update(ofEventArgs & a) {
    // Load 1 image per update so we don't block the gl thread for too long
    ofImageLoaderEntry entry;
    if (images_to_update.tryReceive(entry)) {
        //entry.image->setUseTexture(true);
        //entry.image->update();
        addTile(entry);
    }
    
}
