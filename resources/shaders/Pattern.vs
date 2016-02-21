#version 120

uniform mat2 uMatrix;
uniform vec2 uOffset;
uniform vec2 uTexCoordsOffset;

attribute vec2 position;
attribute vec2 textureCoords;

varying vec2 varTextureCoords;

void main()
{
    gl_Position = vec4(uMatrix * position + uOffset, 0, 1);
	varTextureCoords = textureCoords - uTexCoordsOffset;
}
