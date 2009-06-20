
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;

uniform int mode;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vViewDir;

float saturate(float inValue)
{
   return clamp(inValue, 0.0, 1.0);
}

mat3 MatrixInverse(mat3 inMatrix)
{  
   float det = dot(cross(inMatrix[0], inMatrix[1]), inMatrix[2]);
   mat3 T = transpose(inMatrix);
   return mat3(cross(T[1], T[2]),
               cross(T[2], T[0]),
               cross(T[0], T[1])) / det;
}

void main(void)
{
   // Normalize the passed vectors from the vertex shader
   vec3 viewDir = normalize(vViewDir);
   vec3 lightDir = normalize(vLightDir);
	
 // get edge vectors of the pixel triangle
   vec3 dp1  = dFdx(viewDir);
   vec3 dp2  = dFdy(viewDir);   
   vec2 duv1 = dFdx(gl_TexCoord[0].xy);
   vec2 duv2 = dFdy(gl_TexCoord[0].xy);  
   
   // solve the linear system
   mat3 M = mat3(dp1, dp2, cross(dp1, dp2));
   mat3 inverseM = MatrixInverse(M);
   vec3 T = inverseM * vec3(duv1.x, duv2.x, 0.0);
   vec3 B = inverseM * vec3(duv1.y, duv2.y, 0.0);
   
   // construct tangent frame  
   float maxLength = max(length(T), length(B));
   T = T / maxLength;
   B = B / maxLength;
   
   vec3 tangent = normalize(T);
   vec3 binormal = normalize(B);  
   
   mat3 tangentBasis = mat3(T, B, vNormal);
   
   // Get the colors from our base and normal textures
   vec3 base = texture2D(diffuseTexture, gl_TexCoord[0].xy).xyz;
   vec3 bump = texture2D(normalTexture, gl_TexCoord[0].xy).xyz;
   
   // Change the range from  [-1, 1] to [0, 1]
   bump = (bump * 2.0) - 1.0;   
   
   // Transform into world space
   vec3 normal = normalize(tangentBasis * bump);
   float originalNdotL = saturate(dot(vNormal, lightDir));
   
   float NdotL = saturate(dot(normal, lightDir));
   
   //  Get the phong contribution
   vec3 reflectionDir = normalize(2.0 * NdotL * vec3(normal) - lightDir);
   float reflectionAngle =  saturate(dot(reflectionDir, viewDir));
   
   // Limit the specular contribution
   float specularFactor = 0.25;
  
   vec3 ambient = vec3(0.2) * base;
   vec3 diffuse = vec3(0.8) * base * NdotL;
   vec3 specular = originalNdotL * vec3(pow(reflectionAngle, 16.0)) * specularFactor;
   
   vec3 finalColor;
   
   if (mode == 1)
   {
	  finalColor = base;
   }
   else if (mode == 2)
   {
	  finalColor = bump;
   }
   else if (mode == 3)
   {
	  finalColor = ambient;
   }
   else if (mode == 4)
   {
      finalColor = diffuse;
   }
   else if (mode == 5)
   { 
	  finalColor = specular; 	
   }
   else if (mode == 6)
   {
      finalColor = vec3(NdotL); 
   }
   else if (mode == 7)
   {
	  finalColor = normal;
   }
   else if (mode == 8)
   {
      finalColor = vNormal.xyz;
   }
   else if (mode == 9)
   {   
	  finalColor = vLightDir;	 
   }   
   else if (mode == 0)
   {
      finalColor = ambient + diffuse + specular;
   }
   else
   {
      finalColor = vec4(1.0, 0.0, 0.0, 1.0);
   }
   
   gl_FragColor = vec4(finalColor, 1.0);
}


