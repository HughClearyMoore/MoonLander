#version 330

in vec2 vTexCoord;
out vec4 fragColour;

uniform sampler2D uTexture;

void main()
{
    fragColour = texture(uTexture, vTexCoord);
}

