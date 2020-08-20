#include "ofMain.h"
#include "PointCloud.h"

glm::vec3 randomSphere(float radius) {
  float inclination = ofRandom(TWO_PI*4);
  float azimuth = ofRandom(TWO_PI*4);
  return glm::vec3(
    sin(inclination)*cos(azimuth)*radius,
    sin(inclination)*sin(azimuth)*radius,
    cos(inclination)*radius
  );
}



PointCloud::PointCloud() {}

void PointCloud::init(int w, int h) {
    meshWhite.setMode(OF_PRIMITIVE_POINTS);
    meshBlack.setMode(OF_PRIMITIVE_POINTS);
    lineMeshBlack.setMode(OF_PRIMITIVE_LINES); // OF_PRIMITIVE_LINE_STRIP is another option
    
	  glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
    glPointSize(1); // make the points bigger
    
    clear();
    
    sizeMultiplier = float(w)/1920.0; // all sizes assume 1920x1080
  }

  void PointCloud::addRandomPointToWhite() {
    float radius = pow(ofRandom(1.0), 4.0);
    float inclination = ofRandom(TWO_PI*4)*radius;
    float azimuth = floor(ofRandom(6.0)) + ofRandom(0.1, 10.0)*radius;
    radius *= 500;
    glm::vec3 pos = glm::vec3(
      sin(inclination)*cos(azimuth)*radius*sizeMultiplier,
      sin(inclination)*sin(azimuth)*radius*sizeMultiplier,
      cos(inclination)*radius*sizeMultiplier
    );
    meshWhite.addColor(ofColor(255, 100));
    meshWhite.addVertex(pos);
    pointsWhite.push_back(pos);
  }

  void PointCloud::addRandomPointToWhiteCloseTo(glm::vec3 point) {
    glm::vec3 rand_point = randomSphere(ofRandom(100));
    glm::vec3 pos = point + rand_point;
    meshWhite.addColor(ofColor(255, 50));
    meshWhite.addVertex(pos);
    pointsWhite.push_back(pos);
    meshBlack.addColor(ofColor(0, 50));
    meshBlack.addVertex(pos);
    pointsBlack.push_back(pos);
  }

  void PointCloud::addPointToWhite(glm::vec3 p) {
    meshWhite.addColor(ofColor(255, 100));
    meshWhite.addVertex(p);
    pointsWhite.push_back(p);
  }
  
  void PointCloud::clear() {
    meshWhite.clear();
    meshBlack.clear();
    lineMeshBlack.clear();
    pointsWhite.clear();
    pointsBlack.clear();
    pointsToMove = 0;
  }
  
  void PointCloud::generateRandomCloud() {
    clear();
    for(int i = 0; i < 100000; i++) {
      // generate random spherical coordinate
      float radius = pow(ofRandom(1.0), 4.0);
      float inclination = ofRandom(TWO_PI*4)*radius;
      float azimuth = floor(ofRandom(6.0)) + ofRandom(0.1, 10.0)*radius;
      radius *= 500;
      glm::vec3 pos = glm::vec3(
        sin(inclination)*cos(azimuth)*radius*sizeMultiplier,
        sin(inclination)*sin(azimuth)*radius*sizeMultiplier,
        cos(inclination)*radius*sizeMultiplier
      );
      // glm::vec3 pos = glm::vec3(
      //   (pow(ofRandom(1.0), 3.0)-.5)*200, 
      //   pow(ofRandom(1.0)-.5, 3.0)*200, 
      //   pow(ofRandom(1.0), 5.0)*200*round(ofRandom(-1, 1)));
      pointsBlack.push_back(pos);
    }
    generateMeshes();
  }

  void PointCloud::activate() {
    activated = true;
  }
  
  void PointCloud::deactivate() {
    activated = false;
  }
  
  
  
  void PointCloud::addBlackLines(int numLines) {
    //lineMeshBlack.clear();
    int numVertices = meshBlack.getNumVertices();
    for(int i = 0; i < numLines; i++) {
      // get two points at random and draw a line between them
      glm::vec3 p1, p2, temp;
      float distSqrd, maxDist = 1000, minDist = 100;
      maxDist = pow(sizeMultiplier, 2) * maxDist; 
      minDist = pow(sizeMultiplier, 2) * minDist; 
      int numTries = 0;
      int maxTries = 500;
      if(lastLinePoint == glm::vec3(0, 0, 0) 
        || lineMeshBlack.getNumVertices()%100 == 0
      ) {
        p1 = meshBlack.getVertex(floor(ofRandom(numVertices)));
      } else p1 = lastLinePoint;
      do {
        p2 = meshBlack.getVertex(floor(ofRandom(numVertices)));
        // calculate distance squared
        temp = p2-p1;
        distSqrd = glm::dot(temp, temp);
        maxDist *= 1.004;
        numTries++;
      } while((distSqrd > maxDist || distSqrd < minDist) && (numTries < maxTries));
      
      if(numTries < maxTries) {
        lastLinePoint = p2;
        
        lineMeshBlack.addColor(ofColor(0, 50));
        lineMeshBlack.addVertex(p1);
        lineMeshBlack.addColor(ofColor(0, 50));
        lineMeshBlack.addVertex(p2);
      }
    }
  }
  
  void PointCloud::movePointsRatio(float ratio) {
    int numPointsToMoveNow = pointsBlack.size()*ratio;
    // cout << "Starting to move " << numPointsToMoveNow << " number of points!" << endl;
    movePointsToWhite(numPointsToMoveNow);
  }
  
  void PointCloud::movePointsToWhite(int numPointsToMoveNow) {
		pointsWhite.reserve(numPointsToMoveNow);
    // from black to white
    for (int i = 0; i < numPointsToMoveNow; i++) {
      if(pointsBlack.size() < 1) break;
      
      size_t index = ofRandom(pointsBlack.size()-1);
			// remove_at returns the element that was removed
      pointsWhite.push_back(remove_at(pointsBlack, index));
    }
    generateMeshes();
  }
  
  void PointCloud::update(float dt) {
    pointsToMove += 100 * dt;
    int numPointsToMoveNow = floor(pointsToMove);
    pointsToMove -= numPointsToMoveNow;
    
    // // from white to black
    // for (int i = 0; i < numPointsToMoveNow; i++) {
    //   if(pointsWhite.size() < 1) break;
    // 
    //   int index = ofRandom(pointsWhite.size()-1);
    //   cout << "index to move: " << index << endl;
    //   pointsBlack.push_back(pointsWhite[index]);
    //   pointsWhite.erase(pointsWhite.begin() + index);
    // }
		
    // movePointsToWhite(numPointsToMoveNow);
    
    if(doAddBlackLines) {
      addBlackLines(4000 * dt);
    }
		
		zpos += zposVelocity * dt;
		if( (zposVelocity > 0 && zpos > zposTarget)
			|| (zposVelocity < 0 && zpos < zposTarget)
    ) {
			zpos = zposTarget;
			zposVelocity = 0;
		}
    
    rotateTime(dt);
  }
	
	void PointCloud::draw(float currentTime) {
    if(activated) {
      // even points can overlap with each other, let's avoid that
      //ofEnableDepthTest();
    	// cam.begin();
    	//ofScale(2, -2, 2); // flip the y axis and zoom in a bit
    	//ofRotateYDeg(90);
      ofPushMatrix();
      ofTranslate(0, 0, zpos);
      ofRotateYRad(rotation);
      ofRotateXRad(rotation*0.01582);
      ofSetColor(255, alpha);
    	if(doDrawWhiteCloud) meshWhite.draw();
      if(doDrawBlackCloud) meshBlack.draw();
      if(doDrawBlackLines) lineMeshBlack.draw();
      ofPopMatrix();
    	// cam.end();
    }
  }
  
  void PointCloud::generateMeshes() {
    meshWhite.clear();
    for(auto& p : pointsWhite) {
      meshWhite.addColor(ofColor(255, 50));
      meshWhite.addVertex(p);
    }
    meshBlack.clear();
    for(auto& p : pointsBlack) {
      meshBlack.addColor(ofColor(0, 50));
      meshBlack.addVertex(p);
    }
  }
	
	void PointCloud::rotateTime(float dt) {
		rotation += TWO_PI * 0.02 * rotationSpeed * dt;
	}
	
	void PointCloud::setZpos(float zpos_) {
		zpos = zpos_*sizeMultiplier;
		zposTarget = zpos;
		zposVelocity = 0;
	}
	
	void PointCloud::zoomTo(float zpos_, float dur) {
		zposTarget = zpos_*sizeMultiplier;
		zposVelocity = (zposTarget - zpos) / dur;
	}