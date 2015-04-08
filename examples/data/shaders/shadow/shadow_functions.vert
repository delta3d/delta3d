#version 120

uniform bool d3d_RenderShadows = false;


//must match texture unit offset property in shadow scene
//this cannot be a uniform since it indexes texture units
const int d3d_ShadowTextureUnit = 5;


void GenerateShadowTexCoords( in vec4 ecPosition ) 
{ 
   if(d3d_RenderShadows)
   {
      // generate coords for shadow mapping 
      gl_TexCoord[d3d_ShadowTextureUnit].s = dot( ecPosition, gl_EyePlaneS[d3d_ShadowTextureUnit] ); 
      gl_TexCoord[d3d_ShadowTextureUnit].t = dot( ecPosition, gl_EyePlaneT[d3d_ShadowTextureUnit] ); 
      gl_TexCoord[d3d_ShadowTextureUnit].p = dot( ecPosition, gl_EyePlaneR[d3d_ShadowTextureUnit] ); 
      gl_TexCoord[d3d_ShadowTextureUnit].q = dot( ecPosition, gl_EyePlaneQ[d3d_ShadowTextureUnit] ); 
   }
} 
