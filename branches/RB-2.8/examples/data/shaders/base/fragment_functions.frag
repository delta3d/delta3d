#version 120
//#extension GL_OES_standard_derivatives : enable

uniform float d3d_NearPlane;
uniform float d3d_FarPlane;
uniform sampler2D d3d_PreDepthTexture;

//used for HDR
uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;

float computeFragDepth(float distance)
{
   return (distance - d3d_NearPlane) / (d3d_FarPlane - d3d_NearPlane);
}

void alphaMix(vec3 color1, vec3 color2, float fogContrib, float alpha, out vec4 mixColor)
{
   mixColor = vec4( mix(color1, color2, fogContrib), alpha);
}

void lightContribution(vec3 normal, vec3 lightDir, vec3 diffuseLightSource, vec3 ambientLightSource, out vec3 lightContrib)
{
   float diffuseSurfaceContrib = max(dot(normal, lightDir),0.0);
   
   // Lit Color (Diffuse plus Ambient)
   vec3 diffuseLight = d3d_SceneLuminance * diffuseSurfaceContrib * diffuseLightSource;
   lightContrib = vec3(diffuseLight + (ambientLightSource * d3d_SceneAmbience));
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
   float fMin = d3d_NearPlane * vpLength / viewPosCurrent.z;
   float w = sqrt(radius * radius - dist * dist);
   float f = vpLength - w;
   float b = vpLength + w;
   float sceneDepth = texture2D(d3d_PreDepthTexture, screenCoord).r * (d3d_FarPlane - d3d_NearPlane);
   float ds = min(sceneDepth, b) - max(fMin, f);
   float sphereDepth = (1.0 - dist / radius) * ds;
   float opacity = 1.0 - exp(-density * sphereDepth);

   return opacity;
}

float samplePreDepthTexture(vec2 screenCoord)
{
   return texture2D(d3d_PreDepthTexture, screenCoord).b * (d3d_FarPlane - d3d_NearPlane);
}

//From Shader X5
mat3 compute_tangent_frame_O3(vec3 N, vec3 p, vec2 uv)
{
    // Optimisation 3:
    // assume M is orthogonal

    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    dp1 = normalize(dp1);
    dp2 = normalize(dp2);
    duv1 = normalize(duv1);
    duv2 = normalize(duv2);

    // solve the linear system
    // (not much solving is left going here)
    mat3 M = mat3(dp1, dp2,cross(dp1, dp2));
    vec3 T = M * vec3(duv1.x, duv2.x,0);
    vec3 B = M * vec3(duv1.y, duv2.y,0);

    // construct tangent frame 
    return mat3(normalize(T), normalize(B), normalize(N));
}

vec3 computeWorldSpaceNormal(vec3 vertPos, vec3 vertNormal, vec3 mapNormal, vec2 vertUV, out mat3 tbn)
{
   // Change the range from  [-1, 1] to [0, 1]
   vec3 tangentSpaceNormal = (2.0 * normalize(mapNormal.xyz)) - vec3(1.0, 1.0, 1.0);

   vec3 vn = normalize(vertNormal);
   tbn = compute_tangent_frame_O3(vn, vertPos, vertUV);
   
   // Transform the tangent space normal into view space
   return normalize(tbn * tangentSpaceNormal); 
}

float computeExpFog(float fogDistance)
{
   //defaults to EXP2 Fog
    const float LOG2 = 1.442695;
    float fogFactor = exp2( -gl_Fog.density * 
                       gl_Fog.density * 
                       fogDistance * 
                       fogDistance * 
                       LOG2 );

    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

float computeLinearFog(float startFog, float endFog, float fogDistance)
{
   float fogTemp = pow(2.0, (fogDistance - startFog) / (endFog - startFog)) - 1.0;
   return 1.0 - clamp(fogTemp, 0.0, 1.0);
}

