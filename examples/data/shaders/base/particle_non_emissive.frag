#version 120

uniform sampler2D diffuseTexture;

uniform float diffuseRadiance;
uniform float ambientRadiance;
uniform bool writeLinearDepth;
uniform float aboveWaterOnly = 0.0;
uniform float WaterHeight = 0.0;

uniform float Intensity;

varying vec3 dynLightContrib;
varying vec4 vertexColor;
varying float vDistance;
varying vec3 worldPos;

float computeFragDepth(float, float);


void main(void)
{
   if (aboveWaterOnly > 0.0 && worldPos.z - WaterHeight < 0.0)
      discard;
      
   float fragDepth = gl_FragCoord.z;
// HACK:   float fragDepth = computeFragDepth(vDistance, gl_FragCoord.z);
   gl_FragDepth = fragDepth;

   //currently we only write a linear depth when doing a depth pre-pass
   //as an optimization we return immediately afterwards
   if(writeLinearDepth)
   {
      return;
   }

   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st); 
   
   //add in the nvg components
   vec3 diffuseLight = vec3(diffuseRadiance, gl_LightSource[1].diffuse.g, gl_LightSource[1].diffuse.b);
   vec3 lightContrib = diffuseLight + vec3(ambientRadiance, gl_LightSource[1].ambient.g, gl_LightSource[1].ambient.b);
   
   lightContrib += dynLightContrib;
   
   lightContrib = clamp(lightContrib, 0.0, 1.0);
   
   baseColor.xyz *= lightContrib;
   gl_FragColor = baseColor * vertexColor;
}

