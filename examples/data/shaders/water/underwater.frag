#version 120
//////////////////////////////////////////////
//An under water shader
//by Bradley Anderegg
//////////////////////////////////////////////

uniform float waterHeightScreenSpace;
uniform mat4 inverseViewMatrix;

varying vec4 worldSpacePos;
varying vec3 lightVector;
varying vec4 camPos;

float deepWaterScalar = 0.64;
float viewDistance = 10.0;

uniform vec4 WaterColor;
vec4 deepWaterColor = deepWaterScalar * WaterColor;

vec3 GetWaterColorAtDepth(float);

void main (void)
{     

   float depth = waterHeightScreenSpace - (viewDistance * worldSpacePos.z);
   depth = clamp(depth, 0.0, viewDistance);
   float depthScalar = (depth / viewDistance);
   
   vec3 color = gl_LightSource[0].ambient.xyz * deepWaterColor.xyz;
   color += (gl_LightSource[0].diffuse.xyz * mix(deepWaterColor.xyz, color, 1.25 * depthScalar));

   if(worldSpacePos.z < waterHeightScreenSpace)
   {
      gl_FragColor = vec4(color, 1.0);     
   }
   else
   {
      //gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);     

      discard;
   }
}
