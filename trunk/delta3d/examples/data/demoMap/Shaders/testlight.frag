uniform sampler2D baseTexture;

varying vec3 vNormal;
varying vec3 vLightDirection;

varying vec3 vPos;
varying vec3 worldNormal;

varying float distance;

void main(void)
{  
   vec4 baseColor = texture2D(baseTexture, gl_TexCoord[0].st);
   vec3 normal = normalize(vNormal);
   vec3 lightDir = normalize(vLightDirection);

   float cosAngle = dot(normal, lightDir);

   vec3 diffuseColor = cosAngle * vec3(gl_LightSource[0].diffuse.rgb);
   vec3 lightContrib = vec3(gl_LightSource[0].ambient.rgb) + diffuseColor;  

   // compute final color with light and base color
   vec3 result = lightContrib * (vec3(baseColor) );     
 
    gl_FragColor = vec4(result, 1.0);

    // Uncomment these lines to render test values
    //gl_FragColor = vec4(cosAngle, cosAngle, cosAngle, 1.0);
    //gl_FragColor = vec4(normal, 1.0);   
    //gl_FragColor = vec4(lightDir, 1.0);
}
