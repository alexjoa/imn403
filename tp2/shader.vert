#version 400

layout(location = 1) in vec3 vertexColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_Position;

out vec3 fragmentColor;

void main(void)
{
     gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);
	 
	 fragmentColor = vertexColor;
}