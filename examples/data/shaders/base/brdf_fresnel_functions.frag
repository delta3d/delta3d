#version 120
// External Functions
float saturate(float inValue);



vec3 Fresnel_None(vec3 specularColor)
{
    return specularColor;
}

// Used by Disney BRDF.
float Fresnel_Schlick(float u)
{
    float m = saturate( 1.0f - u);
    float m2 = m*m;
    return m2*m2*m;
}

vec3 Fresnel_Schlick(vec3 specularColor, vec3 h, vec3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

vec3 Fresnel_CookTorrance(vec3 specularColor, vec3 h, vec3 v)
{
    vec3 n = (1.0f + sqrt(specularColor)) / (1.0f - sqrt(specularColor));
    float c = saturate(dot(v, h));
    vec3 g = sqrt(n * n + c * c - 1.0f);

    vec3 part1 = (g - c)/(g + c);
    vec3 part2 = ((g + c) * c - 1.0f)/((g - c) * c + 1.0f);

    vec3 zeroVec;
    return max(zeroVec, 0.5f * part1 * part1 * ( 1 + part2 * part2));
}
