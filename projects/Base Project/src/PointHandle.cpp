#include "PointHandle.h"
#include "TTK\GraphicsUtils.h"

PointHandle::PointHandle(float _pointSize, glm::vec3 _position, std::string _label)
{
	pointSize = _pointSize;
	position = _position;
	label = _label;
}

// If the distance between the vector that is sent is and the center of the point is smaller than radius of the point
bool PointHandle::isInside(glm::vec3 p)
{
	return glm::length((p - position)) < pointSize;
}

// This draw function will draw the location of the point into screen coordinates
// The size of the dot will be determined by the pointSize variable
void PointHandle::draw()
{
	TTK::Graphics::DrawPoint(position, pointSize);
	TTK::Graphics::DrawText2D(label, position.x, position.y);
}

// This function will regenerate the default labels when a point is erased
void PointHandle::erasePoint(std::vector<PointHandle>& pointVec, int index, bool updateLabels)
{
	pointVec.erase(pointVec.begin() + index);
	for (int i = 0; i < pointVec.size(); ++i)
	{
		if(updateLabels)
			pointVec[i].label = std::to_string(i);
	}
}