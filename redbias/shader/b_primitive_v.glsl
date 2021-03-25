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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations 

void main(){

   	mat4 BoneTransform = gBones[ VertID[0] ] * VertWeight[0];
	BoneTransform += gBones[ VertID[1] ] * VertWeight[1];
    BoneTransform += gBones[ VertID[2] ] * VertWeight[2];
    BoneTransform += gBones[ VertID[3] ] * VertWeight[3];

    // FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = mat3(transpose(inverse(model))) * aNormal;  

    // normal
    vec4 tNormal = BoneTransform * vec4(aNormal, 1.0);
    // vec4 tNormal = vec4(aNormal, 1.0);
    
    mat3 NormalMatrix = mat3(transpose(inverse(model)));
    Normal = normalize(mat4(NormalMatrix) * tNormal).xyz;

    // position
    vec4 tpos = BoneTransform * vec4(aPos, 1.0);
    // vec4 tpos = vec4(aPos, 1.0);

    gl_Position = (projection * view * model) * tpos;

    // out
    TexCoords = aTexCoords;
    ID = VertID;
    Weight = VertWeight;    
    FragPos = (model * tpos).xyz;
}