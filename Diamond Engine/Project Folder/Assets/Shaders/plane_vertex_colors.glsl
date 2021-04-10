#ifdef vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 6) in vec3 colors;

out vec3 fPosition;
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

out vec3 vertexColor;
out vec3 diffuseColor;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPosition;
vec3 lightColor = vec3(0.55, 0.35, 0.35);

void main()
{
    vertexColor = colors;
    
    vs_out.FragPos = vec3(model_matrix * vec4(position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model_matrix))) * normals;
    vs_out.TexCoords = texCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	vec3 fPosition = (model_matrix * vec4(position, 1.0)).xyz;
    vec3 lightDirection = normalize(vec3(lightPosition - fPosition));
    diffuseColor = vec3(max(dot(lightDirection, normals), 0) * lightColor);

    gl_Position = projection * view * model_matrix * vec4(position, 1.0);

}
#endif

#ifdef fragment
#version 330 core

out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

in vec3 vertexColor; 
in vec3 diffuseColor; 

uniform vec3 altColor;
uniform sampler2D shadowMap;

vec3 ambientLight = vec3(0.30, 0.15, 0.10);

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

out vec4 color;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    if(projCoords.z > 1.0)
    	return 0.0;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return shadow;
}

void main()
{
    vec3 color = altColor;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = vec3(0.18, 0.11, 0.12);
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir, fs_in.Normal);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0) * mix (vec4(diffuseColor + ambientLight, 1.0), vec4(vertexColor, 1.0), 0.7);
    //color = mix (vec4(diffuseColor + ambientLight, 1.0), vec4(vertexColor, 1.0), 0.7);
}
#endif


