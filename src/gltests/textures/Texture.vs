#version 330

uniform mat2 uMatrix;
uniform vec2 uOffset;

layout (location = 0) in vec2 position;
layout (location = 3) in vec2 textureCoords;

out vec2 varTextureCoords;

void main()
{
    gl_Position = vec4(uMatrix * position + uOffset, 0, 1);
	varTextureCoords = textureCoords;
}
