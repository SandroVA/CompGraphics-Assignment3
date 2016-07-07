
#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;

out vec3 outNormal;
out vec3 fragPosition;
out vec2 outUV;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position.x, position.y, position.z, 1.0);
	outNormal = vec3(model_matrix * vec4(normal, 0.0)); //rotate our normals
	fragPosition = vec3(model_matrix * vec4(position.x, position.y, position.z, 1.0));
	outUV = UV;
}