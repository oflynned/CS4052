#version 330 core

in LIGHTING {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
} lighting;

uniform sampler2D texture_diffuse1;

out vec4 out_color;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vec4 modelTexture = texture(texture_diffuse1, lighting.TexCoords);
	vec3 color = modelTexture.rgb;
	
	// ambient
	vec3 ambient = 0.01f * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - lighting.Position);
	vec3 normal = normalize(lighting.Normal);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * color;

	// specular
	vec3 viewDir = normalize(viewPos - lighting.Position);
	vec3 reflectDir = reflect(-lightDir, normal);

	// blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.3f) * spec;
	out_color = vec4(ambient + diffuse + specular, 1.0f);
}