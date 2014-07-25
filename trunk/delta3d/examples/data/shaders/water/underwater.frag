//////////////////////////////////////////////
//An under water shader
//by Bradley Anderegg
//////////////////////////////////////////////

uniform float WaterHeight;
uniform mat4 inverseViewMatrix;

varying vec4 worldSpacePos;
varying vec3 lightVector;
varying vec4 camPos;

float deepWaterScalar = 0.64;
float viewDistance = 10.0;

uniform vec4 WaterColor;
vec4 deepWaterColor = deepWaterScalar * WaterColor;

vec3 GetWaterColorAtDepth(float);
void lightContribution(vec3, vec3, vec3, vec3, out vec3);

void main (void)
{     

   float depth = WaterHeight - (viewDistance * worldSpacePos.z);
   depth = clamp(depth, 0.0, viewDistance);
   float depthScalar = (depth / viewDistance);

   vec3 lightContribFinal;
   lightContribution(vec3(0.0, 0.0, 1.0), lightVector, gl_LightSource[0].diffuse.xyz, 
      gl_LightSource[0].ambient.xyz, lightContribFinal);

   
   vec3 color = lightContribFinal * GetWaterColorAtDepth(camPos.z);
   color = mix(color, deepWaterColor.xyz, 0.5 * depthScalar);

   if(worldSpacePos.z < WaterHeight)
   {
      //gl_FragColor = vec4(vec3(lightContribFinal), 1.0);      
      gl_FragColor = vec4(color, 1.0);     
   }
   else
   {
      discard;
      //gl_FragColor = vec4(color, 0.0);   
   }
}
