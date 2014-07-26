// The following shaders make use of it least one of this functions:
//   ephemeris_fog_frag.glsl
//   perpixel_lighting_one_directional_frag.glsl
//   perpixel_lighting_detailmap_frag.glsl
//   shader_gloss_reflect_pixel.glsl
//   shader_gloss_reflect_pixel_hmmwv_interior.glsl
//   shader_nogolss_pixel.glsl
//   terrain_fragment.glsl


/////////////////////////////////////////////////////////////////////
///////////General Functions Used By Fragment Fog Shaders////////////
/////////////////////////////////////////////////////////////////////
uniform bool writeLinearDepth;
uniform float nearPlane;
uniform float farPlane;
uniform sampler2D depthTexture;

float computeFragDepth(float distance, float fragZ)
{
   float fragDepth = fragZ;
   if(writeLinearDepth)
   {
      fragDepth = (distance - nearPlane) / (farPlane - nearPlane);
   }
   return fragDepth;
}

void alphaMix(vec3 color1, vec3 color2, float fogContrib, float alpha, out vec4 mixColor)
{
   mixColor = vec4( mix(color1, color2, fogContrib), alpha);
}

/////////////////////////////////////////////////////////////////////
// Computes the light color based on the normal of the light and the surface.
// It also considers that just using the normal is too harsh, so we blend in the 
// light value with an imaginary up vector. Pass in the norms, light dir, color of the 
// light (diffuse and ambient). Out is the final light color. 
// Returns the diffuse contrib value (0.0 to 1.0). The modified dot of the norm and light dir
void lightContribution(vec3 normal, vec3 lightDir, vec3 diffuseLightSource, vec3 ambientLightSource, out vec3 lightContrib)
{
   // Light contribution considers the light impacting the surface. Since that 
   // is too dramatic, we weight the effect of light against a straight up vector.
   float diffuseSurfaceContrib = max(dot(normal, lightDir),0.0);
   float fUpContribution = max(dot(vec3(0.0, 0.0, 1.0), lightDir), -0.1);
   fUpContribution = (fUpContribution + 0.1) / 1.1;  // we use a bit past horizontal, else it darkens too soon.
   float diffuseContrib = fUpContribution * 0.52 + diffuseSurfaceContrib * 0.52;

   // Lit Color (Diffuse plus Ambient)
   vec3 diffuseLight = diffuseLightSource * diffuseContrib;
   lightContrib = vec3(diffuseLight + ambientLightSource);
}

void computeSpecularContribution(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 glossMap, out vec3 specularContribution)
{
   vec3 reflectVec = reflect(lightDir, normal);
   float reflectContrib = max(0.0,dot(reflectVec, -viewDir));
   specularContribution = vec3(glossMap.r) * (pow(reflectContrib, 16.0));
}

/////////////////////////////////////////////////////////////////////
// Determines the opacity of a spherical soft particle (drawn as a billboard). 
// The center is fully opaque (1.0), the edges are fully transparent (0.0)
// This technique is derived from the article, "Spherical Billboards for 
// Rendering Volumetric Data" in Shader X5.
/////////////////////////////////////////////////////////////////////
float softParticleOpacity(vec3 viewPosCenter, vec3 viewPosCurrent, 
      float radius, vec2 screenCoord, float density)
{
   float dist = length(viewPosCenter.xy - viewPosCurrent.xy);
   float vpLength = radius + length(viewPosCurrent);
   float fMin = nearPlane * vpLength / viewPosCurrent.z;
   float w = sqrt(radius * radius - dist * dist);
   float f = vpLength - w;
   float b = vpLength + w;
   float sceneDepth = texture2D(depthTexture, screenCoord).r * (farPlane - nearPlane);
   float ds = min(sceneDepth, b) - max(fMin, f);
   float sphereDepth = (1.0 - dist / radius) * ds;
   float opacity = 1.0 - exp(-density * sphereDepth);

   return opacity;
}

