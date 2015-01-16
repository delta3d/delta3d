#version 120

uniform sampler2DShadow shadowTexture;
uniform vec2 ambientBias;

uniform bool d3d_RenderShadows = false;
uniform float d3d_ShadowEffectScalar = 1.0;

//must match texture unit offset property in shadow scene
//this cannot be a uniform since it indexes texture units
const int d3d_ShadowTextureUnit = 5;


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
