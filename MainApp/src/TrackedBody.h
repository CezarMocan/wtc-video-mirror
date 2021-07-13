#pragma once

#include "ofxKinectForWindows2.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "GeometryUtils.h"
#include "Constants.h"

#ifndef TRACKED_BODY_H
#define TRACKED_BODY_H

using namespace std;

class TrackedBody {
public:
	TrackedBody(int index, float smoothingFactor, int contourPoints = 150);

	void setIsTracked(bool isTracked);
	void setNumberOfContourPoints(int contourPoints);
	void setFramesDuration(int frames);


	void updateContourData(vector<ofPolyline> contours);
	void updateDelayedContours();

	pair<ofPath*, ofRectangle> getContourSegment(int start, int amount);

	void update();
	void drawContours();
	void drawContourForRaster(ofColor color);
	void drawWithShader(ofShader* shader);
	void draw();

	ICoordinateMapper* coordinateMapper;
	ofPolyline rawContour;
	ofPolyline contour;
	ofPath* segment;
	ofImage texture;

	deque<ofPath> contoursAcrossFrames;

	int index;

	void setGeneralColor(ofColor color);
	float smoothingFactor;
	int contourPoints;
	int framesDuration;
	int noContours;
	bool isTracked;
	int contourIndexOffset;
	ofColor generalColor;

	ofPath contourPath;
	vector<ofPolyline> delayedContours;

	ofFbo mainFbo, polyFbo;

	static float globalScale;
	static float globalTranslateX;
	static float globalTranslateY;
};

#endif