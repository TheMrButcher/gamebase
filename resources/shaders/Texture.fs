#version 330

uniform sampler2D uSampler;
uniform vec4 uColor;

in vec2 varTextureCoords;
out vec4 FragColor;

void main()
{
    FragColor = texture2D(uSampler, varTextureCoords) * uColor;
}
