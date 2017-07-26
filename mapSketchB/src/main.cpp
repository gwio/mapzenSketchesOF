#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {
  // ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL
  // context
  ofGLFWWindowSettings settings;
  settings.setGLVersion(3, 2); // we define the OpenGL version we want to use
  settings.width = 1920;
  settings.height = 1078;
  settings.decorated = true;
  ofCreateWindow(settings);
  // this kicks off the running of my app
  ofRunApp(new ofApp());
  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp(new ofApp());
}
