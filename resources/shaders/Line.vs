#version 120

uniform mat2 uMatrix;
uniform vec2 uOffset;
uniform float uExtScale;

attribute vec2 position;
attribute float distance;
attribute vec2 extVec;

varying float varDistance;

void main()
{
    gl_Position = vec4(uMatrix * (position + extVec * uExtScale) + uOffset, 0, 1);
    varDistance = distance;
}
