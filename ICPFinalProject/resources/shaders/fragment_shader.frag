#version 330 core
struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float     shininess; // shininess of the material
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant; // constants for distance - llight
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1   // how many pointlights in the scene
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};


in vec2 TexCoords;
in vec3 Normal; // Normal vectors of the objects
in vec3 FragPos;  

out vec4 FragColor; // Output fragments


//uniform sampler2D texture_diffuse1;
//uniform sampler2D ourTexture;
uniform vec3 viewPos; // position of the view
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;

// functions declaration
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // define an output color value
    vec3 output = vec3(0.0);

    // add the directional light's contribution to the output
    output += CalcDirLight(dirLight, norm, viewDir);

    // do the same for all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
  	    output += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // you can implement: and add others lights as well (like spotlights)
    output += CalcSpotLight(spotLight, norm, FragPos, viewDir); 

    FragColor = vec4(output, 1.0);

    //FragColor = texture(texture_diffuse1, TexCoord);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

     vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);

}