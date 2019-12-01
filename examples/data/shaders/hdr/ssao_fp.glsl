/*
 * SSAO - Technique.
 * This shader is applied on the screen sized quad with scene rendered before as input.
 */

// data containg just random values between 0 and 1
uniform vec4 fk3f[32];

// input textures with normal scene view and linearized depth values
uniform sampler2D texColorMap;
uniform sampler2D texNormalDepthMap;

// size of the input texture 
uniform float osgppu_ViewportWidth;
uniform float osgppu_ViewportHeight;
uniform vec2  invResolution;

// parameters for the ssao
const float rad = 0.5;

// camera parameter
uniform float zNear;
uniform float zFar;
uniform vec2 invFocalLength;
uniform vec2 focalLength;

// quality mode of this shader
const int qualityMode = 1;
const bool useNormal = true;

/**
 * Convert z-depth value into camera distance coordinate
 **/
float convertZ(in float depth)
{
    // compute distance to the viewer
    float a = zFar / ( zFar - zNear );
    float b = zFar * zNear / ( zNear - zFar );    
    float dist = b / ( depth - a );

    return (dist - zNear) / (zFar - zNear);
}


//----------------------------------------------------------------------------------
vec3 uv_to_eye(vec2 uv, float eye_z)
{
    uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
    return vec3(uv * invFocalLength * eye_z, eye_z);
}

//----------------------------------------------------------------------------------
vec3 fetch_eye_pos(vec2 uv)
{
    float z = texture2D(texNormalDepthMap, uv).w;
    return uv_to_eye(uv, z);
}

/**
 * Horizon based ambient occlusion computation.
 **/
vec4 horizonBasedAO()
{
    // get eye position
    vec3 P = fetch_eye_pos(IN.texUV);
    
    // Project the radius of influence g_R from eye space to texture space.
    // The scaling by 0.5 is to go from [-1,1] to [0,1].
    float2 step_size = 0.5 * g_R  * g_FocalLen / P.z;

    // Early out if the projected radius is smaller than 1 pixel.
    float numSteps = min ( g_NumSteps, min(step_size.x * g_Resolution.x, step_size.y * g_Resolution.y));
    if( numSteps < 1.0 ) return 1.0;
    step_size = step_size / ( numSteps + 1 );

    // Nearest neighbor pixels on the tangent plane
    float3 Pr, Pl, Pt, Pb;
    float4 tangentPlane;
    if (useNormal) {
        float3 N = normalize(tNormal.SampleLevel(samNearest, float3(IN.texUV, 0), 0).xyz);
        tangentPlane = float4(N, dot(P, N));
        Pr = tangent_eye_pos(IN.texUV + float2(g_InvResolution.x, 0), tangentPlane);
        Pl = tangent_eye_pos(IN.texUV + float2(-g_InvResolution.x, 0), tangentPlane);
        Pt = tangent_eye_pos(IN.texUV + float2(0, g_InvResolution.y), tangentPlane);
        Pb = tangent_eye_pos(IN.texUV + float2(0, -g_InvResolution.y), tangentPlane);
    } else {
        Pr = fetch_eye_pos(IN.texUV + float2(g_InvResolution.x, 0));
        Pl = fetch_eye_pos(IN.texUV + float2(-g_InvResolution.x, 0));
        Pt = fetch_eye_pos(IN.texUV + float2(0, g_InvResolution.y));
        Pb = fetch_eye_pos(IN.texUV + float2(0, -g_InvResolution.y));
        float3 N = normalize(cross(Pr - Pl, Pt - Pb));
        tangentPlane = float4(N, dot(P, N));
    }
    
    // Screen-aligned basis for the tangent plane
    float3 dPdu = min_diff(P, Pr, Pl);
    float3 dPdv = min_diff(P, Pt, Pb) * (g_Resolution.y * g_InvResolution.x);

    // (cos(alpha),sin(alpha),jitter)
    float3 rand = tRandom.Load(int3((int)IN.pos.x&63, (int)IN.pos.y&63, 0)).xyz;

    float ao = 0;
    float d;
    float alpha = 2.0f * M_PI / g_NumDir;

    // this switch gets unrolled by the HLSL compiler
    switch (qualityMode)
    {
    case 0:
        for (d = 0; d < g_NumDir*0.5; ++d) {
            float angle = alpha * d;
            float2 dir = float2(cos(angle), sin(angle));
            float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
            ao += AccumulatedHorizonOcclusion_LowQuality(deltaUV, IN.texUV, P, numSteps, rand.z);
        }
        ao *= 2.0;
        break;
    case 1:
        for (d = 0; d < g_NumDir; d++) {
            float angle = alpha * d;
            float2 dir = float2(cos(angle), sin(angle));
            float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
            ao += AccumulatedHorizonOcclusion(deltaUV, IN.texUV, P, numSteps, rand.z, dPdu, dPdv);
        }
        break;
    case 2:
        for (d = 0; d < g_NumDir; d++) {
            float angle = alpha * d;
            float2 dir = float2(cos(angle), sin(angle));
            float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy;
            ao += AccumulatedHorizonOcclusion_Quality(deltaUV, IN.texUV, P, numSteps, rand.z, dPdu, dPdv);
        }
        break;
    }

    return 1.0 - ao / g_NumDir * g_Contrast;
}


/**
 **/
void main(void)
{
    // this is just a resolution of the input texture
    const vec2 fres = (osgppu_ViewportWidth, osgppu_ViewportHeight);

    // get pixel depth value
    float zLinear = texture2D(texNormalDepthMap, gl_TexCoord[0].xy).w;

    // compute eye position
    //vec3 ep = zLinear * gl_TexCoord[1].xyz;// / gl_TexCoord[1].z;    
    //vec3 bl = vec3(0,0,0);

    // for each sample we do
    /*for (int i=0; i < 1; i++)
    {
        // compute random vector pointing from eye to some point in the scene within certain radius
        vec3 se = ep + rad * fk3f[i].xyz;

        // project the point back into the screen to get the screen coordinates
        vec3 ss = se.xyz * vec3(.75,1.0,1.0);
        vec4 sz = texture2DProj( tex0, ss*.5+ss.z*vec3(.5) );

        //vec2 ss = (se.xy/se.z)*vec2(.75,1.0);
        //vec2 sn = ss*.5 + vec2(.5);
        //vec4 sz = texture2D(texDepthMap, sn);

        float zd = 50.0*max( se.z-sz.x, 0.0 );
        bl += se;//1.0/(1.0+zd*zd);              // occlusion = 1/( 1 + 2500*max{dist,0)^2 )
    }*/

    /*vec4 pl = texture2D( tex0, gl_Color.xy*.xy );
    pl = pl*2.0 - vec4(1.0);

    float bl = 0.0;
    for( int i=0; i<32; i++ )
    {
        vec3 se = ep + rad*reflect(fk3f[i].xyz,pl.xyz);

        vec2 ss = (se.xy/se.z)*vec2(.75,1.0);
        vec2 sn = ss*.5 + vec2(.5);
        vec4 sz = texture2D(tex1,sn);

        float zd = 50.0*max( se.z-sz.x, 0.0 );
        bl += 1.0/(1.0+zd*zd);
    }*/
    gl_FragColor.rgb = zLinear;//vec3(bl/32.0);
}
