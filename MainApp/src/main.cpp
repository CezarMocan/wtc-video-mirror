#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(COLOR_WIDTH, COLOR_HEIGHT);
	ofCreateWindow(settings);
	return ofRunApp(std::make_shared<ofApp>());
}
