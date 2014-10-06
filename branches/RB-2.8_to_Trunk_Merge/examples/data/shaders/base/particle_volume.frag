#version 120

uniform sampler3D noiseTexture;
uniform float osg_SimulationTime;
uniform float d3d_NearPlane;
uniform float d3d_FarPlane;

uniform vec4 volumeParticleColor;
uniform float volumeParticleRadius;
uniform float volumeParticleIntensity;
uniform float volumeParticleDensity;
uniform float volumeParticleVelocity;
uniform float volumeParticleNoiseScale;
uniform vec4 ScreenDimensions;

varying vec4 vOffset;
varying vec2 vTexCoords;
varying vec3 vLightContrib;
varying vec4 vViewPosCenter;
varying vec4 vViewPosVert;
varying vec3 vParticleOffset;

varying vec3 vPos;
varying vec3 vNormal;

float softParticleOpacity(vec3 viewPosCenter, vec3 viewPosCurrent, 
      float radius, vec2 screenCoord, float density);


void main(void)
{
   vec2 radiusPosition = vTexCoords.xy * 2.0 - vec2(1.0, 1.0);
   // use r*r (vice r) using a dot, which drops the square root of length(). Works well.
   float r = dot(radiusPosition.xy, radiusPosition.xy); 
   if(r > 1.0) discard; // Eliminate processing on boundary pixels

   //to compute the noise texture coordinates, the x,y are relative to the x,y location relative to the center of the particle
   //the w component of the particle offset is a random number used to offset this so they are not all the same
   //we mod the w component by a small number to offset the speed as well
   float particleNoiseScaleOffset = mod(vOffset.w, 0.13);
   vec2 particleNoiseOffset = vec2(sin(vOffset.w), -cos(vOffset.w));
   vec3 noiseCoords = vec3((particleNoiseScaleOffset + volumeParticleNoiseScale) * vec2(radiusPosition.xy + particleNoiseOffset), osg_SimulationTime * (particleNoiseScaleOffset + 0.1));
   float noise = (texture3D(noiseTexture, noiseCoords)).a;
   
   if(noise < r) discard;
   else noise -= r;

   // soft particles avoid sharp cuts into main geometry
   float opacity = 1;//softParticleOpacity(vViewPosCenter.xyz, vViewPosVert.xyz, 
         //volumeParticleRadius, gl_FragCoord.xy / ScreenDimensions.xy, volumeParticleDensity);

   // use some noise in final color to keep it interesting (straight noise makes it too dark).
   vec3 finalColor = (0.6 + 0.6 * noise) * vLightContrib * volumeParticleColor.xyz;
   float finalAlpha = opacity * volumeParticleColor.w * volumeParticleIntensity * noise;
   gl_FragColor = vec4(finalColor,finalAlpha);
   
   vec3 debug = (vViewPosVert.xyz + vec3(1,1,1)) * 0.5;
   gl_FragColor = vec4(debug.xyz,finalAlpha);
}

