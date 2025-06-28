#version 330

layout(location = 0) in vec3 aPos;

uniform mat4 uModel; // rotation matrix

out vec4 vertexColour;


void main()
{
    gl_Position = uModel * vec4(aPos * 0.1, 1.0);    

    vertexColour = vec4((aPos + vec3(1.0)) * 0.5, 1.0);
}