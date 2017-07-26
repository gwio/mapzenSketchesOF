#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  // load shader files to gui
  dir.allowExt("frag");
  dir.listDir("");
  dir.sort();
  shaderFiles.resize(dir.size());
  shaderfile = "shader.frag";
  ofBackground(0, 0, 0);
  drawGui = true;
  // latitude, longitude, zoomfactor (1-15 for mapzen?), image size, type
  // ("Terrarium", "Normal") -image size always a factor of 256  allocates
  // ofImage, updates maptiles while loading
  mapZen.createMapImage(62.162616, -143.646240, 11.8, 256 * 3, 256 * 3,
                        "Terrarium");

  mapZen.createMapImage(62.162616, -143.646240, 11.8, 256 * 3, 256 * 3,
                        "Normal");
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
  shader.load("shader.vert", "shader.frag");

  linesFbo.allocate(256 * 8, 256 * 5, GL_RGBA, 16);

  gui.setup();
  gui.add(lightX.setup("light x", 1.0, -1.0, 1.0));
  gui.add(lightY.setup("light y", 1.0, -1.0, 1.0));
  gui.add(lightZ.setup("light z", 1.0, -1.0, 1.0));
  gui.add(colorR.setup("color R", 1.0, -1.0, 1.0));
  gui.add(colorG.setup("color G", 1.0, -1.0, 1.0));
  gui.add(colorB.setup("color B", 1.0, -1.0, 1.0));
  gui.add(linesAmt.setup("linesAmt", 10, 0, 255));
  gui.add(convertMap.setup("convert Image"));
  convertMap.addListener(this, &ofApp::convertMapImg);
  gui.add(overlayLines.setup("overlay Lines"));
  overlayLines.addListener(this, &ofApp::overlayLinesImg);
  for (int i = 0; i < dir.size(); i++) {
    gui.add(shaderFiles.at(i).setup(dir.getName(i)));
    shaderFiles.at(i).addListener(this, &ofApp::loadShader);
  }
  light.z = 1;
  shaderClick = false;
  sClick = 1;
}

//--------------------------------------------------------------
void ofApp::update() {

  light.x = sin(ofGetElapsedTimef() / 2) * 100;
  light.y = cos(ofGetElapsedTimef() / 2) * 100;

  light.x = lightX;
  light.y = lightY;
  light.z = lightZ;

  sendlight = ofVec3f(0, 0, 0) - light;
  sendlight.normalize();

  if (ofGetFrameNum() % 300 == 0) {
    shader.load("shader.vert", shaderfile);
  }

  ofColor tempC;

  if (shaderClick) {
    sClick = 1;
  } else {
    sClick = 0;
  }
}

//--------------------------------------------------------------
void ofApp::draw() {

  shader.begin();
  //send the variables into the shader
  shader.setUniform2f("mousePos", ofGetMouseX(), ofGetMouseY());
  shader.setUniform1i("click", sClick);
  shader.setUniform3f("lightPos", sendlight.x, sendlight.y, sendlight.z);

  shader.setUniform3f("rgbFac", colorR, colorG, colorB);
  shader.setUniform1f("time", ofGetElapsedTimef());
  shader.setUniformTexture("linesTex", linesFbo.getTextureReference(), 3);
  for (int i = 0; i < mapZen.getMapImagesPtr()->size(); i++) {
    shader.setUniformTexture(
        "tex" + ofToString(i),
        mapZen.getMapImagesPtr()->at(i)->getTextureReference(), i + 1);
    mapZen.getMapImagesPtr()->at(i)->draw(0, 0);
  }

  shader.end();

 

  if (drawGui) {
	  ofDrawBitmapString("convert img data after loading... \n g: drawGui \n s: screenshot \n c: convert Img data", 300, 100);
	  gui.draw();
	  ofDrawBitmapString(ofGetFrameRate(), 20, 20);
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

  // press a key after loading to convert data to b&w

  if (key == 'c') {
    polyMap.loadMap(*mapZen.getMapImagesPtr()->at(0), linesAmt);
  }

  if (key == 'f') {
    ofToggleFullscreen();
  }

  if (key == 'g') {
	  drawGui = !drawGui;
  }

  if (key == 's') {
    ofImage pix;
    pix.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
    pix.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    pix.saveImage(ofGetTimestampString() + "debug.png");
  }

  if (key == 'l') {
    linesFbo.begin();
    ofClear(0, 0, 0, 1.0);
    glLineWidth(22.0);
    for (int i = 0; i < polyMap.levels.size(); i++) {
      for (int j = 0; j < polyMap.levels.at(i).lines.size(); j++) {
        ofPath &thisLine = polyMap.levels.at(i).lines.at(j);
        thisLine.draw();
      }
    }
    linesFbo.end();
  }

  if (key == 'r') {
    float lat = ofRandom(-60.0, 60.0);
    float lon = ofRandom(-180.0, 180.0);
    float zoom = ofRandom(11, 13);

    mapZen.clearMaps();
    mapZen.createMapImage(lat, lon, zoom, 256 * 8, 256 * 5, "Terrarium");

    mapZen.createMapImage(lat, lon, zoom, 256 * 8, 256 * 5, "Normal");
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
}

void ofApp::overlayLinesImg() {
  linesFbo.begin();
  ofClear(0, 0, 0, 1.0);
  glLineWidth(22.0);
  for (int i = 0; i < polyMap.levels.size(); i++) {
    for (int j = 0; j < polyMap.levels.at(i).lines.size(); j++) {
      ofPath &thisLine = polyMap.levels.at(i).lines.at(j);
      thisLine.draw();
    }
  }
  linesFbo.end();
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

  shaderClick = !shaderClick;
}

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
