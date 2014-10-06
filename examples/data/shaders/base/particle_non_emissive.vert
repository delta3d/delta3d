#version 120

// Constants defined in dynamic_light_functions.frag
const int MAX_DYNAMIC_LIGHTS = 25;
uniform int NUM_DYNAMIC_LIGHTS_TO_USE = 0;

//each dynamic light has 3 associated vec4's
//the first vec4 is a vec3 position and an intensity
//the second vec4 is a vec3 color
//and the third vec4 is a vec3 attenuation and a saturation intensity
uniform vec4 dynamicLights[3 * MAX_DYNAMIC_LIGHTS];
uniform mat4 inverseViewMatrix;

uniform mat4 osg_ViewMatrixInverse = mat4(1.0);

varying vec3 dynLightContrib;
varying vec3 worldNormal;
varying vec3 worldPos;
varying vec4 vertexColor;
varying float vDistance;

float calculateDistance(mat4, vec4);
float GetHeightOnWaterSuface(vec2 point);
vec3 computeDynamicLightContrib(vec3 wsNormal, vec3 wsPos);

void main()
{
   gl_Position = ftransform();
   gl_TexCoord[0] = gl_MultiTexCoord0; 
   vertexColor = gl_Color;
   
   dynLightContrib = vec3(0.0, 0.0, 0.0);

   vDistance = calculateDistance(gl_ModelViewMatrix, gl_Vertex);
   
   worldPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);
   vec3 lightDir0 = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);   
   vec3 lightDir1 = normalize(inverseView3x3 * gl_LightSource[1].position.xyz);   
   
   vec3 psuedo_normal = vec3(0.0, 0.0, 1.0);
   
   dynLightContrib += vec3(gl_LightSource[0].ambient);
   dynLightContrib += vec3(gl_LightSource[0].diffuse * max(0.0, dot(lightDir0, psuedo_normal)));
   
   dynLightContrib += vec3(gl_LightSource[1].ambient);
   dynLightContrib += vec3(gl_LightSource[1].diffuse * max(0.0, dot(lightDir1, psuedo_normal)));
   
   dynLightContrib += computeDynamicLightContrib(psuedo_normal, worldPos);
      
   dynLightContrib = clamp(dynLightContrib, 0.0, 1.0);
}
