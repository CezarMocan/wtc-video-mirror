/*
 *  ofxTweener.h
 *  openFrameworks
 *
 *  Created by Sander ter Braak on 26-08-10.
 *
 */

#include "ofMain.h"
#include "ofxTransitions.h"
#include <Poco/Timestamp.h>

#pragma once

#define TWEENMODE_OVERRIDE 0x01
#define TWEENMODE_SEQUENCE 0x02

// modified by Todd Vanderlin to use lambda
#define _cb_func_ std::function<void(float * var)>

class Tween {
public:
	typedef float(ofxTransitions::* easeFunction)(float,float,float,float);
	float * _var;
	float _from, _to, _duration,_by, _useBezier;
	easeFunction _easeFunction;
	Poco::Timestamp _timestamp;
};


class ofxTweener : public ofBaseApp {

public:
	
	ofxTweener();
	
	void addTween(float &var, float to, float time, _cb_func_ callback=nullptr);
	void addTween(float &var, float to, float time, float (ofxTransitions::*ease) (float,float,float,float), _cb_func_ callback=nullptr);
	void addTween(float &var, float to, float time, float (ofxTransitions::*ease) (float,float,float,float), float delay, _cb_func_ callback=nullptr);
	void addTween(float &var, float to, float time, float (ofxTransitions::*ease) (float,float,float,float), float delay, float bezierPoint, _cb_func_ callback=nullptr);
    
	
	void removeTween(float &var);	
	void setTimeScale(float scale);
	void update();
	void removeAllTweens();	
	void setMode(int mode);
	
	int getTweenCount();	
	
	
private:
	float				_scale;
	ofxTransitions		a;
	bool				_override;
	void				addTween(float &var, float to, float time, float (ofxTransitions::*ease) (float,float,float,float), float delay, float bezierPoint, bool useBezier, _cb_func_ callback=nullptr);
	float				bezier(float b, float e, float t, float p);
	vector<Tween>		tweens;
    std::map<float *, _cb_func_> callbacks;

};

extern ofxTweener Tweener;
