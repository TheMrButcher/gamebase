#version 330

uniform vec4 uColor;

in float varDistance;
out vec4 FragColor;

void main()
{
    FragColor = vec4(uColor.r, uColor.g, uColor.b, uColor.a * varDistance);
}
