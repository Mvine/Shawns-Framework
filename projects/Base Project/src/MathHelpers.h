#pragma once

// Linear interpolation
template<typename T>
inline T lerp(const T &v0, const T &v1, const float& t)
{
	// TODO: Replace this line with the correct algorithm 
	// The last parameter is the interpolation parameters
	// The first two parameters are the points to interpolate between
	return (((1 - t) * v0) + (t * v1));
}

// Inverse Lerp: determines how much a value is between two other values
inline float inverseLerp(const float &v, const float &v0, const float &v1)
{
	// TODO: Replace this line with the correct algorithm 
	// The 1st parameter is the point between the 2nd and 3rd parameter
	return ((v - v0) / (v1 - v0));
}