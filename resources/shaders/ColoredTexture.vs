#version 120

uniform mat2 uMatrix;
uniform vec2 uOffset;

attribute vec2 position;
attribute vec2 textureCoords;
attribute vec4 color;

varying vec2 varTextureCoords;
varying vec4 varColor;

void main()
{
    gl_Position = vec4(uMatrix * position + uOffset, 0, 1);
	varTextureCoords = textureCoords;
	varColor = color;
}
