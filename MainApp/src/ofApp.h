#pragma once

#include "ofMain.h"
#include "ofxHapPlayer.h"
#include "ofxKinectForWindows2.h"
#include "ofxGui.h"
#include "Constants.h"
#include "TrackedBody.h"
#include "AutoShader.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void loadVideo(string filename, ofxHapPlayer& player);

		ofxHapPlayer player;
		ofxHapPlayer players[20];
		vector<string> videos;
		int videoIndex = 0;

		ofxKFW2::Device kinect;
		ICoordinateMapper* coordinateMapper;
		void initKinect();
		ofImage bodyIndexImg, foregroundImg;
		vector<ofVec2f> colorCoords;
		int numBodiesTracked;
		bool bHaveAllStreams;

		void detectBodies();
		void computeBodyContours();
		void updateTrackedBodies();
		ofxCv::ContourFinder contourFinder;
		map<int, shared_ptr<TrackedBody> > trackedBodies;
		vector<int> trackedBodyIds;

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

		ofxPanel gui;
		ofParameterGroup parameters;
		ofParameter<float> depthImageScale;
		ofEventListener paramScaleListener;

		ofParameter<float> depthImageTranslateX;
		ofEventListener paramTranslateXListener;

		ofParameter<float> depthImageTranslateY;
		ofEventListener paramTranslateYListener;

		ofAutoShader wavyShader;
		ofAutoShader maskShader;
		ofFbo windowFbo;
		ofFbo videoMaskFbo;
		ofPlanePrimitive mesh;
};
