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
	// TODO: BONUS CHALLENGE
	
	t += deltaTime * speed;
	

	while (t >= 1.0f)
	{
		t -= 1.0f;
		++index;
		if (index >= pointHandles.size() - 1)
			index = 0;
	}

	if (index >= pointHandles.size() - 1 || pointHandles.size() == 0)
	{
		index = 0;
	}
	if (pointHandles.size() > 1)
	{
		position = lerp(pointHandles[index].position, pointHandles[index+1].position, t);
	}
	else
	{
		position.z = 1000.0f;
	}


}