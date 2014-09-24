#version 120

uniform sampler2D diffuseTexture;

uniform float diffuseRadiance;
uniform float ambientRadiance;
uniform bool writeLinearDepth;
uniform float Intensity;

varying vec4 vertexColor;
varying float vDistance;

float computeFragDepth(float, float);

void main(void)
{
   float fragDepth = gl_FragCoord.z;
// HACK:   float fragDepth = computeFragDepth(vDistance, gl_FragCoord.z);
   gl_FragDepth = fragDepth;

   if(writeLinearDepth)
   {
      return;
   }

   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st); 
   
   //add in the nvg components
   vec3 diffuseLight = vec3(diffuseRadiance, gl_LightSource[1].diffuse.g, gl_LightSource[1].diffuse.b);
   vec3 lightContrib = Intensity * (diffuseLight + vec3(ambientRadiance, gl_LightSource[1].ambient.g, gl_LightSource[1].ambient.b));
   
   baseColor.xyz += lightContrib;

   gl_FragColor = baseColor * vertexColor;
}


