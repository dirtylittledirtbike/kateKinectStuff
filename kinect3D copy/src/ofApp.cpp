#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

    ofBackground(80);
 
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = ofxKinectV2().getDeviceList();
    
    // Check to see if there is a KinectV2 attached.
    if (deviceList.size() > 0)
    {
        std::cout<< "Success: Kinect V2 found" << std::endl;
    } else {
        std::cerr<< "Failure: No Kinect V2 found" << std::endl;
        return;
    }
    
    // Reset min/max distances (as per ofxKinectV2).
//    kinect.params.getFloat("minDistance").set(1200);
//    kinect.params.getFloat("maxDistance").set(12000);
    
    kinect.open(deviceList[0].serial);
    
    particleEffect = false;
    
    ballMesh = ofSpherePrimitive(20, 10).getMesh();
    
    //sound setup ---------
    
    int bufferSize = 256;
    smoothedVol = 0.0;
    smoothedVolb = 0.0;
    unsmoothed = 0.0;
    distance = 0;
    
    auto devices = stream.getDeviceList();
    for ( auto &device : devices)
    {
        std::cout << device << std::endl;
    }
    
    ofSoundStreamSettings settings;
    settings.setInDevice(devices[0]);
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    stream.setup(settings);
    
    bUseSmooth = true;
    
 
}

//--------------------------------------------------------------
void ofApp::update()
{
    meshCopy = mesh;
    std::size_t meshRows = kinect.getRegisteredPixels().getHeight();
    std::size_t meshCols = kinect.getRegisteredPixels().getWidth();
    
    float meshWidth = kinect.getRegisteredPixels().getWidth();
    float meshHeight = kinect.getRegisteredPixels().getHeight();
    
    std::size_t numCoordinates = 0;
    glm::vec3 avgPosition = { 0, 0, 0 };


    
    kinect.update();
    

    if (kinect.isFrameNew())
    {
        if (!depthMask.isAllocated())
        {
            depthMask.allocate(kinect.getRawDepthPixels().getWidth(),
                               kinect.getRawDepthPixels().getHeight(),
                               1);
        
        }
        
        ofFloatPixels mappedDepthPixels = kinect.getRawDepthPixels();
        for (std::size_t i = 0; i < mappedDepthPixels.size(); ++i)
            mappedDepthPixels[i] = ofMap(mappedDepthPixels[i], kinect.minDistance, kinect.maxDistance, -600, 600, true);
        
        for (std::size_t y = 0; y < mappedDepthPixels.getHeight() - 1; ++y)
        {
            for (std::size_t x = 0; x < mappedDepthPixels.getWidth() - 1; ++x)
            {
                std::size_t index = y * mappedDepthPixels.getWidth() + x;
                
                float depthAtXY = mappedDepthPixels[index];
                
                // This is when I will make indices.
                ofIndexType currentIndex   = (y    ) * mappedDepthPixels.getWidth() + (x    );
                ofIndexType eastIndex      = (y    ) * mappedDepthPixels.getWidth() + (x + 1);
                ofIndexType southEastIndex = (y + 1) * mappedDepthPixels.getWidth() + (x + 1);
                ofIndexType southIndex     = (y + 1) * mappedDepthPixels.getWidth() + (x    );
                
                glm::vec3 currentPosition = { x, y, mappedDepthPixels[currentIndex] };
                glm::vec3 eastPosition = { x, y, mappedDepthPixels[eastIndex] };
                glm::vec3 southPosition = { x, y, mappedDepthPixels[southIndex] };
                glm::vec3 southEastPosition = { x, y, mappedDepthPixels[southEastIndex] };
                
                float distance1 = glm::distance(currentPosition.z, eastPosition.z);
                float distance2 = glm::distance(currentPosition.z, southPosition.z);
                float distance3 = glm::distance(currentPosition.z, eastPosition.z);
                float distance4 = glm::distance(currentPosition.z, southEastPosition.z);
                float avgDist = (distance1 + distance2 + distance3 + distance4)/4;
                
                if (avgDist < 20 && (currentPosition.z > -600) && (currentPosition.z < -400))
                {
                    depthMask[index] = 255;
                }
                else
                {
                    depthMask[index] = 0;
                }
            }
        }
        
        ofxCv::er
        
        
        depthMaskTex.loadData(depthMask);
        
        
//        if (!smoothDepthPixels.isAllocated())
//        {
//            smoothDepthPixels = kinect.getRawDepthPixels();
//        }
//
//        for (std::size_t x = 0; x < smoothDepthPixels.getWidth(); ++x)
//        {
//            for (std::size_t y = 0; y < smoothDepthPixels.getHeight(); ++y)
//            {
//                std::size_t index = y * smoothDepthPixels.getWidth() + x;
//                smoothDepthPixels[index] = alpha * smoothDepthPixels[index] + (1 - alpha) * kinect.getRawDepthPixels()[index];
//            }
//        }

//
//        mesh.clear();
//
//        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//
//
//        // First, generate the vertices.
//        for (std::size_t row = 0; row < meshRows; row++)
//        {
//            for (std::size_t col = 0; col < meshCols; col++)
//            {
//                // Sample the color from the RGB image.
//                std::size_t colorX = ofMap(col, 0, meshCols, 0, kinect.getRawDepthPixels().getWidth());
//                std::size_t colorY = ofMap(row, 0, meshRows, 0, kinect.getRawDepthPixels().getHeight());
//
//                std::size_t depthX = ofMap(col, 0, meshCols, 0, kinect.getRawDepthPixels().getWidth());
//                std::size_t depthY = ofMap(row, 0, meshRows, 0, kinect.getRawDepthPixels().getHeight());
//
//
//                ofColor color = kinect.getRegisteredPixels().getColor(colorX, colorY);
//                color.a = 255;
//                mesh.addColor(color);
//
//                // 3D Positions in space.
//                float x = ofMap(col, 0, kinect.getRawDepthPixels().getWidth(), -600, 600);
//                float y = ofMap(row, 0, kinect.getRawDepthPixels().getHeight(), -600, 600);
//                float z = ofMap(kinect.getRawDepthPixels()[depthY * kinect.getRawDepthPixels().getWidth() + depthX], kinect.minDistance, kinect.maxDistance, -600, 600, true);
//
//                position = { x, y, z };
//                mesh.addVertex(position);
//
//            }
//        }
//
//        particles.clear();
//
//
//        // Second create indices.
//        // First, generate the vertices.
//        for (std::size_t col = 0; col < meshCols; col++)
//        {
//            for (std::size_t row = 0; row < meshRows; row++)
//            {
//                if (col < (meshCols - 1) && row < (meshRows - 1))
//                {
//                    // This is when I will make indices.
//                    ofIndexType currentIndex   = (row    ) * meshCols + (col    );
//                    ofIndexType eastIndex      = (row    ) * meshCols + (col + 1);
//                    ofIndexType southEastIndex = (row + 1) * meshCols + (col + 1);
//                    ofIndexType southIndex     = (row + 1) * meshCols + (col    );
//
//                    glm::vec3 currentPosition = mesh.getVertices()[currentIndex];
//                    glm::vec3 eastPosition = mesh.getVertices()[eastIndex];
//                    glm::vec3 southPosition = mesh.getVertices()[southIndex];
//                    glm::vec3 southEastPosition = mesh.getVertices()[southEastIndex];
//
//                    float distance1 = glm::distance(currentPosition.z, eastPosition.z);
//                    float distance2 = glm::distance(currentPosition.z, southPosition.z);
//                    float distance3 = glm::distance(currentPosition.z, eastPosition.z);
//                    float distance4 = glm::distance(currentPosition.z, southEastPosition.z);
//                    float avgDist = (distance1 + distance2 + distance3 + distance4)/4;
//
//                    if (avgDist < 20)
//                    {
//                        if ((currentPosition.z > -600) && (currentPosition.z < -400))
//                        {
//                            avgPosition += currentPosition;
//                            numCoordinates++;
//
//                            makeParticle(currentPosition.x, currentPosition.y, currentPosition.z,mesh.getColors()[currentIndex]);
//
//                            mesh.addIndex(currentIndex);
//                            mesh.addIndex(eastIndex);
//                            mesh.addIndex(southEastIndex);
//
//                            // Triangle 1
//                            mesh.addIndex(currentIndex);
//                            mesh.addIndex(southEastIndex);
//                            mesh.addIndex(southIndex);
//                        }
//                    }
//                }
//            }
//        }
//
//
//        texture.loadData(kinect.getPixels());
//        depthTexture.loadData(kinect.getRegisteredPixels());
//
//
//
//        ofRectangle screenRectangle(0, 0, ofGetWidth(), ofGetHeight());
//        screenRectangle.scaleFromCenter(1.5);
//
//        for (std::size_t i = 0; i < particles.size(); i++)
//        {
//            // Add gravity to our velocity.
//            if (particleEffect)
//            {
//                particles[i].velocity.z += 0;
//                particles[i].velocity.y += 1;
//                particles[i].velocity.z += 0;
////                particles[i].radius = ofRandom(30, 200);
//
////                particles[i].velocity.z = 0;
////                particles[i].velocity.y = 0;
////                particles[i].velocity.z = 0;
//            }
//            else
//            {
//                particles[i].velocity.x = 0;
//                particles[i].velocity.y = 0;
//                particles[i].velocity.z = 0;
//            }
//            particles[i].update();
//
////            if (screenRectangle.inside(particles[i].position))
////            {
////                // The particle is still inside the screen.
////            }
////            else
////            {
////                // It is off screen.
////                particles[i].isDead = true;
////            }
//        }
//
//
//    }
//
//    avgPosition /= double(numCoordinates);
//
//    for (auto& vertex: mesh.getVertices())
//    {
//        vertex -= avgPosition;
//    }
//
//
////    meshCopy = mesh;
////
////
////
////
////    if (mouseDisplacement) {
////        // Get the mouse location - it must be relative to the center of our screen
////        // because of the ofTranslate() command in draw()
////        glm::vec3 mouse = {ofGetWidth()/2 - mouseX, ofGetHeight()/2 - mouseY, 0};
////
////        // Loop through all the vertices in the mesh and move them away from the
////        // mouse
////        for (int i=0; i < mesh.getNumVertices(); ++i) {
////            glm::vec3 vertex = meshCopy.getVertex(i);
////            float distanceToMouse = glm::distance(mouse, vertex);
////
////            // Scale the displacement based on the distance to the mouse
////            // A small distance to mouse should yield a small displacement
////            float displacement = ofMap(distanceToMouse, 0, 400, 300, 0, true);
////
////            // Calculate the direction from the mouse to the current vertex
////            glm::vec3 direction = vertex - mouse;
////
////            // Normalize the direction so that it has a length of one
////            // This lets us easily change the length of the vector later
////            direction = glm::normalize(direction);
////
////            // Push the vertex in the direction away from the mouse and push it
////            // a distance equal to the value of the variable displacement
////            glm::vec3 displacedVertex = vertex + displacement*direction;
////
////            mesh.setVertex(i, displacedVertex);
////        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
//    float texWidth = ofGetWidth();
//    float texHeight = ofGetHeight();
//
//    ofSetColor(ofColor::white);
//    depthTexture.draw(0, 0, texWidth/3, texHeight/3);
//
//    if (particleEffect)
//    {
//        cam.begin();
//        ofEnableDepthTest();
//        ofPushMatrix();
//        ofScale(1, -1);
//
//        float mappedVol = ofMap(smoothedVol, 0, 1, 0, ofGetWidth());
//
//        float scale = ofMap(ofGetMouseX(), 0, ofGetWidth(), .2, 10);
//
//        for (std::size_t i = 0; i < particles.size(); i+=10)
//        {
//            //            particles[i].draw();÷
//            ofSetColor(particles[i].color);
//            ofPushMatrix();
//            ofTranslate(particles[i].position);
//            ofScale(scale, scale, scale);
//            ballMesh.draw();
//            ofPopMatrix();
//        }
//
//        ofPopMatrix();
//        ofDisableDepthTest();
//        cam.end();
//    }
//    else
//    {
//        cam.begin();
//        ofPushMatrix();
//        ofScale(1, -1);
//        mesh.draw();
//        ofPopMatrix();
//        cam.end();
//    }
//
//    ofDrawBitmapString(ofToString(smoothedVol), 15, 15);

    
    
    depthMaskTex.draw(0, 0);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

//    if (key == 'm') {
//        mouseDisplacement = !mouseDisplacement; // Inverts the boolean
//        mesh = meshCopy; // Restore the original mesh
//    }
    
    if (key == ' ' ){
        particleEffect = !particleEffect;
    }
    
}

//--------------------------------------------------------------
void ofApp::makeParticle(int x, int y, int z, ofColor color)
{
    Particle p;
    
    p.position.x = x;
    p.position.y = y;
    p.position.z = z;
    
    p.radius = 20;
    p.color = color;
    p.velocity.x = 0;
    p.velocity.y = 0;
    p.velocity.z = 0;
    //    p.gravity.x = ofRandom(-3, 3);
    //    p.gravity.y = ofRandom(-3, 3);
    //    p.gravity.z = ofRandom(-3, 3);
    
    particles.push_back(p);
}

//----------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer &input)
{
    double currentVol;
    int numCounted = 0;
    int channels = input.getNumChannels();
    
    // calculate RMS (root-mean-square) of audio buffer
    for (std::size_t i = 0; i < input.getNumFrames(); i++)
    {
        // collect values at index for left and right channels
        double left = input[i * channels] * 0.5;
        double right = input[i * channels + 1] * 0.5;
        
        // square it to make sure it is always positive
        currentVol += left * left;
        currentVol += right * right;
        numCounted += 2;
    }
    
    // mean-- divide by total counted
    currentVol /= (float)numCounted;
    
    // root -- get the square root
    currentVol = sqrt(currentVol);
    
    unsmoothed = currentVol;
    smoothedVol *= 0.99;
    smoothedVol += currentVol;
    smoothedVolb *= 0.9;
    smoothedVolb += currentVol;
    
}
