
uniform sampler2D baseTexture;
uniform int SHADOW_TEX_COORD_UNIT;

float SampleShadowTexture()
{
   return osgShadow_ambientBias.x + shadow2DProj( osgShadow_shadowTexture, gl_TexCoord[SHADOW_TEX_COORD_UNIT] ).r * osgShadow_ambientBias.y;
}
