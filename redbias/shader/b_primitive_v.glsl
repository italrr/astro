#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bittanget;
layout (location = 5) in ivec4 VertID;
layout (location = 6) in vec4 VertWeight;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out ivec4 ID;
out vec4 Weight;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bones[MAX_BONES];

void main(){

    mat4 bMatrix = bones[ID[0]] * Weight[0];
    bMatrix += bones[ID[1]] * Weight[1];
    bMatrix += bones[ID[2]] * Weight[2];
    bMatrix += bones[ID[3]] * Weight[3];

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    ID = VertID;
    Weight = VertWeight;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}