#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // openFrameworks rendering setup
    ofDisableArbTex();
    ofSetVerticalSync(true);
    ofBackground(0);

    // Loading videos
    videos.push_back("video/video-1-hap-noaudio.mov");
    videos.push_back("video/video-2-hap-noaudio.mov");
    videos.push_back("video/video-3-hap-noaudio.mov");
    videos.push_back("video/video-4-hap-noaudio.mov");
    videos.push_back("video/video-5-hap-noaudio.mov");

    for (int i = 0; i < videos.size(); i++) {
        loadVideo(videos[i], players[i]);
        players[i].setLoopState(OF_LOOP_PALINDROME);
    }

    // Contour finder setup
	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(1000);
	contourFinder.setThreshold(0);

    // Kinect setup
    initKinect();

    // Shaders and FBOs setup
    maskShader.setup("shaders/mask-shader.vert", "shaders/mask-shader.frag");
    videoMaskFbo.allocate(COLOR_WIDTH, COLOR_HEIGHT);
    windowFbo.allocate(COLOR_WIDTH, COLOR_HEIGHT);
    mesh.set(COLOR_WIDTH, COLOR_HEIGHT, 0, 0);
    mesh.setPosition(COLOR_WIDTH / 2, COLOR_HEIGHT / 2, 0);
    
    // GUI parameters setup
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
	// Count number of tracked bodies and update object for each tracked body
	auto& bodies = kinect.getBodySource()->getBodies();
	vector<int> oldTrackedBodyIds = this->trackedBodyIds;
	this->trackedBodyIds.clear();

	for (auto& body : bodies) {
		if (body.tracked) {
			this->trackedBodyIds.push_back(body.bodyId);
			if (this->trackedBodies.find(body.bodyId) == this->trackedBodies.end()) {
				this->trackedBodies[body.bodyId] = make_shared<TrackedBody>(body.bodyId, 0.25, 800);
				this->trackedBodies[body.bodyId]->setIsTracked(true);
			}
            
            // This number of contour points is the resolution of the polygon that
            // represents the body. More points means better accuracy, but slower performance.
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

	// WARNING: This code needs to be tested for multiple people.

    // The way kinect.getBodyIndexSource() works is that it's an image
    // in which the pixels of body #1 all have the color (1, 1, 1),
    // the pixels of body #2 all have the color (2, 2, 2), etc.
    // So the contourFinder takes the bodyId as a color, and looks
    // for continuous areas of pixels with that color.
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
    // At every frame, we ask the kinect to update its video feed
    // and then we update our list of tracked bodies, and 
    // get their contours as polygons.
	kinect.update();
	this->detectBodies();
	this->computeBodyContours();

    // We switch up the video that's being played roughly every 500 frames.
    if (ofRandom(500) < 1) {
        videoIndex = (videoIndex + 1) % 5;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // First, we draw the camera feed directly onto the screen.
  	kinect.getColorSource()->draw(0, 0, COLOR_WIDTH, COLOR_HEIGHT);

    // Next up, we draw the body contours onto a texture (FBO,)
    // in order to use them as a mask for the video.
    // When implementing multiple bodies, you'll have to 
    // either use one FBO per body (so you can have independent masks,)
    // or draw all bodies on the same FBO, but with different colors
    // and let the shader figure out which color masks which video.
    videoMaskFbo.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255);

    for (int i = 0; i < this->trackedBodyIds.size(); i++) {
        const int bodyId = this->trackedBodyIds[i];
        if (trackedBodies[bodyId]->isTracked) {
            trackedBodies[bodyId]->draw(ofColor(255, 255, 255, 255));
        }
    }
    videoMaskFbo.end();

    if (players[videoIndex].isLoaded()) {
        // We draw the masked video onto another FBO
        windowFbo.begin();
        ofSetColor(0, 0, 0, 5);
        ofFill();

        // The shader takes a mask texture (where we drew the body in white, over a black background)
        // and the current frame of the current video, and applies the mask.
        maskShader.begin();
        float width = COLOR_WIDTH;
        float height = COLOR_HEIGHT; //width * players[videoIndex].getHeight() / players[videoIndex].getWidth();

        maskShader.setUniformTexture("mask", videoMaskFbo.getTexture(), 1);

        // Just for demo purposes, draw the current video most of the time,
        // but also draw the camera feed every now and then.
        if (sin(ofGetElapsedTimef()) < 0.9) {
            players[videoIndex].draw(0, 0, width, height);
        }
        else {
            kinect.getColorSource()->draw(0, 0, COLOR_WIDTH, COLOR_HEIGHT);
        }
        
        maskShader.end();
        windowFbo.end();
    }

    // And then we draw the FBO containing the masked videos on top of the camera feed.
    windowFbo.draw(0, 0);

    // Some debug stats & the GUI.
	stringstream ss;
	ss << "fps : " << ofGetFrameRate() << endl;
	ss << "Tracked bodies: " << trackedBodyIds.size();
	if (!bHaveAllStreams) ss << endl << "Not all streams detected!";
	ofDrawBitmapStringHighlight(ss.str(), gui.getPosition().x + gui.getWidth() + 10, 20);

    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
