
uniform sampler2D baseTexture;
uniform sampler2DShadow shadowTexture;
uniform vec2 ambientBias;

const int SHADOW_TEX_COORD_UNIT = 6;

float SampleShadowTexture()
{
   return ambientBias.x + shadow2DProj( shadowTexture, gl_TexCoord[SHADOW_TEX_COORD_UNIT] ).r * ambientBias.y;
}
