#version 410

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

in vec3 CamDirection;
in vec3 LightDirection;

uniform vec3 LightWorldPos;

out vec4 frag_color;

void main()
{
	vec3 lightColor      = vec3(1.0f, 1.0f, 1.0f);
	float lightIntensity = 5.0f;

	vec3 diffuse  = inColor;
	vec3 ambient  = vec3(0.2f, 0.7f, 0.2f);
	vec3 specular = vec3(0.5f, 0.5f, 0.5f);

	vec3 normalDir  = normalize(inNormal);
	vec3 lightDir   = normalize(LightDirection);
	vec3 camDir     = normalize(CamDirection);
	vec3 halfwayDir = normalize(lightDir + camDir);

	//specular intensity
	float specIntensity = pow(max(dot(normalDir, halfwayDir), 0.0f), 256.0f);

	//specular component
	vec3 specularOut = specIntensity * specular;

	float distance = length(LightWorldPos - inWorldPos);

	//diffuse component
	float diffuseComponent = max(dot(normalDir, lightDir), 1.0f);

	vec3 diffuseOut = (diffuseComponent * diffuse) / (distance * distance);

	//ambient component
	vec3 ambientOut = ambient * 1.0f;

	vec3 result = (ambientOut + diffuseOut + specularOut) * lightIntensity * inColor;

	frag_color = vec4(result, 1.0f);
}