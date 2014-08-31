#version 120

uniform sampler2D diffuseTexture;
uniform float d3d_SceneLuminance; // = 1.0;

const float exposure = 5.0;

void main(void)
{
   vec3 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st).rgb;
   
   float luminance = ( diffuseColor.r * 0.299 + diffuseColor.g * 0.587 + diffuseColor.b * 0.114 );

   float brightness = ( 1.0 - exp(-luminance * exposure) );
   float scale = d3d_SceneLuminance * ( brightness / (luminance + 0.001) );
  
  
   vec3 finalColor = scale * diffuseColor * vec3(0.85, 0.9, 0.85);
   //finalColor += diffuseColor;
   
   finalColor += 0.5 * pow(finalColor, vec3(1.5));
   finalColor += 0.5 * pow(finalColor, vec3(2.5));

   
   gl_FragColor = vec4(finalColor,  1.0);
}

