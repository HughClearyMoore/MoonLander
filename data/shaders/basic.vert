#version 330

layout(location = 0) in vec3 aPos;

uniform mat4 uModelViewProj;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec4 vertexColour;


void main()
{
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);    

    vertexColour = vec4((aPos + vec3(1.0)) * 0.5, 1.0);
}