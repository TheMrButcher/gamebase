#version 120

uniform vec4 uColor;

varying float varDistance;

void main()
{
    gl_FragColor = vec4(uColor.r, uColor.g, uColor.b, uColor.a * varDistance);
}
