/*
This sketch loads hightmap data (terrarium) and the normal-map data from mapzen via the mapZenHelper class.
After loading the 256 sized chunks the polyMap class will be used to convert the image encoding to usefull images.
The polyMap class also uses openCV to create hight contour (set bei the lines amout).
Then those textures are send to the fragment shader for different image manipulation
*/

#pragma once

#include "ofMain.h"
#include "mapZenHelper.h"
#include "ofxGui.h"
#include "polyMap.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void loadShader(const void * sender);
		void convertMapImg();
		void overlayLinesImg();

		mapZenLoader mapZen;
		PolyMap polyMap;
		ofFbo linesFbo;
		bool drawGui;

		ofShader shader;

		ofVec3f light,sendlight;

		ofxPanel gui;
		ofxFloatSlider lightX, lightY, lightZ;
		ofxFloatSlider colorR, colorG, colorB;
		ofxIntSlider linesAmt;
		bool shaderClick;
		int sClick;
		ofxButton convertMap, overlayLines;
		ofDirectory dir;
		string shaderfile;
		vector<ofxButton> shaderFiles;
};
