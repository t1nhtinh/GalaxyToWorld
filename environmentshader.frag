#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
in vec3 Normal;
in vec3 Position;
in vec3 v;


out vec4 FragColor;

//uniform samplerCube skybox;
//uniform sampler1D edgeramp;
uniform vec3 camPos;

//Helper Methods
vec3 outlineEdge();
vec3 diffuse(vec3,vec3,vec3);
vec3 specular(vec3,vec3,vec3,vec3);
float discretizeShading(float);

void main()
{
  vec3 lightPos = vec3(0);

  vec3 diff = diffuse( Normal, Position, lightPos);
  vec3 spek = specular(camPos,Position,normalize(lightPos - Position),Normal);
  vec3 edgeLighting = outlineEdge();
  FragColor = vec4(edgeLighting * ( diff + spek), 1.0f);
}

vec3 specular(vec3 viewPos,vec3 fragPos, vec3 lightDir, vec3 norm){
    vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normalize(norm));
	float result = pow(max(dot(viewDir, reflectDir),0.0),16f);

    vec3 lightColor = vec3(1.0f);
	vec3 materialSpec= vec3(0.5f,0.5f,0.5f);

	result = discretizeShading(result);
	return lightColor * (result * materialSpec);
}

vec3 diffuse(vec3 norm, vec3 fragPos, vec3 lightPos){
    norm = normalize(norm);
    vec3 lightD = normalize(lightPos - fragPos);
    float result = max(dot(norm,lightD),0.0f);

    vec3 lightColor = vec3(0.5f);
	vec3 materialDiffuse = vec3(1.0f, 0.5f, 0.31f);

	result = discretizeShading(result);
	return lightColor * (result * materialDiffuse);
}

vec3 outlineEdge( ) {
    vec3 viewDir = normalize(camPos - Position);
	float edge = max(dot(normalize(Normal),viewDir), 0.0f);
	vec3 intensity;

	edge = discretizeShading(edge);
	intensity = vec3(edge);
	return intensity;
}
float discretizeShading(float result){
	if(result < 0.01){
	 result = 0.0f;
	} else if(result > 0.01 && result < 0.76) {
	  result = 0.50f;
	 /*
	} else if(result > 0.01 && result < 0.26) {
	  result = 0.10f;
	} else if(result > 0.26 && result < 0.51) {
	  result = 0.20f;
	} else if(result > 0.50 && result < 0.76) {
	  result = 0.30f;
	  */
	} else if(result > 0.76) {
	  result = 1.0f;
	}

	return result;
}