//
//  ofxCavalierContours.h
//  shape-intersections-per-shape
//
//  Created by Todd  Vanderlin on 5/20/21.
//
#pragma once
#include "ofMain.h"
#include "cavc/polylineoffset.hpp"
#include "cavc/polylinecombine.hpp"

class ofxCavalierContours {
public:
    
    static ofPolyline toOf(cavc::Polyline<double> input) {
        auto poly = cavc::convertArcsToLines(input, 0.01);
        ofPolyline output;
        for(auto &pt : poly.vertexes()) {
            output.addVertex(pt.x(), pt.y());
        }
        output.setClosed(poly.isClosed());
        return output;
    }
    
    static cavc::Polyline<double> toCC(ofPolyline input) {
        cavc::Polyline<double> output;
        for(auto &pt : input) {
            output.addVertex(pt.x, pt.y, 0);
        }
        output.isClosed() = input.isClosed();
        return output;
    }
    
    static std::vector<ofPolyline> toOf(std::vector<cavc::Polyline<double>> polys) {
        std::vector<ofPolyline> output;
        for(auto &poly : polys) {
            output.push_back(ofxCavalierContours::toOf(poly));
        }
        return output;
    }
    
    static void scale(cavc::Polyline<double> &poly, float scale) {
        cavc::scalePolyline(poly, (double)scale);
    }

    static void translate(cavc::Polyline<double> &poly, glm::vec2 pos) {
        cavc::translatePolyline(poly, cavc::Vector<double, 2>(pos.x, pos.y));
    }
    
    static cavc::Polyline<double> circle(float x, float y, float r) {
        ofPath p;
        p.setCircleResolution(300);
        p.arc(glm::vec2(x, y), r, r, 0, 360);
        auto poly = p.getOutline()[0];
        poly.setClosed(true);
        return ofxCavalierContours::toCC(poly);
    }
    static cavc::Polyline<double> toCC(ofPath input) {
        cavc::Polyline<double> output;
        auto outline = input.getOutline();
        if (outline.size()) {
            for(auto &pt : outline[0]) {
                output.addVertex(pt.x, pt.y, 0);
            }
            output.isClosed() = outline[0].isClosed();
        }
        return output;
    }
    
};
