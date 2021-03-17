#version 400

uniform vec4 CurrentColor;

in vec3 fragmentColor;

out vec4 out_Color;

void main(void)
{
    out_Color = vec4(fragmentColor,1.0);
}