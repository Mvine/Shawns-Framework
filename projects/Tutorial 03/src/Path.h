#pragma once

#include <vector>
#include "PointHandle.h"

class Path
{
public:
	std::vector<std::vector<PointHandle>>pointHandlesCollection;
	// List of points locations
	std::vector<PointHandle> pointHandles;
	// List of distances between points (last element is distance between last point and the first)
	std::vector<float> distances;
	int index = 0;		// what index of PointHandle the object is on
	float t = 0;		// interpolation parameter (Should only be 0 to 1)
	float speed = 1.0f; // The speed the path updates at (Higher is faster)
	glm::vec3 position; // The interpolated position on the path

	bool lerping = 1;
	bool catmulling = 0;
	bool forwards = 1;
	void computeDistances();
	void update(double &deltaTime);
};