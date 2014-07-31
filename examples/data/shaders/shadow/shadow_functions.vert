
uniform int SHADOW_TEX_COORD_UNIT;


void GenerateShadowTexCoords( in vec4 ecPosition ) 
{ 
   // generate coords for shadow mapping 
   gl_TexCoord[SHADOW_TEX_COORD_UNIT].s = dot( ecPosition, gl_EyePlaneS[SHADOW_TEX_COORD_UNIT] ); 
   gl_TexCoord[SHADOW_TEX_COORD_UNIT].t = dot( ecPosition, gl_EyePlaneT[SHADOW_TEX_COORD_UNIT] ); 
   gl_TexCoord[SHADOW_TEX_COORD_UNIT].p = dot( ecPosition, gl_EyePlaneR[SHADOW_TEX_COORD_UNIT] ); 
   gl_TexCoord[SHADOW_TEX_COORD_UNIT].q = dot( ecPosition, gl_EyePlaneQ[SHADOW_TEX_COORD_UNIT] ); 
} 
