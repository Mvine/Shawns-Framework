#version 410

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

in vec3 CamDirection;
in vec3 LightDirection;

uniform vec3 LightWorldPos;

layout(location = 1) out vec4 frag_color;

void main() 
{
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);//Better off in c++ so it can be changed accordingly
	float lightIntensity = 5.0f; //Better off in c++ so it can be changed accordingly

	vec3 diffuseColor = inColor;
	vec3 ambientColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 specularColor = vec3(0.5f, 0.5f, 0.5f);

	//Normal
	vec3 n = normalize(inNormal);
	//Light Direction
	vec3 l = normalize(LightDirection);
	//Camera Direction
	vec3 e = normalize(CamDirection);
	//Halfway Vector
	vec3 h = normalize(l + e);
	//Specular intensity
	float spec = pow(max(dot(n,h), 0.0f), 256);

	//Specular component
	vec3 specOut = spec * specularColor;

	//distance for attenuating
	float dist = length(LightWorldPos - inWorldPos);

	//Diffuse component
	float diffuseComponent = max(dot(n,l), 0.0f);
	vec3 diffuseOut = (diffuseComponent * diffuseColor) / (dist * dist);

	//Ambient component
	vec3 ambientOut = ambientColor * 0.0f; //Better off in c++ so it can be changed accordingly

	//Result
	vec3 result = (ambientOut + diffuseOut + specOut) * lightIntensity;

	frag_color = vec4(inColor, 1.0f);
}