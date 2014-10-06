
uniform float WaterHeight;
uniform vec4 WaterColor;

const float cDeepWaterScalar = 0.64;
const float cViewDistance = 100.0; 

vec3 GetWaterColorAtDepth(float pDepth)
{
   vec3 cDeepWaterColor = cDeepWaterScalar * WaterColor.xyz;
   float dist = WaterHeight - pDepth;
   dist = clamp(dist, 0.0, cViewDistance);
   float depthScalar = (dist / cViewDistance);

   vec3 color = mix(WaterColor.xyz, cDeepWaterColor, depthScalar);

   return color;
}
