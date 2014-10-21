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

float deepWaterScalar = 0.85;
uniform float UnderWaterViewDistance;

uniform vec4 WaterColor;
vec4 deepWaterColor = deepWaterScalar * WaterColor;

vec3 GetWaterColorAtDepth(float);

void main (void)
{     

   float depth = waterHeightScreenSpace - (UnderWaterViewDistance * worldSpacePos.z);
   depth = clamp(depth, 0.0, UnderWaterViewDistance);
   float depthScalar = (depth / UnderWaterViewDistance);
   
   vec3 color = gl_LightSource[0].ambient.xyz * deepWaterColor.xyz;
   color += gl_LightSource[0].diffuse.xyz * deepWaterColor.xyz;
   
   if(worldSpacePos.z < 0.0)
   {
      gl_FragColor = vec4(color, 1.0);     
   }
   else
   {
      discard;
   }
}
