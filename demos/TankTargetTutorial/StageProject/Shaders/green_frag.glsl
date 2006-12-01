uniform sampler2D diffuseTexture;
uniform sampler2D secondaryTexture;
uniform float TimeDilation;

varying vec3 vNormal;
varying vec3 vHalfVec;
varying vec3 vLightDir;

float alpha = 0.2;
vec4 blendColor = vec4(0.0,1.0,0.0,1.0); // Green

void main()
{
   vec4 diffuseColor = texture2D(diffuseTexture,gl_TexCoord[0].st);
   //float NdotHV = max(dot(vNormal, vHalfVec),0.0);   
   //float specular = pow( NdotHV, 10.0 );
   //float normalDotLight = max(dot(vNormal, vLightDir),0.0);
   //diffuseColor *= normalDotLight;
   //diffuseColor = vec4(diffuseColor.xyz + (diffuseColor.xyz * specular),1.0);
   //gl_FragColor = (alpha * diffuseColor) + (1.0-alpha)*blendColor;

   float normedTime = abs(TimeDilation - 0.5);

   vec2 lookup1 = vec2(gl_TexCoord[0].x + TimeDilation, gl_TexCoord[0].y+TimeDilation+.25);
   vec2 lookup2 = vec2(gl_TexCoord[0].x - sqrt(normedTime+float(1)), gl_TexCoord[0].y + sqrt(normedTime+float(1)));
   vec2 lookup3 = vec2(gl_TexCoord[0].x - (TimeDilation*float(2)), gl_TexCoord[0].y + TimeDilation);
   vec4 base = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec4 offset1 = texture2D(secondaryTexture, lookup1);
   vec4 offset2 = texture2D(secondaryTexture, lookup2);
   vec4 offset3 = texture2D(secondaryTexture, lookup3);
   vec4 color;

   color.a = 1.0;
   color.r = offset1.r*0.6 + offset2.r*0.3 + offset3.r*0.3;
   color.g = offset1.g*0.2 + offset2.g*0.7 + offset3.g*0.2;
   color.b = offset1.b*0.2 + offset2.b*0.2 + offset3.b*0.8;
   gl_FragColor = (alpha * diffuseColor) + (1.0-alpha)*color;

 
}
