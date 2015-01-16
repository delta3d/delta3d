#version 120

uniform sampler2D diffuseTexture;
uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;
const float exposure = 5.0;


void main(void)
{
   vec3 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st).rgb;
   
   float luminance = ( diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114 );

   float brightness = ( 1.0 - exp(-luminance * exposure) );
   float scale = ( brightness / (luminance + 0.001) );
  
  
   vec3 finalColor = scale * diffuseColor * vec3(0.98, 1.0, 0.98);
   
   finalColor += pow(finalColor, vec3(4.0));
   
   float multiplier = 1.35;
  
   
   gl_FragColor = vec4(multiplier * finalColor,  1.0);
}

