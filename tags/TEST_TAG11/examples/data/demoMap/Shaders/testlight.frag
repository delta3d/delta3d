// Note - This shader is not optimized.  It is layed out for clarity.

uniform sampler2D baseTexture;

varying vec3 vNormal;
varying vec3 vViewDir;

varying vec3 vPos;
varying float distance;

void main(void)
{  
   vec4 baseColor = texture2D(baseTexture, gl_TexCoord[0].st);

   // normalize all of our incoming vectors
   vec3 normal = normalize(vNormal);
   vec3 lightDir = normalize(gl_LightSource[0].position);
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * dot(normal, lightDir) * normal - lightDir);

   float cosAngle = dot(normal, lightDir);
   float reflectionAngle =  dot(reflectionDir, viewDir);

   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(gl_LightSource[0].ambient.rgb);
   vec3 diffuseColor = cosAngle * vec3(gl_LightSource[0].diffuse.rgb);
   vec3 specularColor = vec3(gl_LightSource[0].specular.rgb) * pow(max(0.0, reflectionAngle), 8.0);

   vec3 lightContrib = ambientColor + diffuseColor + specularColor;

   // compute final color with light and base color
   vec3 result = lightContrib * (vec3(baseColor) );     
 
    gl_FragColor = vec4(result, 1.0);

    // Uncomment these lines to render test values
    //gl_FragColor = vec4(cosAngle, cosAngle, cosAngle, 1.0);
    //gl_FragColor = vec4(normal, 1.0);   
    //gl_FragColor = vec4(lightDir, 1.0);

   // The default value for the specular exponent appears to be 0...
   //float specularExp = gl_LightSource[0].spotExponent;
   //gl_FragColor = vec4(specularExp, specularExp, specularExp, 1.0);
}
