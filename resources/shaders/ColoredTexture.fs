#version 120

uniform sampler2D uSampler;

varying vec2 varTextureCoords;
varying vec4 varColor;

void main()
{
    gl_FragColor = texture2D(uSampler, varTextureCoords) * varColor;
}
