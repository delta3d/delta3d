#version 120

uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;
uniform float d3d_Exposure = 1.0;

const int MAX_DYNAMIC_LIGHTS = 25;
const int MAX_SPOT_LIGHTS = 10;

const int NUM_DYNAMIC_LIGHT_ATTRIBS = 3;
const int NUM_SPOT_LIGHT_ATTRIBS = 4;

uniform int NUM_DYNAMIC_LIGHTS_TO_USE = 0;
uniform int NUM_SPOT_LIGHTS_TO_USE = 0;

//each dynamic light has 3 associated vec4's
//the first vec4 is a vec3 position and an intensity
//the second vec4 is a vec3 color
//and the third vec4 is a vec3 attenuation and a saturation intensity
uniform vec4 d3d_DynamicLights[NUM_DYNAMIC_LIGHT_ATTRIBS * MAX_DYNAMIC_LIGHTS];



//each spot light has 4 associated vec4's
//the first vec4 is a vec3 position and an intensity
//the second vec4 is a vec3 color
//the third vec4 is a vec3 attenuation and a float spot exponent
//the fourth is a vec3 direction and float cos cutoff
uniform vec4 d3d_SpotLights[NUM_SPOT_LIGHT_ATTRIBS * MAX_SPOT_LIGHTS];


void dynamic_light_fragment(vec3 normal, vec3 pos, out vec3 totalLightContrib);
void spot_light_fragment(vec3 normal, vec3 pos, out vec3 totalLightContrib);


vec3 computeDynamicLightContrib(vec3 wsNormal, vec3 wsPos)
{
   vec3 dynamicLightContrib;
   dynamic_light_fragment(wsNormal, wsPos, dynamicLightContrib);

   vec3 spotLightContrib;
   spot_light_fragment(wsNormal, wsPos, spotLightContrib);
   dynamicLightContrib += spotLightContrib;

   return dynamicLightContrib;
}

void dynamic_light_fragment(vec3 normal, vec3 pos, out vec3 totalLightContrib)
{
   totalLightContrib = vec3(0.0,0.0, 0.0);

   for(int i = 0; i < NUM_DYNAMIC_LIGHTS_TO_USE * 3; i+=3)
   {      
      vec3 lightPos = d3d_DynamicLights[i].xyz;
     
      vec3 lightDir = lightPos - pos;
      
      float dist = length(lightDir);      
      float dist2 = dist * dist;
      
      //normalize, we already have the length
      lightDir /= dist;


      //this computes the attenuation which keeps the positional lights lighting within range of the light
      float atten = 1.0 / ( d3d_DynamicLights[i + 2].x + (d3d_DynamicLights[i + 2].y * dist) + (d3d_DynamicLights[i + 2].z * dist2));      
      float normalDotLight = max(0.0, dot(normal, lightDir));
            
      //we use 50% of the dot product lighting contribution and then add 50% of the ambient contribution
      //which is basically taken as just the light color
      vec3 dotProductLightingAndAmbient = 0.5 * d3d_SceneLuminance * ((d3d_SceneAmbience + normalDotLight) * d3d_DynamicLights[i+1].xyz);
         
      //we attenuate the resulting contribution of the dot product and ambient lighting,
      //multiply it by the intensity and then accumulate it into the resulting color
      float absIntensity = max(0.0, d3d_DynamicLights[i].w); //the intensity can be negative to support shadows on terrain
                                                         //but we have to clamp it here to keep from messing everything else up
      totalLightContrib +=  absIntensity * min(1.0, atten) * dotProductLightingAndAmbient; 
   } 
   
   totalLightContrib = d3d_SceneLuminance * clamp(totalLightContrib, 0.0, 1.0);
}



void spot_light_fragment(vec3 normal, vec3 pos, out vec3 totalLightContrib)
{
   totalLightContrib = vec3(0.0, 0.0, 0.0);  

   for(int i = 0; i < NUM_SPOT_LIGHTS_TO_USE * NUM_SPOT_LIGHT_ATTRIBS; i += NUM_SPOT_LIGHT_ATTRIBS)
   {      
      float spotCosCutoff = d3d_SpotLights[i + 3].w;
      float spotExponent = d3d_SpotLights[i + 2].w;
      vec3 spotPosition = vec3(d3d_SpotLights[i].xyz);
      vec3 spotDirection = normalize(d3d_SpotLights[i + 3].xyz);
      
      vec3 lightDir = spotPosition - pos;
      float dist = length(lightDir);
      float dist2 = dist * dist;
      lightDir /= dist;
      
      float spotEffect = dot(spotDirection, -lightDir);
      bool spotToggle = spotEffect > spotCosCutoff;
      float absIntensity = max(0.0, d3d_SpotLights[i].w); //the intensity can be negative to support shadows on terrain
                                                      //but we have to clamp it here to keep from messing everything else up

      spotEffect = max(0.0, (pow(spotEffect, spotExponent)));//just in case we want a negative cutoff we will do an abs here

      //this computes the attenuation which keeps the positional lights lighting within range of the light
      float attenDenom = d3d_SpotLights[i + 2].x + (d3d_SpotLights[i + 2].y * dist) + (d3d_SpotLights[i + 2].z * dist2);
      float atten = min(1.0, spotEffect / attenDenom);
      float normalDotLight = max(0.0, dot(normal, -spotDirection));//lightDir));

      //we use 65% of the dot product lighting contribution and then add 35% of the ambient contribution
      //which is basically taken as just the light color
      vec3 dotProductLightingAndAmbient = d3d_SpotLights[i+1].xyz * (0.65 * normalDotLight + 0.35);
         
      //we attenuate the resulting contribution of the dot product and ambient lighting,
      //multiply it by the intensity and then accumulate it into the resulting color

      // we don't add anything sometimes
      if (spotToggle && absIntensity > 0.0)
      {
         totalLightContrib += absIntensity * atten * dotProductLightingAndAmbient; 
      }
   } 
   
   totalLightContrib = d3d_SceneLuminance * clamp(totalLightContrib, 0.0, 1.0);
}



