#version 120

float Geometric_Implicit(float a, float NdV, float NdL)
{
    return NdL * NdV;
}

float Geometric_Neumann(float a, float NdV, float NdL)
{
    return (NdL * NdV) / max(NdL, NdV);
}

float Geometric_CookTorrance(float a, float NdV, float NdL, float NdH, float VdH)
{
    return min(1.0f, min((2.0f * NdH * NdV)/VdH, (2.0f * NdH * NdL)/ VdH));
}

float Geometric_Kelemen(float a, float NdV, float NdL, float LdV)
{
    return (2 * NdL * NdV) / (1 + LdV);
}

float Geometric_Beckman(float a, float dotValue)
{
    float c = dotValue / ( a * sqrt(1.0f - dotValue * dotValue));

    if ( c >= 1.6f )
    {
        return 1.0f;
    }
    else
    {
        float c2 = c * c;
        return (3.535f * c + 2.181f * c2) / ( 1 + 2.276f * c + 2.577f * c2);
    }
}

float Geometric_Smith_Beckmann(float a, float NdV, float NdL)
{
    return Geometric_Beckman(a, NdV) * Geometric_Beckman(a, NdL);
}

float Geometric_GGX(float a, float dotValue)
{
    float a2 = a * a;
    return (2.0f * dotValue) / (dotValue + sqrt(a2 + ((1.0f - a2) * (dotValue * dotValue))));
}

float Geometric_Smith_GGX(float a, float NdV, float NdL)
{
    return Geometric_GGX(a, NdV) * Geometric_GGX(a, NdL);
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
        // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}
