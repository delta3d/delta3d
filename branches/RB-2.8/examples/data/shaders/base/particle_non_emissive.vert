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

void main()
{
   gl_Position = ftransform();
   gl_TexCoord[0] = gl_MultiTexCoord0; 
   vertexColor = gl_Color;
   
   dynLightContrib = vec3(0.0, 0.0, 0.0);

   vDistance = calculateDistance(gl_ModelViewMatrix, gl_Vertex);
   
   worldPos = (osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
   
   mat3 inverseView3x3 = mat3(osg_ViewMatrixInverse[0].xyz, osg_ViewMatrixInverse[1].xyz, osg_ViewMatrixInverse[2].xyz);
   vec3 lightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);   
   
   vec3 psuedo_normal = vec3(0.0, 0.0, 1.0);
   
   dynLightContrib += vec3(gl_LightSource[0].ambient);
   dynLightContrib += vec3(gl_LightSource[0].diffuse * max(0.0, dot(lightDir, psuedo_normal)));
   
   for(int i = 0; i < NUM_DYNAMIC_LIGHTS_TO_USE * 3; i+=3)
   {      
      vec3 lightDir = vec3(dynamicLights[i].xyz) - worldPos;
      float dist = length(lightDir);      
      float dist2 = dist * dist;
      
      //this computes the attenuation which keeps the positional lights lighting within range of the light
      float atten = 1.0 / ( dynamicLights[i + 2].x + (dynamicLights[i + 2].y * dist) + (dynamicLights[i + 2].z * dist2));      
      float normalDotLight = max(0.0, dot(psuedo_normal, normalize(lightDir)));
            
      //we use 50% of the dot product lighting contribution and then add 50% of the ambient contribution
      //which is basically taken as just the light color
      vec3 dotProductLightingAndAmbient = 0.5 * ((1.0 + normalDotLight) * dynamicLights[i+1].xyz);
         
      dynLightContrib += dynamicLights[i].w * min(1.0, atten) * dotProductLightingAndAmbient; 
   } 
   dynLightContrib = clamp(dynLightContrib, 0.0, 1.0);
}
