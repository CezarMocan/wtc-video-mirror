//
//  AutoShader.h
//  Particles
//
//  Created by tvanderlin on 3/12/20.
//
//

// Uncomment below in order to enable automatic shader reloading
#define AUTO_SHADER_RELOAD

#pragma once
#include "ofMain.h"
#include "Poco/File.h"
#include "Poco/Timestamp.h"

class ofAutoShader : public ofShader {
public:
    void setup(string vertName, string fragName, string geomName = "") {
        this->vertName = vertName;
        this->fragName = fragName;
        this->geomName = geomName;
        ofShader::load(vertName, fragName, geomName);
        ofAddListener(ofEvents().update, this, &ofAutoShader::update);
        ofShader::bindDefaults();
    }
    
    void update(ofEventArgs &args) {
#ifdef AUTO_SHADER_RELOAD
        if (ofGetFrameNum() % 30 != 0) return;
        bool needsReload = false;

        ofFile fragFile(fragName);
        if(fragFile.exists()) {
            Poco::Timestamp fragTimestamp = Poco::File(fragFile.getAbsolutePath()).getLastModified();
            if(fragTimestamp != lastFragTimestamp) {
                needsReload = true;
                lastFragTimestamp = fragTimestamp;
            }
        } else {
            fragName = "";
        }
        
        ofFile vertFile(vertName);
        if(vertFile.exists()) {
            Poco::Timestamp vertTimestamp = Poco::File(vertFile.getAbsolutePath()).getLastModified();
            if(vertTimestamp != lastVertTimestamp) {
                needsReload = true;
                lastVertTimestamp = vertTimestamp;
            }
        } else {
            vertName = "";
        }        
        
        ofFile geomFile(geomName);
        if(geomFile.exists()) {
            Poco::Timestamp geomTimestamp = Poco::File(geomFile.getAbsolutePath()).getLastModified();
            if(geomTimestamp != lastGeomTimestamp) {
                needsReload = true;
                lastGeomTimestamp = geomTimestamp;
            }
        } else {
            geomName = "";
        }               

        if(needsReload) {
            bool ok = false;
            if (geomName == "") {
                ok = ofShader::load(vertName, fragName);
            }
            else {
                ok = ofShader::load(vertName, fragName, geomName);
            }
            if (ok) {
                //cout << "loaded " << vertName << endl;
                //cout << "loaded " << fragName << endl;
                //cout << "loaded " << geomName << endl;
            }
            else {
                cout << "****** error loading *****" << endl;
            }
        }

#endif
    }
private:
    string vertName, fragName, geomName;
    Poco::Timestamp lastFragTimestamp, lastVertTimestamp, lastGeomTimestamp;
};
