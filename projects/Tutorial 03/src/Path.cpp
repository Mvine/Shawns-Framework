#include "Path.h"
#include "MathHelpers.h"
#include <iostream>

void Path::computeDistances()
{
	distances.resize(pointHandles.size());
	if (pointHandles.size() > 1)
	{
		for (int i = 0; i < pointHandles.size() - 1; ++i)
			distances[i] = glm::distance(pointHandles[i].position, pointHandles[i + 1].position);
		
		distances[distances.size()-1] = glm::distance(pointHandles[pointHandles.size()-1].position, pointHandles[0].position);
	}
}

void Path::update(double &deltaTime)
{
	/*
		Interpolation is constant no matter how far away the two points are.
		An object on the path will take the same amount of time no matter
		what the distance is between points. In order to make an object
		take longer or shorter for different distances you must factor it
		into the equation that increments the interpolation parameter.

		The longer the distance is between the two points,
		the slower t should increment every update

	*/	
	t += deltaTime * speed;

	if (lerping) {
		while (t >= 1.0f)
		{
			t -= 1.0f;
			++index;
			if (index >= pointHandles.size() - 1)
				index = 0;
		}

		if (index >= pointHandles.size() - 1 || pointHandles.size() == 0)
		{
			index = 0.;
		}
		if (pointHandles.size() > 1)
		{
			if(forwards)
			position = lerp(pointHandles[index].position, pointHandles[index + 1].position, t);
			else
			position = lerp(pointHandles[index+1].position, pointHandles[index].position, t);

			if (index >= pointHandles.size() - 1)
				index = 0;
		}
		else
		{
			position.z = 1000.0f;
		}
	}
	
	else if (catmulling) {
		if(index <= 0)
		{
			index = 1;
		}
		
		while (t >= 1.0f)
		{
			t -= 1.0f;
			++index;
			if (index >= pointHandles.size() - 3 || index < 2)
				index = 1;
		}

		if (index >= pointHandles.size() - 3 || pointHandles.size() == 0)
		{
			index = 1;
		}
		if (pointHandles.size() > 3)
		{
			glm::vec3 p0;
			glm::vec3 p1;
			glm::vec3 p2;
			glm::vec3 p3;

			p0 = pointHandles[index - 1].position;
			p1 = pointHandles[index].position;
			p2 = pointHandles[index + 1].position;
			p3 = pointHandles[index + 2].position;

			//start
			glm::vec3 lastPos = p1;

			//t = glm::fract(t);
			if (forwards)
				position = glm::catmullRom(p0, p1, p2, p3, t);
			else
				position = glm::catmullRom(p3, p2, p1, p0, t);

			if (index >= pointHandles.size() - 2)
				index = 1;
		}
		else
		{
			position.z = 1000.0f;
		}
	}


}