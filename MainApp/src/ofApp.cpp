#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofSetVerticalSync(true);

    ofBackground(0);

    videos.push_back("video/video-1-hap-noaudio.mov");
    videos.push_back("video/video-2-hap-noaudio.mov");
    videos.push_back("video/video-3-hap-noaudio.mov");
    videos.push_back("video/video-4-hap-noaudio.mov");
    videos.push_back("video/video-5-hap-noaudio.mov");

    // Load a movie file
//    players[0].load("video/video-1-hap-noaudio.mov");
//    players[0].play();
    for (int i = 0; i < 5; i++) {
        loadVideo(videos[i], players[i]);
        players[i].setLoopState(OF_LOOP_PALINDROME);
    }

	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(1000);
	contourFinder.setThreshold(15);

    initKinect();

    maskShader.setup("shaders/mask-shader.vert", "shaders/mask-shader.frag");
    videoMaskFbo.allocate(COLOR_WIDTH, COLOR_HEIGHT);
    windowFbo.allocate(COLOR_WIDTH, COLOR_HEIGHT);

    mesh.set(COLOR_WIDTH, COLOR_HEIGHT, 0, 0);
    mesh.setPosition(COLOR_WIDTH / 2, COLOR_HEIGHT / 2, 0);
    
    gui.setup();

    parameters.add(depthImageScale.set("depth scale", DEPTH_DEFAULT_SCALE, 1, 4));
    paramScaleListener = depthImageScale.newListener([&](float& val) {
        TrackedBody::globalScale = val;
        });

    parameters.add(depthImageTranslateX.set("depth x offset", DEPTH_DEFAULT_OFFSET_X, 0, 500));
    paramTranslateXListener = depthImageTranslateX.newListener([&](float& val) {
        TrackedBody::globalTranslateX = val;
        });

    parameters.add(depthImageTranslateY.set("depth y offset", DEPTH_DEFAULT_OFFSET_Y, -500, 500));
    paramTranslateYListener = depthImageTranslateY.newListener([&](float& val) {
        TrackedBody::globalTranslateY = val;
        });

    gui.add(parameters);    
}

void ofApp::initKinect() {
    kinect.open();
    kinect.initDepthSource();
    kinect.initColorSource();
    kinect.initInfraredSource();
    kinect.initBodySource();
    kinect.initBodyIndexSource();

    if (kinect.getSensor()->get_CoordinateMapper(&coordinateMapper) < 0) {
        ofLogError() << "Could not acquire CoordinateMapper!";
    }

    numBodiesTracked = 0;
    bHaveAllStreams = false;

    bodyIndexImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    foregroundImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);

    colorCoords.resize(DEPTH_WIDTH * DEPTH_HEIGHT);
}

void ofApp::loadVideo(string filename, ofxHapPlayer& plr) {
    plr.loadAsync(filename);
    plr.play();
}

void ofApp::detectBodies() {
	// Count number of tracked bodies and update skeletons for each tracked body
	auto& bodies = kinect.getBodySource()->getBodies();
	vector<int> oldTrackedBodyIds = this->trackedBodyIds;
	this->trackedBodyIds.clear();

	for (auto& body : bodies) {
		if (body.tracked) {
			// Update body skeleton data for tracked bodies
			this->trackedBodyIds.push_back(body.bodyId);

			if (this->trackedBodies.find(body.bodyId) == this->trackedBodies.end()) {
				this->trackedBodies[body.bodyId] = make_shared<TrackedBody>(body.bodyId, 0.25, 800);
				this->trackedBodies[body.bodyId]->setIsTracked(true);
			}

			// TODO (cezar): Turn this into parameter
			this->trackedBodies[body.bodyId]->setNumberOfContourPoints(200);
		}
		else {
			// Remove untracked bodies from map
			for (auto it = oldTrackedBodyIds.begin(); it != oldTrackedBodyIds.end(); ++it) {
				int index = *it;
				if (index == body.bodyId) {
					this->trackedBodies[index]->setIsTracked(false);
					this->trackedBodies.erase(index);
				}
			}
		}
	}
}

void ofApp::computeBodyContours() {
	int previewWidth = DEPTH_WIDTH;
	int previewHeight = DEPTH_HEIGHT;

	// WARNING: This code works under the assumption that there is only one tracked body
	// (this is our installation setup.)
	for (int i = 0; i < this->trackedBodyIds.size(); i++) {
		const int bodyId = this->trackedBodyIds[i];
		contourFinder.setUseTargetColor(true);
		contourFinder.setTargetColor(ofColor(bodyId));
		contourFinder.setThreshold(0);
		contourFinder.findContours(kinect.getBodyIndexSource()->getPixels());

		shared_ptr<TrackedBody> currentBody = this->trackedBodies[bodyId];
		currentBody->updateContourData(contourFinder.getPolylines());
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
	this->detectBodies();
	this->computeBodyContours();

    videoMaskFbo.begin();
    ofClear(0, 0, 0, 0);
    /*
        ofSetColor(0, 0, 0, 5);
        ofFill();
        ofDrawRectangle(0, 0, COLOR_WIDTH, COLOR_HEIGHT);
        */

    ofSetColor(255);

    for (int i = 0; i < this->trackedBodyIds.size(); i++) {
        const int bodyId = this->trackedBodyIds[i];
        if (trackedBodies[bodyId]->isTracked) {
            trackedBodies[bodyId]->drawContourForRaster(ofColor(255, 255, 255, 255));
        }
    }
    videoMaskFbo.end();

    if (ofRandom(500) < 1) {
        videoIndex = (videoIndex + 1) % 5;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
  	kinect.getColorSource()->draw(0, 0, COLOR_WIDTH, COLOR_HEIGHT);

    //videoMaskFbo.draw(0, 0);
    //videoMaskFbo.getTexture().draw(0, 0);

    if (players[videoIndex].isLoaded()) {
        windowFbo.begin();

        ofSetColor(0, 0, 0, 5);
        ofFill();
        //ofDrawRectangle(0, 0, COLOR_WIDTH, COLOR_HEIGHT);

        maskShader.begin();
        float width = COLOR_WIDTH;
        float height = COLOR_HEIGHT; //width * players[videoIndex].getHeight() / players[videoIndex].getWidth();

        maskShader.setUniformTexture("mask", videoMaskFbo.getTexture(), 1);

        if (sin(ofGetElapsedTimef()) < 2) {
            players[videoIndex].draw(0, 0, width, height);
        }
        else {
            kinect.getColorSource()->draw(0, 0, COLOR_WIDTH, COLOR_HEIGHT);
        }
        
        maskShader.end();

        windowFbo.end();
    }

    windowFbo.draw(0, 0);

	stringstream ss;
	ss << "fps : " << ofGetFrameRate() << endl;
	ss << "Tracked bodies: " << trackedBodyIds.size();
	if (!bHaveAllStreams) ss << endl << "Not all streams detected!";
	ofDrawBitmapStringHighlight(ss.str(), gui.getPosition().x + gui.getWidth() + 10, 20);

    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    int index = key - '1';
    if (index >= 0 && index < 5) {
        if (players[index].isLoaded()) {
            players[index].stop();
            players[index].close();
        }
        else {
            loadVideo(videos[index], players[index]);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
