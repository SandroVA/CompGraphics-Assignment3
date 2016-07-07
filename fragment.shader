#version 330 core

in vec3 outNormal;
in vec3 fragPosition;
in vec2 outUV;

out vec4 color;

uniform sampler2D Texture;
uniform vec4 selectedColor;
uniform bool useTexture;
uniform vec3 viewPos;

void main()
{
	vec3 lightColour = vec3(0.8f, 0.8f, 0.8f);
	vec3 lightPosition = vec3(1000.0f, 700.0f, 1000.0f);

	vec3 cubeColour = vec3(texture(Texture, outUV));
	vec3 resultantColour;

	//ambient lighting
	float ambientStrength = 0.4f;
	vec3 ambient_contribution = ambientStrength * lightColour;

	//diffuse lighting
	vec3 norm = normalize(outNormal);
	vec3 light_direction = normalize(lightPosition - fragPosition);
	float incident_degree = max(dot(norm, light_direction), 1.0f);
	vec3 diffuse_contribution = incident_degree * lightColour;

	//specular lighting
	float specularStrength = 0.2f;
	vec3 viewDir = normalize(viewPos - fragPosition);
	vec3 reflectDir = reflect(-light_direction, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.2), 1024);
	vec3 specular_contribution = specularStrength * spec * lightColour;

	float distance = length(lightPosition - fragPosition) / 400;

	if(useTexture)
	{
		resultantColour = ((ambient_contribution + diffuse_contribution) * cubeColour) / distance;
		color = vec4(resultantColour, 1.0f);
	}
	else
	{
		resultantColour = ((ambient_contribution + diffuse_contribution) * (vec3(selectedColor) * cubeColour)) / distance;
		color = vec4(resultantColour, 1.0f);
	}
} 