#version 120
//#extension GL_OES_standard_derivatives : enable

//used for pre depth calculations and soft particle opacity
uniform float d3d_NearPlane;
uniform float d3d_FarPlane;
uniform sampler2D d3d_PreDepthTexture;

//used for planar reflections
//it just needs some reasonable default so its never zero
uniform float ScreenHeight = 1024;
uniform float ScreenWidth = 768;

//reflections, planar and cube
uniform sampler2D reflectionMap;
uniform samplerCube d3d_ReflectionCubeMap;

//used for HDR
uniform float d3d_SceneLuminance = 1.0;
uniform float d3d_SceneAmbience = 1.0;
uniform float d3d_Exposure = 1.0;


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
   float diffuseContrib = d3d_SceneLuminance * diffuseSurfaceContrib;

   // Lit Color (Diffuse plus Ambient)
   vec3 diffuseLight = diffuseLightSource * diffuseContrib;
   lightContrib = vec3(diffuseLight + (d3d_SceneAmbience * ambientLightSource ));
}

void computeSpecularContribution(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 glossMap, out vec3 specularContribution)
{
   vec3 reflectVec = reflect(lightDir, normal);
   float reflectContrib = max(0.0,dot(reflectVec, -viewDir));
   specularContribution = vec3(glossMap.r) * (pow(reflectContrib, 16.0));
}

vec2 computeSphereMapCoord(in vec3 viewDir, in vec3 normal)
{
   float m;
   vec3 r,u;

   u = normalize(viewDir);
   r = reflect(u,normal);
   m = 2.0 * sqrt(dot(r.xy,r.xy) + ((r.z + 1.0) * (r.z + 1.0)));

   return vec2(r.x/m + 0.5,r.y/m + 0.5);
}


//A simple function to rotate a texture with a heading in degrees
vec2 rotateTexCoords(vec2 coords, float angle)
{
   float degInRad = radians(angle);   
   
   vec2 coordsRot;
   coordsRot.x = dot(vec2(cos(degInRad), -sin(degInRad)), coords);
   coordsRot.y = dot(vec2(sin(degInRad), cos(degInRad)), coords);
   return coordsRot;
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

float samplePreDepthTexture(vec2 fragCoord)
{
   vec2 depthCoords = vec2(fragCoord.x / ScreenWidth, fragCoord.y / ScreenHeight);
   return texture2D(d3d_PreDepthTexture, depthCoords).b * (d3d_FarPlane - d3d_NearPlane);
}

//From Shader X5
mat3 compute_tangent_frame_O3(vec3 N, vec3 p, vec2 uv)
{
    // Optimisation 3:
    // assume M is orthogonal

    // get edge vectors of the pixel triangle
    vec3 dp1 = vec3(1.0); //dFdx(p);
    vec3 dp2 = vec3(1.0); // dFdy(p);
    vec2 duv1 = vec2(1.0); // dFdx(uv);
    vec2 duv2 = vec2(1.0); // dFdy(uv);

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

vec3 samplePlanarReflectionTexture(vec2 fragCoord)
{
   vec2 refTexCoords = vec2(fragCoord.x / ScreenWidth, (fragCoord.y / ScreenHeight));      
   return texture2D(reflectionMap, refTexCoords).rgb;   
}

vec3 sampleCubeMapReflection(vec3 worldPos, vec3 camPos, vec3 normal)
{
   float dist = length(worldPos - camPos);
   
   //could this end up as a divide by zero ???
   vec3 wsViewDir = (worldPos - camPos) / dist;
   
   vec3 reflectCubeCoords = reflect(wsViewDir, normal);
   vec3 rayCol = worldPos + ((d3d_FarPlane - dist) * reflectCubeCoords);
   rayCol = normalize(rayCol - camPos);

   return textureCube(d3d_ReflectionCubeMap, rayCol).rgb;   
 }

vec3 sampleCubeMapReflectionLOD(vec3 worldPos, vec3 camPos, vec3 normal, float roughness)
{
   float dist = length(worldPos - camPos);
   
   //could this end up as a divide by zero ???
   vec3 wsViewDir = (worldPos - camPos) / dist;
   
   vec3 reflectCubeCoords = reflect(wsViewDir, normal);
   vec3 rayCol = worldPos + ((d3d_FarPlane - dist) * reflectCubeCoords);
   rayCol = normalize(rayCol - camPos);

   float mipIndex =  roughness * roughness * 8.0;
   return textureCube(d3d_ReflectionCubeMap, rayCol, mipIndex).rgb;   
 }

//From GPUGems 1 edited by Randima Fernando, ch1 article by Mark Finch
//a great fast approximation, use computeRefractCoef for a more physically based computation
float FastFresnel(float nDotL, float fbias, float fpow)
{
   float facing = 1.0 - nDotL;
   return max(fbias + ((1.0 - fbias) * pow(facing, fpow)), 0.0);
}


//From More OpenGL Programming with David Astle, chapter 8 article by Angus Dorbie
//a good refractIndex for water is 1.333
float computeReflectionCoef(vec3 viewDir, vec3 viewSpaceNormal, float refractIndex)
{
   float incidentAngle = acos(dot(viewDir, viewSpaceNormal));
   float refractAngle = asin(sin(incidentAngle) / refractIndex);
   
   float Rs = pow(sin(incidentAngle - refractAngle) /
                        sin(incidentAngle + refractAngle), 2.0 );

   float Rp = pow( (refractIndex * cos(incidentAngle) - cos(refractAngle) ) /
                        (refractIndex * cos(incidentAngle) + cos(refractAngle) ), 2.0);

   //this basically acts as a polarization filter
   return (Rs + Rp) * 0.5;
}


float computeExpFogWithDensity(float fogDistance, float fogDensity)
{
   //defaults to EXP2 Fog
    const float LOG2 = 1.442695;
    float fogFactor = exp2( -fogDensity * 
                       fogDensity * 
                       fogDistance * 
                       fogDistance * 
                       LOG2 );

    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}


float computeExpFog(float fogDistance)
{
   return computeExpFogWithDensity(fogDistance, gl_Fog.density);
}

float computeLinearFog(float startFog, float endFog, float fogDistance)
{
   float fogTemp = pow(2.0, (fogDistance - startFog) / (endFog - startFog)) - 1.0;
   return 1.0 - clamp(fogTemp, 0.0, 1.0);
}

