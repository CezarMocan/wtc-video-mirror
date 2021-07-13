#pragma once

#pragma once
#include "ofMain.h"

class GeometryUtils {
public:
	static float getPolylineSquaredDistanceWithOffset(ofPolyline a, ofPolyline b, int offset) {
		auto aV = a.getVertices();
		auto bV = b.getVertices();
		float distance = 0;
		for (int i = 0; i < aV.size(); i++) {
			int bIndex = (i + offset) % bV.size();
			distance += ofVec2f(aV[i]).squareDistance(ofVec2f(bV[bIndex]));
		}
		return distance;
	}

	static ofPath polylineToPath(ofPolyline& p) {
		ofPath path;
		path.clear();
		path.moveTo(p[0]);
		for (int i = 1; i < p.size(); i++)
			path.lineTo(p[i]);
		path.close();
		return path;
	}
};
