#version 120
//////////////////////////////////////////////
//This shader is used to color the far plane with 
// the underwater color to give the effect of an ocean
//by Bradley Anderegg
//////////////////////////////////////////////
uniform float WaterHeight;
uniform mat4 inverseViewMatrix;

varying vec4 worldSpacePos; 

float deepWaterScalar = 0.85;
uniform float UnderWaterViewDistance;

uniform vec4 WaterColor;
vec4 deepWaterColor = deepWaterScalar * WaterColor;

vec3 GetWaterColorAtDepth(float);

void main (void)
{     

   float depth = (UnderWaterViewDistance * worldSpacePos.z);
   depth = clamp(depth, 0.0, UnderWaterViewDistance);
   float depthScalar = (depth / UnderWaterViewDistance);
   
   vec3 color = gl_LightSource[0].ambient.xyz * deepWaterColor.xyz;
   color += gl_LightSource[0].diffuse.xyz * deepWaterColor.xyz;
   
   if(worldSpacePos.z < WaterHeight)
   {
      gl_FragColor = vec4(color, 1.0);     
   }
   else
   {
      discard;
   }
}
