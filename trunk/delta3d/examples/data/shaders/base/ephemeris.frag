#version 120

varying float vFog;


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

   //add fog band to blend with distant geometry
   float sunEffect = length(multiplier * finalColor);
   sunEffect = clamp(sunEffect, 0.0, exposure) / exposure;
   vec3 fogColor = gl_Fog.color.rgb;// * min(1.0, length(finalColor));;
   
   gl_FragColor = vec4( mix(multiplier * finalColor, fogColor, vFog - sunEffect), 1.0);
}

