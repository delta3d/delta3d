#version 120

uniform sampler2DShadow shadowTexture;
uniform vec2 ambientBias;

uniform bool d3d_RenderShadows = false;
uniform int d3d_ShadowTextureUnit = 1;
uniform float d3d_ShadowEffectScalar = 1.0;

float SampleShadowTexture()
{
   if(d3d_RenderShadows)
   {
      return mix(1.0, (ambientBias.x + shadow2DProj( shadowTexture, gl_TexCoord[d3d_ShadowTextureUnit] ).r * ambientBias.y), d3d_ShadowEffectScalar);
   }
   else
   {
      return 1.0;
   }
}
