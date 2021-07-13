#include <sstream>
#include "TrackedBody.h"

float TrackedBody::globalScale = DEPTH_DEFAULT_SCALE;
float TrackedBody::globalTranslateX = DEPTH_DEFAULT_OFFSET_X;
float TrackedBody::globalTranslateY = DEPTH_DEFAULT_OFFSET_Y;


TrackedBody::TrackedBody(int index, float smoothingFactor, int contourPoints)
{
	this->index = index;
	this->smoothingFactor = smoothingFactor;
	this->contourPoints = contourPoints;
	this->framesDuration = 1;
	//this->speedShader.load("shaders_gl3/bodySpeed");

	this->mainFbo.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
	this->polyFbo.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);

	this->noContours = 2;
	this->isTracked = false;

	this->generalColor = ofColor(255, 225, 128, 255);
	this->segment = new ofPath();
}

// ------ Setting state ------

void TrackedBody::setIsTracked(bool isTracked)
{
	this->isTracked = isTracked;
}

void TrackedBody::setNumberOfContourPoints(int contourPoints)
{
	if (contourPoints != this->contourPoints) {
		this->contourPoints = contourPoints;
		this->contour.clear();
	}
}

void TrackedBody::updateContourData(vector<ofPolyline> contours)
{
	if (contours.size() == 0) return;
	// 1. Discard all contours except for the one of maximum perimeter
	// If this is not precise enough, can use the area (but that's probably more
	// computationally expensive)
	pair<int, float> maxContour = make_pair(-1, -1.0f);
	for (int i = 0; i < contours.size(); i++) {
		float perimeter = contours[i].getPerimeter();
		if (perimeter > maxContour.second) {
			maxContour = make_pair(i, perimeter);
		}
	}

	// 2. Save largest area contour and resample it to the desired number of points.
	ofPolyline newContour = contours[maxContour.first].getResampledByCount(this->contourPoints);
	this->rawContour = ofPolyline(newContour);

	// 3. Match with persistent contour
	if (this->contour.size() == 0) {
		this->contour = newContour;
	}
	else {
		// Select matchesToCheck closest points in the new line to the first point in the persistent line
		// And then checked the total distance between the circular permutations, in order to find the right order.
		const int matchesToCheck = 5;

		auto newVertices = newContour.getVertices();
		auto persistentVertices = this->contour.getVertices();
		ofVec2f referencePersistentVertex = ofVec2f(persistentVertices[0]);
		vector<float> distances;

		for (auto it = newVertices.begin(); it != newVertices.end(); ++it) {
			distances.push_back(referencePersistentVertex.squareDistance(ofVec2f(*it)));
		}

		nth_element(distances.begin(), distances.begin() + matchesToCheck, distances.end());
		float thresholdDistance = distances[matchesToCheck - 1];

		pair<int, float> minDistance = make_pair(-1, 1000000000.0f);
		for (int i = 0; i < newVertices.size(); i++) {
			float sqDistance = referencePersistentVertex.squareDistance(ofVec2f(newVertices[i]));
			if (sqDistance <= thresholdDistance + 0.1f) {
				// We have a candidate
				float distance = GeometryUtils::getPolylineSquaredDistanceWithOffset(this->contour, newContour, i);
				if (distance < minDistance.second) minDistance = make_pair(i, distance);
			}
		}

		this->contourIndexOffset = minDistance.first;

		// 4. Update persistent contour, with smoothing
		for (int i = 0; i < this->contour.size(); i++) {
			int newIndex = (i + this->contourIndexOffset) % newContour.size();
			this->contour[i].x = (1 - this->smoothingFactor) * newContour[newIndex].x + this->smoothingFactor * this->contour[i].x;
			this->contour[i].y = (1 - this->smoothingFactor) * newContour[newIndex].y + this->smoothingFactor * this->contour[i].y;
		}
	}
	
	// Store contours across multiple frames
	ofPath pathContour = GeometryUtils::polylineToPath(this->contour);
	pathContour.scale(TrackedBody::globalScale, TrackedBody::globalScale);
	pathContour.translate(glm::vec2(TrackedBody::globalTranslateX, TrackedBody::globalTranslateY));

	contoursAcrossFrames.push_back(pathContour);

	if (contoursAcrossFrames.size() > this->framesDuration) {
		contoursAcrossFrames.pop_front();
	}
}

void TrackedBody::draw(ofColor color) {
	for (auto& contourPath : contoursAcrossFrames) {
		contourPath.setFilled(true);
		contourPath.setFillColor(color);
		contourPath.draw(0, 0);
	}
}