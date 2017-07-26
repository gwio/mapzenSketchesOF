#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    drawGui = true;
    // load shader files to gui
    dir.allowExt("frag");
    dir.listDir("");
    dir.sort();
    shaderFiles.resize(dir.size());
    shaderfile = "shaderA.frag";
    
    ofBackground(0, 0, 0);
    ofSetVerticalSync(false);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    // latitude, longitude, zoomfactor (1-15 for mapzen?), image size, type
    // ("Terrarium", "Normal") -image size always a factor of 256  allocates
    // ofImage, updates maptiles while loading
    mapZen.prepareContainer(4);
    mapZen.createMapImage(47.126298, 10.398560, 10.5, 256 * 4, 256 * 4,
                          "Terrarium",0);
    mapZen.createMapImage(47.126298, 10.398560, 10.5, 256 * 4, 256 * 4, "Normal",1);
    
    mapZen.createMapImage(47.126298, 10.398560, 10.5, 256 * 4, 256 * 4,
                          "Terrarium",2);
    mapZen.createMapImage(47.126298, 10.398560, 10.5, 256 * 4, 256 * 4, "Normal",3);
    
    // mapZen.createMapImage(67.397989, -148.842773, 11.8, 256 * 1, 256 * 1,
    // "Normal");
    /*Terrarium format PNG tiles contain raw elevation data in meters,
     in Web Mercator projection (EPSG:3857).
     All values are positive with a 32,768 offset, split into the red, green, and
     blue channels, with 16 bits of integer and 8 bits of fraction.
     
     To decode:
     (red * 256 + green + blue / 256) - 32768
     
     ------
     
     Normal format PNG tiles are processed elevation data with the the red, green,
     and blue values corresponding to the direction the pixel �surface� is facing
     (its XYZ vector), in Web Mercator projection (EPSG:3857). The alpha channel
     contains quantized elevation data with values suitable for common hypsometric
     tint ranges.
     
     red = x vector
     green = y vector
     blue = z vector
     alpha = quantized elevation data
     */
    shader.load("shader.vert", shaderfile);
    
    // glShadeModel(GL_FLAT);
    
    gui.setup();
    convertMap.addListener(this, &ofApp::convertMapImg);
    gui.add(convertMap.setup("convert Image"));
    gui.add(lightX.setup("lightPos x", 1.0, -100.0, 100.0));
    gui.add(lightY.setup("lightPos y", 1.0, -100.0, 100.0));
    gui.add(lightZ.setup("lightPos z", 1.0, -100.0, 100.0));
    
    gui.add(noiseA.setup("noiseA", 0.0005, 0.0001, 0.001));
    gui.add(noiseB.setup("noiseB", 0.35, 0.0001, 0.5));
    
    gui.add(colorR.setup("shadow color R", 1.0, 0.0, 2.0));
    gui.add(colorG.setup("shadow color G", 1.0, 0.0, 2.0));
    gui.add(colorB.setup("shadow color B", 1.0, 0.0, 2.0));
    
    gui.add(clightR.setup("light color R", 1.0, 0.0, 4.0));
    gui.add(clightG.setup("light color G", 1.0, 0.0, 4.0));
    gui.add(clightB.setup("light color B", 1.0, 0.0, 4.0));
    
    gui.add(camFov.setup("Fov", 60, 5.0, 110));
    gui.add(camRatio.setup("camRatio", 0.5, 0.0, 1.0));
    
    gui.add(linesAmt.setup("linesAmt", 10, 0, 255));
    
    for (int i = 0; i < dir.size(); i++) {
        gui.add(shaderFiles.at(i).setup(dir.getName(i)));
        shaderFiles.at(i).addListener(this, &ofApp::loadShader);
    }
    
    light.z = 1;
    
    plane.set(256 * 4, 256 * 4, 222, 222, OF_PRIMITIVE_TRIANGLES);
    plane.mapTexCoordsFromTexture(
                                  mapZen.getMapImagesPtr()->back()->getTextureReference());
    
    lines.allocate(256 * 4, 256 * 4, GL_RGBA, 8);
    
    cam.setPosition(0, 1600, -800);
    cam.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::update() {
    
    // light.x = sin(ofGetElapsedTimef() / 2) * 100;
    // light.y = cos(ofGetElapsedTimef() / 2) * 100;
    
    light.x = lightX;
    light.y = lightY;
    light.z = lightZ;
    
    sendlight = ofVec3f(0, 0, 0) - light;
    sendlight.normalize();
    
    if (ofGetFrameNum() % 300 == 0) {
        shader.load("shader.vert", shaderfile);
    }
    
    cam.setFov(camFov);
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    ofBackground(ofColor::white);
    
    for (int i = 0; i < mapZen.getMapImagesPtr()->size(); i++) {
        // shader.setUniformTexture("tex" + ofToString(i),
        // mapZen.getMapImagesPtr()->at(i)->getTextureReference(), i + 1);
        // mapZen.getMapImagesPtr()->at(i)->draw(0, 0);
    }
    
    if (drawGui)
        ofDrawBitmapString(ofGetFrameRate(), 20, 20);
    
    ofSetColor(ofColor::antiqueWhite);
    
    glEnable(GL_DEPTH_TEST);
    cam.begin();
    // ofDrawAxis(800);
    shader.begin();
    
    shader.setUniform3f("lightPos", sendlight);
    shader.setUniform3f("shadowColor", ofVec3f(colorR, colorG, colorB));
    shader.setUniform3f("lightColor", ofVec3f(clightR, clightG, clightB));
    shader.setUniform1f("time", ofGetElapsedTimef());
    shader.setUniform1f("noiseA", noiseA);
    shader.setUniform1f("noiseB", noiseB);
    shader.setUniform2f("mousePos", ofGetMouseX(), ofGetMouseY());
    shader.setUniformTexture(
                             "map1", mapZen.getMapImagesPtr()->at(0)->getTextureReference(), 0);
    shader.setUniformTexture(
                             "nor1", mapZen.getMapImagesPtr()->at(1)->getTextureReference(), 1);
    
    shader.setUniformTexture(
                             "map2", mapZen.getMapImagesPtr()->at(2)->getTextureReference(), 2);
    shader.setUniformTexture(
                             "nor2", mapZen.getMapImagesPtr()->at(3)->getTextureReference(), 3);
    
    shader.setUniformTexture("lines", lines.getTextureReference(), 4);
    
    // lineMesh.draw();
    
    for (int i = 0; i < mapCurves.size(); i++) {
        // mapCurves.at(i).draw();
    }
    ofRotateZ(ofGetElapsedTimef() * 8);
    plane.draw();
    
    /*
     for (int i = 0; i < polyMap.levels.size(); i++) {
     for (int j = 0; j < polyMap.levels.at(i).lines.size(); j++) {
     //ofSetColor(ofColor(255*polyMap.levels.at(i).height));
     ofPushMatrix();
     ofTranslate(0, 0, (255/polyMap.levels.size()) * i);
     ofPath& thisLine = polyMap.levels.at(i).lines.at(j);
     //thisLine.setColor(ofColor(i * 10));
     thisLine.draw();
     ofPopMatrix();
     }
     }
     */
    shader.end();
    cam.end();
    
    glDisable(GL_DEPTH_TEST);
    if (drawGui)
        gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    
    // press a key after loading to convert data to b&w
    
    if (key == 'c') {
        polyMap.loadMap(*mapZen.getMapImagesPtr()->at(0), linesAmt);
        polyMap.loadMap(*mapZen.getMapImagesPtr()->at(2), linesAmt);
        
        // polyMap.loadMap(*mapZen.getMapImagesPtr()->at(1), linesAmt);
        
        lineMesh.clear();
        lineMesh.setMode(OF_PRIMITIVE_LINES);
        int stepI = 10;
        int stepJ = 10;
        for (int i = 0; i < mapZen.getMapImagesPtr()->at(0)->getHeight();
             i += stepI) {
            ofPath tempP;
            
            for (int j = 0; j < mapZen.getMapImagesPtr()->at(0)->getWidth() - stepJ;
                 j += stepJ) {
                float hmapA = mapZen.getMapImagesPtr()->at(0)->getColor(j, i).r;
                float hmapB = mapZen.getMapImagesPtr()->at(0)->getColor(j + stepJ, i).r;
                ofVec3f trans = ofVec3f(-256 * 3, -256 * 3, 0);
                ofVec3f tempVecA = ofVec3f(j, i, hmapA);
                ofVec3f tempVecB = ofVec3f(j + stepJ, i, hmapB);
                
                //	ofVec3f tempVecA = ofVec3f(j, (i)+hmapA,0);
                //	ofVec3f tempVecB = ofVec3f(j + stepJ, (i)+hmapB, 0);
                lineMesh.addVertex(tempVecA + trans);
                lineMesh.addVertex(tempVecB + trans);
                lineMesh.addColor(ofColor(0, 0, 0, 150));
                lineMesh.addColor(ofColor(0, 0, 0, 150));
                
                tempP.curveTo(tempVecA + trans);
            }
            tempP.setFilled(false);
            tempP.setStrokeColor(ofColor(ofColor(0, 0, 0, 100)));
            tempP.setStrokeWidth(2.50);
            
            mapCurves.push_back(tempP);
        }
    }
    
    if (key == 'f') {
        ofToggleFullscreen();
    }
    
    if (key == 's') {
        
        ofFbo printImg;
        printImg.allocate(600, 600, GL_RGBA, 4);
        
        printImg.begin();
        ofClear(0, 0, 0, 255);
        
        ofBackground(ofColor::white);
        
        glEnable(GL_DEPTH_TEST);
        
        glEnable(GL_MULTISAMPLE);
        cam.begin();
        shader.begin();
        
        shader.setUniform3f("lightPos", sendlight);
        shader.setUniform3f("shadowColor", ofVec3f(colorR, colorG, colorB));
        shader.setUniform3f("lightColor", ofVec3f(clightR, clightG, clightB));
        shader.setUniform1f("time", ofGetElapsedTimef());
        shader.setUniform1f("noiseA", noiseA);
        shader.setUniform1f("noiseB", noiseB);
        shader.setUniformTexture(
                                 "map1", mapZen.getMapImagesPtr()->at(0)->getTextureReference(), 0);
        shader.setUniformTexture(
                                 "nor1", mapZen.getMapImagesPtr()->at(1)->getTextureReference(), 1);
        shader.setUniformTexture(
                                 "map2", mapZen.getMapImagesPtr()->at(2)->getTextureReference(), 2);
        shader.setUniformTexture(
                                 "nor2", mapZen.getMapImagesPtr()->at(3)->getTextureReference(), 3);
        shader.setUniformTexture("lines", lines.getTextureReference(), 4);
        
        plane.draw();
        
        shader.end();
        cam.end();
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_MULTISAMPLE);
        
        printImg.end();
        
        ofImage pix;
        pix.allocate(printImg.getWidth(), printImg.getHeight(),
                     OF_IMAGE_COLOR_ALPHA);
        printImg.readToPixels(pix);
        
        ofImage pix2;
        pix2.allocate(printImg.getWidth(), printImg.getHeight(),
                      OF_IMAGE_COLOR_ALPHA);
        pix2.setFromPixels(pix);
        // pix.setFromPixels(pix);
        pix.save(ofGetTimestampString() + "print.png");
    }
    
    if (key == 'l') {
        lines.begin();
        ofClear(0, 0, 0, 1.0);
        glLineWidth(22.0);
        for (int i = 0; i < polyMap.levels.size(); i++) {
            for (int j = 0; j < polyMap.levels.at(i).lines.size(); j++) {
                ofPath &thisLine = polyMap.levels.at(i).lines.at(j);
                thisLine.draw();
            }
        }
        lines.end();
    }
    
    if (key == 'r') {
        float lat = ofRandom(-60.0, 60.0);
        float lon = ofRandom(-180.0, 180.0);
        float zoom = ofRandom(11, 13);
        
        mapZen.createMapImage(lat, lon, zoom, 256 * 8, 256 * 5, "Terrarium",0);
        
        mapZen.createMapImage(lat, lon, zoom, 256 * 8, 256 * 5, "Normal",1);
    }
    
    if (key == 'g') {
        drawGui = !drawGui;
    }
}

void ofApp::loadShader(const void *sender) {
    ofxButton *button = (ofxButton *)sender;
    shaderfile = button->getName();
    shader.load("shader.vert", button->getName());
    cout << button->getName() << endl;
}

void ofApp::convertMapImg() {
    polyMap.loadMap(*mapZen.getMapImagesPtr()->at(0), linesAmt);
    polyMap.loadMap(*mapZen.getMapImagesPtr()->at(2), linesAmt);
    
    // polyMap.loadMap(*mapZen.getMapImagesPtr()->at(1), linesAmt);
    
    lineMesh.clear();
    lineMesh.setMode(OF_PRIMITIVE_LINES);
    int stepI = 10;
    int stepJ = 10;
    for (int i = 0; i < mapZen.getMapImagesPtr()->at(0)->getHeight();
         i += stepI) {
        ofPath tempP;
        
        for (int j = 0; j < mapZen.getMapImagesPtr()->at(0)->getWidth() - stepJ;
             j += stepJ) {
            float hmapA = mapZen.getMapImagesPtr()->at(0)->getColor(j, i).r;
            float hmapB = mapZen.getMapImagesPtr()->at(0)->getColor(j + stepJ, i).r;
            ofVec3f trans = ofVec3f(-256 * 3, -256 * 3, 0);
            ofVec3f tempVecA = ofVec3f(j, i, hmapA);
            ofVec3f tempVecB = ofVec3f(j + stepJ, i, hmapB);
            
            //	ofVec3f tempVecA = ofVec3f(j, (i)+hmapA,0);
            //	ofVec3f tempVecB = ofVec3f(j + stepJ, (i)+hmapB, 0);
            lineMesh.addVertex(tempVecA + trans);
            lineMesh.addVertex(tempVecB + trans);
            lineMesh.addColor(ofColor(0, 0, 0, 150));
            lineMesh.addColor(ofColor(0, 0, 0, 150));
            
            tempP.curveTo(tempVecA + trans);
        }
        tempP.setFilled(false);
        tempP.setStrokeColor(ofColor(ofColor(0, 0, 0, 100)));
        tempP.setStrokeWidth(2.50);
        
        mapCurves.push_back(tempP);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
