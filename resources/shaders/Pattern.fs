#version 120

uniform sampler2D uSampler;
uniform vec4 uColor;

varying vec2 varTextureCoords;

void main()
{
    gl_FragColor = texture2D(uSampler, varTextureCoords) * uColor;
}
