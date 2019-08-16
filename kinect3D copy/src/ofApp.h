#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxCv.h"

class Particle
{
public:
    
    void update()
    {
        position += velocity;
    }
    
    glm::vec3 position;
    ofColor color;
    glm::vec3 velocity;
    //glm::vec3 gravity;
    
    float radius = 0;
    bool isDead = false;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void keyPressed(int key);
        void makeParticle(int x, int y, int z, ofColor color);
        void audioIn(ofSoundBuffer &input);
    
    ofSoundStream stream;
    
    std::vector <Particle> particles;
    glm::vec3 position;
    glm::vec3 gravity;
    
    ofxKinectV2 kinect;
    ofTexture texture;
    ofTexture depthTexture;
    ofTexture bufferTexture;
    ofPixels kinectPixels;
    
    ofFloatPixels depthPixels;
    
    ofEasyCam cam;
    
    ofMesh mesh;
    ofMesh meshCopy;
    ofVboMesh ballMesh;
    
    bool particleEffect;
    bool bUseSmooth;
    
    float smoothedVol;
    float unsmoothed;
    float distance;
    float smoothedVolb;
		
    float w;
    float h;
    
    ofPixels depthMask;
    ofTexture depthMaskTex;

//    float alpha = 0.99;
//    ofFloatPixels smoothDepthPixels;
    
};
