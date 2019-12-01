#version 120

const float PI = 3.141592653589793238462643383279;

uniform sampler2D envTexture;



uniform vec4 g_PBRValues;
uniform vec4 g_LightingParameters; // Light intensity, ambient light, reflection power.
uniform vec3 g_CameraPosition;

uniform vec4 g_BRDFParam1;
uniform vec4 g_BRDFParam2;

uniform vec4 g_UserAlbedo;
uniform vec4 g_UserSpecular;
uniform vec4 g_OverrideValues; // albedo, normal, metallic, roughness


//varying mat3 vTangentToWorld;

varying vec3 vNormal;



// External Functions
float saturate(float inValue);
float Fresnel_Schlick(float u);
vec3 Specular_F_Roughness(vec3 specularColor, float a, vec3 h, vec3 v);
vec3 Specular(vec3 specularColor, vec3 h, vec3 v, vec3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV);
vec3 computeWorldSpaceNormal(vec3 vertPos, vec3 vertNormal, vec3 mapNormal, vec2 vertUV, out mat3 tbn);



vec3 saturate(vec3 color)
{
   vec3 result;
   result.x = saturate(color.x);
   result.y = saturate(color.y);
   result.z = saturate(color.z);
   return result;
}

vec3 Diffuse(vec3 albedo)
{
    return albedo/PI;
}

float sqr(float x)
{
    return x*x;
}

float GTR2_aniso(float NdH, float HdX, float HdY, float ax, float ay)
{
    return 1.0f / (PI * ax*ay * sqr(sqr(HdX/ax) + sqr(HdY/ay) + NdH*NdH));
}

float smithG_GGX(float NdV, float alphaG)
{
    float a = alphaG*alphaG;
    float b = NdV*NdV;
    return 1.0f / (NdV + sqrt(a + b - a*b));
}

float GTR1(float NdH, float a)
{
    if (a >= 1.0f)
    {
        return 1.0f / PI;
    }

    float a2 = a*a;
    float t = 1.0f + (a2 - 1.0f) * NdH * NdH;
    return (a2 - 1.0f) / (PI*log(a2)*t);
}

/*vec3 ComputeLight(vec3 albedoColor,vec3 specularColor, vec3 normal, float roughness, vec3 lightPosition, vec3 lightColor, vec3 lightDir, vec3 viewDir)
{
    // Compute some useful values.
    float NdL = saturate(dot(normal, lightDir));
    float NdV = saturate(dot(normal, viewDir));
    vec3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    vec3 cDiff = Diffuse(albedoColor);
    vec3 cSpec = Specular(specularColor, h, viewDir, lightDir, a, NdL, NdV, NdH, VdH, LdV);

    return lightColor * NdL * (cDiff * (1.0f - cSpec) + cSpec);
}*/



// From Disney's BRDF explorer: https://github.com/wdas/brdf
vec3 ComputeBRDFResult(vec3 baseColor,out vec3 specularColor, vec3 normal, float roughness, float metallic, vec3 lightDir, vec3 viewDir, vec3 X, vec3 Y, out vec3 diffuse)
{
   const vec3 oneVec = vec3(1,1,1);
   float subsurface            = g_BRDFParam1.x;
   float specular              = g_BRDFParam1.y;
   float specularTint          = g_BRDFParam1.z;
   float anisotropic           = g_BRDFParam1.w;
   float sheen                 = g_BRDFParam2.x;
   float sheenTint             = g_BRDFParam2.y;
   float clearcoat             = g_BRDFParam2.z;
   float clearcoatGloss        = g_BRDFParam2.w;

    // Compute some useful values.
    float NdL = saturate(dot(normal, lightDir));
    float NdV = saturate(dot(normal, viewDir));

    vec3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float LdH = saturate(dot(lightDir, h));
    float a = max(0.001f, roughness * roughness);

    float luminance = 0.3f * baseColor.x + 0.6f * baseColor.y + 0.1f * baseColor.z;

    vec3 tint = luminance > 0.0f ? baseColor/luminance : oneVec; // Normalize luminance to isolate hue+sat.
    specularColor = mix(specular * 0.08f * mix(oneVec, tint, specularTint), baseColor, metallic);
    vec3 CSheen = mix(oneVec, tint, sheenTint);

    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float FL = Fresnel_Schlick(NdL);
    float FV = Fresnel_Schlick(NdV);
    float Fd90 = 0.5f + 2.0f * LdH * LdH * a;
    float Fd = mix(1.0f, Fd90, FL) * mix(1.0f, Fd90, FV);

    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
    float Fss90 = LdH * LdH * a;
    float Fss = mix(1.0f, Fss90, FL) * mix(1.0f, Fss90, FV);
    float ss = 1.25f * (Fss * (1.0f / (NdL + NdV + 0.0001f) - 0.5f) + 0.5f);

    // Specular
    float aspect = sqrt(1.0f - anisotropic*0.9f);
    float ax = max(0.001f, sqr(a)/aspect);
    float ay = max(0.001f, sqr(a)*aspect);
    float Ds = GTR2_aniso(NdH, dot(h, X), dot(h, Y), ax, ay);
    float FH = Fresnel_Schlick(LdH);
    vec3 Fs = mix(specularColor, oneVec, FH);
    float roughg = sqr(a*0.5f+0.5f);
    float Gs = smithG_GGX(NdL, roughg) * smithG_GGX(NdV, roughg);

    // Sheen
    vec3 Fsheen = FH * sheen * CSheen;

    // Clearcoat (ior = 1.5 -> F0 = 0.04)
    float Dr = GTR1(NdH, mix(0.1f, 0.001f, clearcoatGloss));
    float Fr = mix(0.04f, 1.0f, FH);
    float Gr = smithG_GGX(NdL, 0.25f) * smithG_GGX(NdV, 0.25f);
    //diffuse = ((1.0f/PI) * mix(Fd, ss, subsurface) * baseColor + Fsheen) * (1.0f - metallic);
    diffuse = ((1.0f/PI) * mix(Fd, ss, subsurface) * baseColor + Fsheen) * (1.0f - metallic);
    return (diffuse + Gs*Fs*Ds + 0.25f*clearcoat*Gr*Fr*Dr)*NdL;
}

struct BRDFFragParams
{
   vec3 position;
   vec3 normal; // Varied Surface Normal
   vec2 uv;
   float alpha;
   vec3 albedoColor;
   vec3 normalColor; // Normal Map Texel
   float metallic;
   float roughness;
   vec3 irradianceColor;
   vec3 illumColor;
   vec3 viewDirection;
   vec3 lightDirection;
   vec3 lightPosition;
   vec3 lightColor;
   bool lightIsPoint;
};

vec4 computeBRDF(BRDFFragParams params)
{
   vec3 oneVec = vec3(1,1,1);
   float pbrRoughness          = g_PBRValues.x;
   float pbrMetallic           = g_PBRValues.y;
   float overrideAlbedo        = g_OverrideValues.x;
   float overrideNormal        = g_OverrideValues.y;
   float overrideMetallic      = g_OverrideValues.z;
   float overrideRoughness     = g_OverrideValues.w;
   float overrideSpecular      = g_UserSpecular.w;
   float lightIntensity        = g_LightingParameters.x;
   float ambientLightIntensity = g_LightingParameters.y;
   float reflectionIntensity   = g_LightingParameters.z;
   
   vec3 pos = params.position;
   vec3 lightDir = params.lightDirection;
   vec3 lightColor = params.lightColor;
   vec3 viewDir = params.viewDirection;

   vec3 albedoColor = params.albedoColor;// * (1.0f - overrideAlbedo);
   vec3 normalColor = params.normalColor.rgb;
   float metallic = params.metallic;//  * (1.0f - overrideMetallic);
   float roughness = params.roughness;//  * (1.0f - overrideRoughness);
   vec3 irradiance = params.irradianceColor;
    
   mat3 tbn;
   vec3 normal = params.normal;
   normal = computeWorldSpaceNormal(pos, normalize(normal), normalize(normalColor), params.uv, tbn);
   
	
   // Gamma correction.
   vec3 expVec = oneVec * 2.2f;
//   albedoColor = pow(albedoColor.rgb, expVec);

   // Apply overrides.
//   roughness += pbrRoughness * overrideRoughness;
//   metallic += pbrMetallic * overrideMetallic;
//   albedoColor += g_UserAlbedo.xyz * overrideAlbedo;
//   normal += n * overrideNormal;

/*#ifdef USE_GLOSSINESS
    roughness = 1.0f - roughness;
#endif*/

/*#ifdef METALLIC
    // mix with metallic value to find the good diffuse and specular.
    vec3 realAlbedo = albedoColor.rgb - albedoColor.rgb * metallic;

    // 0.03 default specular value for dielectric.
    vec3 realSpecularColor = mix(vec3(0.03f,0.03f,0.03f), albedoColor.rgb, metallic);
/*#elif SPECULAR

    vec3 realAlbedo = albedoColor + g_UserAlbedo * overrideAlbedo;
    vec3 realSpecularColor = specularColor + g_UserSpecular.xyz * overrideSpecular;
#elif DISNEY_BRDF*/
    vec3 minVec = oneVec * 0.03f;
    vec3 realAlbedo = albedoColor + g_UserAlbedo.xyz * overrideAlbedo;
    vec3 realSpecularColor = mix(minVec, albedoColor, metallic); // TODO: Use disney specular color.
//#endif // METALLIC*/

    vec3 spec;
    vec3 diffuse;
    vec3 light1 = ComputeBRDFResult(albedoColor, spec, normal, roughness, metallic, lightDir, viewDir, tbn[0], tbn[1], diffuse);

    float attenuation = 0.001f;
    /*if (params.lightIsPoint)
    {
      float lightDist = length(-params.lightPosition + pos);
      attenuation = PI/(lightDist * lightDist);
    }*/

    //vec3 reflectVector = reflect( -viewDir, normal);
    vec3 reflectVector = reflect( viewDir, normal);

    float mipIndex =  roughness * roughness * 8.0f;

    vec3 envColor = texture2D(envTexture, reflectVector.xy).rgb;//textureLod(envTexture, reflectVector, mipIndex).rgb;
//    envColor = pow(envColor.rgb, expVec);

    vec3 envFresnel = Specular_F_Roughness(realSpecularColor, roughness * roughness, normal, viewDir);

    attenuation *= 0.1f;
    realAlbedo = saturate(diffuse);

    vec3 lightAndReflectContrib = attenuation * lightIntensity * light1 + envFresnel*envColor * reflectionIntensity;
    float avg = (lightAndReflectContrib.r + lightAndReflectContrib.g + lightAndReflectContrib.b)/3;
    float alpha = max(params.alpha, avg);
    alpha = saturate(alpha);
    vec4 result = vec4(lightAndReflectContrib + realAlbedo * irradiance * ambientLightIntensity, alpha);
    result.rgb += params.illumColor;
    return result;
}