#version 120

uniform int g_specDistributionMode;
uniform int g_specFresnelMode;
uniform int g_specGeometricMode;

// Specular Distribution Modes
// 0 - Blinn Phong
// 1 - Beckmann
// 2 - NDF GGX

// Specular Fresnel Modes
// 0 - NONE
// 1 - Schlick
// 2 - Cooktorrance

// Specular Geometric Modes
// 0 - Implicit
// 1 - Neumann
// 2 - Cooktorrance
// 3 - Kelemen
// 4 - Beckmann
// 5 - GGX
// 6 - Schlick GGX


// External Functions
float saturate(float inValue);
vec3 Fresnel_None(vec3 specularColor);
float Fresnel_Schlick(float u);
vec3 Fresnel_Schlick(vec3 specularColor, vec3 h, vec3 v);
vec3 Fresnel_CookTorrance(vec3 specularColor, vec3 h, vec3 v);
float Geometric_Implicit(float a, float NdV, float NdL);
float Geometric_Neumann(float a, float NdV, float NdL);
float Geometric_CookTorrance(float a, float NdV, float NdL, float NdH, float VdH);
float Geometric_Kelemen(float a, float NdV, float NdL, float LdV);
float Geometric_Beckman(float a, float dotValue);
float Geometric_Smith_Beckmann(float a, float NdV, float NdL);
float Geometric_GGX(float a, float dotValue);
float Geometric_Smith_GGX(float a, float NdV, float NdL);
float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL);
float NormalDistribution_GGX(float a, float NdH);
float NormalDistribution_BlinnPhong(float a, float NdH);
float NormalDistribution_Beckmann(float a, float NdH);



float Specular_D(float a, float NdH)
{
   if (g_specDistributionMode == 0)
   {
      return NormalDistribution_BlinnPhong(a, NdH);
   }
   else if (g_specDistributionMode == 1)
   {
      return NormalDistribution_Beckmann(a, NdH);
   }

   // Return something.
   return NormalDistribution_GGX(a, NdH);
}

vec3 Specular_F(vec3 specularColor, vec3 h, vec3 v)
{
   if (g_specFresnelMode == 0)
   {
      return Fresnel_None(specularColor);
   }
   else if (g_specFresnelMode == 1)
   {
      return Fresnel_Schlick(specularColor, h, v);
   }
   
   // Return something.
   return Fresnel_CookTorrance(specularColor, h, v);
}

vec3 Specular_F_Roughness(vec3 specularColor, float a, vec3 h, vec3 v)
{
    
   if (g_specFresnelMode == 0)
   {
      return Fresnel_None(specularColor);
   }
   else if (g_specFresnelMode == 1)
   {
      float value = 1.0 - a;
      vec3 oneVecMinusA = vec3(value, value, value);
      return (specularColor + (max(oneVecMinusA, specularColor) - specularColor) * pow((1 - saturate(dot(v, h))), 5));
   }
   
   // Return something.
   return Fresnel_CookTorrance(specularColor, h, v);
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV)
{
   if (g_specGeometricMode == 0)
   {
      return Geometric_Implicit(a, NdV, NdL);
   }
   else if (g_specGeometricMode == 1)
   {
      return Geometric_Neumann(a, NdV, NdL);
   }
   else if (g_specGeometricMode == 2)
   {
      return Geometric_CookTorrance(a, NdV, NdL, NdH, VdH);
   }
   else if (g_specGeometricMode == 3)
   {
      return Geometric_Kelemen(a, NdV, NdL, LdV);
   }
   else if (g_specGeometricMode == 4)
   {
      return Geometric_Smith_Beckmann(a, NdV, NdL);
   }
   else if (g_specGeometricMode == 5)
   {
      return Geometric_Smith_GGX(a, NdV, NdL);
   }
   
   // Return something.
   return Geometric_Smith_Schlick_GGX(a, NdV, NdL);
}

vec3 Specular(vec3 specularColor, vec3 h, vec3 v, vec3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
    return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h) ) / (4.0f * NdL * NdV + 0.0001f);
}
