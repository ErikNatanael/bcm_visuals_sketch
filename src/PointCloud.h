#pragma once

#include "ofMain.h"

/*
TODO: Make point clouds for
* Lorenz equations
* Ikeda map
* Rössler attractor https://en.wikipedia.org/wiki/R%C3%B6ssler_attractor
* Software object instructions?

https://en.wikipedia.org/wiki/Complex_analysis

*/

glm::vec3 randomSphere(float radius);
// 
// template <typename T>
// T remove_at(std::vector<T>&v, typename std::vector<T>::size_type n);

// from https://stackoverflow.com/questions/9218724/get-random-element-and-remove-it
template <typename T>
T remove_at(std::vector<T>&v, typename std::vector<T>::size_type n)
{
    T ans = std::move_if_noexcept(v[n]);
    v[n] = std::move_if_noexcept(v.back());
    v.pop_back();
    return ans;
}

class Line {
public:
	glm::vec3 p1, p2;
	
	Line(glm::vec3 p1_, glm::vec3 p2_) {
		p1 = p1_;
		p2 = p2_;
	}
	
	glm::vec3 lerp(float amt) {
		return glm::vec3(
			ofLerp(p1.x, p2.x, amt),
			ofLerp(p1.y, p2.y, amt),
			ofLerp(p1.z, p2.z, amt)
		);
	}
	
	float distance() {
		return glm::distance(p1, p2);
	}
};

class PointCloud {
public:
  ofMesh meshWhite;
  ofMesh meshBlack;
  ofMesh lineMeshBlack;
	ofMesh lineMeshWhite;
  // ofEasyCam cam;
  
  vector<glm::vec3> pointsWhite; // coordinates for all points
  vector<glm::vec3> pointsBlack; // coordinates for all points
  
  float pointsToMove = 0; // a variable to be able to save fractional amounts of points to move
  float rotation = 0;
  float rotationSpeed = 1.0;
	float zpos = 300;
	float zposTarget = 300;
	float zposVelocity = 0;
  bool activated = true;
	bool doDrawWhiteCloud = true;
	bool doDrawBlackCloud = true;
  bool doDrawBlackLines = true;
	bool doAddBlackLines = false;
  
  bool growLorenzSystem = false;
  double bTarget = 8./3.;
  double bChange = 0;
  double bValue = 0;
  
  float sizeMultiplier = 1.;
  
  float alpha = 255.0;
  
  glm::vec3 lastLinePoint;
  
  PointCloud();

  void init(int w, int h);
  void addPointToWhite(glm::vec3 p);
  void clear();
  void generateRandomCloud();
  void generateMeshes();
  void activate();
  void deactivate();

  void addBlackLines(int numLines);
  void movePointsRatio(float ratio);
  void movePointsToWhite(int numPointsToMoveNow);
  void update(float dt);
  void draw(float currentTime);
  void rotateTime(float dt);
  void setZpos(float zpos_);
  void zoomTo(float zpos_, float dur);
  
  void addRandomPointToWhite();
  void addRandomPointToWhiteCloseTo(glm::vec3 point);
  
};