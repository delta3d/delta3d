uniform sampler2D diffuseTexture;

void main(void)
{
   vec3 diffuseColor = texture2D(diffuseTexture, gl_TexCoord[0].st).rgb;

   vec3 exposure = 4.5 * pow(diffuseColor, 10.5);
   
   exposure += (diffuseColor - exposure);

   //gl_FragColor = vec4(diffuseColor, 1.0);

   gl_FragColor = vec4(exposure,  1.0);
}

