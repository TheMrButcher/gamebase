#version 330

uniform mat2 uMatrix;
uniform vec2 uOffset;
uniform float uExtScale;

layout (location = 0) in vec2 position;
layout (location = 1) in float distance;
layout (location = 2) in vec2 extVec;

out float varDistance;

void main()
{
    gl_Position = vec4(uMatrix * (position + extVec * uExtScale) + uOffset, 0, 1);
    varDistance = distance;
}
