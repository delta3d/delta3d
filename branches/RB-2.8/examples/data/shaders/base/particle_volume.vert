#version 120

const int MAX_NUM_PARTICLES = 150;

uniform mat4 osg_ViewMatrixInverse = mat4(1.0);

uniform vec4 volumeParticlePos[MAX_NUM_PARTICLES];
uniform float volumeParticleRadius;
uniform float volumeParticleDensity;

uniform mat4 inverseViewMatrix;

varying vec4 vOffset;
varying vec2 vTexCoords;
varying vec3 vLightContrib;
varying vec4 vViewPosCenter;
varying vec4 vViewPosVert;
varying vec3 vPos;
varying vec3 vNormal;
//varying vec3 vParticleOffset;

void lightContributionVertex(vec3, vec3, vec3, vec3, out vec3);
void dynamic_light_fragment(vec3, vec3, out vec3);
void spot_light_fragment(vec3, vec3, out vec3);

void main()
{
   mat4 temp = osg_ViewMatrixInverse;
   mat4 inverseViewMatrix = mat4(temp[0], temp[2], temp[1], temp[3]);
   
   //vParticleOffset = vec3(gl_Vertex.w, gl_Vertex.w*1.4, gl_Vertex.w*0.5);
   vOffset = volumeParticlePos[int(gl_Vertex.w)];
   
   vViewPosCenter = gl_ModelViewMatrix * vec4(vOffset.xyz, 1.0);
   vec3 center_pos = (inverseViewMatrix * vViewPosCenter).xyz;
   
   vViewPosVert = vViewPosCenter;
   vViewPosVert.xy += gl_Vertex.xy * volumeParticleRadius;

   vPos = (inverseViewMatrix * vViewPosVert).xyz;
   gl_Position = gl_ProjectionMatrix * vViewPosVert;

 
   vTexCoords = gl_MultiTexCoord0.xy;


   // Create a normal that faces the camera (in view space), change to world space
   mat3 inverseView3x3 = mat3(inverseViewMatrix[0].xyz, inverseViewMatrix[1].xyz, inverseViewMatrix[2].xyz);
   vec3 outwardNormal = inverseView3x3 * vec3(0.0, 0.0, 1.0);
   // Create a normal facing diagonally out at each vertex.
   vNormal = normalize(vPos.xyz - center_pos);
   // Make 2 sets of normals: facing 45 degrees toward and 45 degrees away from the eye.  
   vec3 awayNormal = (vNormal - outwardNormal)/2.0;
   vNormal = (vNormal + outwardNormal)/2.0;
   vNormal = normalize(vNormal);
 
   //Compute the Light Contribution
   vec3 dynamicLightContrib;
   vec3 spotLightContrib;
   vLightContrib = vec3(0.0, 0.0, 0.0);
   vec3 vLightDir = normalize(inverseView3x3 * gl_LightSource[0].position.xyz);
   vec3 sunLightContrib = vec3(0.0, 0.0, 0.0);

   // Calc light from sun, from dynamic area, and dynamic spot lights
   lightContributionVertex(vNormal, vLightDir, vec3(gl_LightSource[0].diffuse), vec3(gl_LightSource[0].ambient), sunLightContrib);
   // Blend part of the sunLightContrib with part of the real sun value (without normals)
   // based on the density of the particle. At 0.20 density, it'd be a 50/50 mix.
   sunLightContrib = mix(gl_LightSource[0].diffuse.rgb, sunLightContrib,
         (min(1.0, volumeParticleDensity*4.0)));
   dynamic_light_fragment(vNormal, vPos.xyz, dynamicLightContrib);
   spot_light_fragment(vNormal, vPos.xyz, spotLightContrib);
   vec3 frontSideLightContrib = sunLightContrib + dynamicLightContrib + spotLightContrib;

   // Do the same for the backside AWAYNORMAL - backside light is damped by the density
   dynamic_light_fragment(awayNormal, vPos.xyz, dynamicLightContrib);
   spot_light_fragment(awayNormal, vPos.xyz, spotLightContrib);
   vec3 backsideLightContrib = (1.0-volumeParticleDensity) * (sunLightContrib + dynamicLightContrib + spotLightContrib);

   // Use the best of the two
   vLightContrib = max(frontSideLightContrib, backsideLightContrib);  
   vLightContrib = clamp(vLightContrib, 0.0, 1.0);
}

