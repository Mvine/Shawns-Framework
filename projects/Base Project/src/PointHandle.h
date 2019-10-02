#pragma once

#include <GLM/glm.hpp>
#include <string>
#include <vector>

/*
	PointHandle is used for our pathing
	Each point keeps track of its position, label, and size
	The pointSize variable is used for both drawing and collision
	the label string is used in the draw call for our convenience
*/
class PointHandle
{
public:
	// Point size is in pixels
	PointHandle(float _pointSize, glm::vec3 _position, std::string _label = "");

	// Does a simple radius based intersection test
	bool isInside(glm::vec3 p);

	void draw();

	// Erase point at an index from a list of PointHandles, with an option to regenerate the labels
	static void erasePoint(std::vector<PointHandle>& pointVec, int index, bool updateLabels = false);

	glm::vec3 position;
	std::string label;
	float pointSize;
};