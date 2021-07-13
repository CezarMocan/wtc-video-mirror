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

	void updateContourData(vector<ofPolyline> contours);
	void draw(ofColor color);

	ICoordinateMapper* coordinateMapper;
	ofPolyline rawContour;
	ofPolyline contour;
	ofPath* segment;

	deque<ofPath> contoursAcrossFrames;

	int index;

	float smoothingFactor;
	int contourPoints;
	int framesDuration;
	int noContours;
	bool isTracked;
	int contourIndexOffset;
	ofColor generalColor;

	ofPath contourPath;

	ofFbo mainFbo, polyFbo;

	static float globalScale;
	static float globalTranslateX;
	static float globalTranslateY;
};

#endif