#version 330

uniform mat2 uMatrix;
uniform vec2 uOffset;

layout (location = 0) in vec2 position;
layout (location = 1) in float distance;

out float varDistance;

void main()
{
    gl_Position = vec4(uMatrix * position + uOffset, 0, 1);
    varDistance = distance;
}
