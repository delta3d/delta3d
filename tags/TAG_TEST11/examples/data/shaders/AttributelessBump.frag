uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specTexture;

uniform mat4 RotationMatrix; //Rotation matrix form model

uniform int mode;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vViewDir;
varying vec3 position;	//object space pos

//From Shader X5
mat3 compute_tangent_frame_O3(vec3 N, vec3 p, vec2 uv)
{
	// Optimisation 3:
	// assume M is orthogonal

	// get edge vectors of the pixel triangle
	vec3 dp1 = normalize(dFdx(p));
	vec3 dp2 = normalize(dFdy(p));
	vec2 duv1 = normalize(dFdx(uv));
	vec2 duv2 = normalize(dFdy(uv));

	// solve the linear system
	// (not much solving is left going here)
	mat3 M = mat3(dp1, dp2,cross(dp1, dp2));
	vec3 T = mul(vec3(duv1.x, duv2.x,0), M);
	vec3 B = mul(vec3(duv1.y, duv2.y,0), M);

	// construct tangent frame 
	return mat3(normalize(T), normalize(B), normalize(N));
}

void main(void)
{  
   vec4 baseColor = texture2D(diffuseTexture, gl_TexCoord[0].st);
   vec4 bumpColor = texture2D(normalTexture, gl_TexCoord[0].st);
   vec4 specColor = texture2D(specTexture, gl_TexCoord[0].st);
   
   // Change the range from  [-1, 1] to [0, 1]
   vec3 tangentSpaceNormal = (bumpColor.rgb * 2.0) - vec3(1.0, 1.0, 1.0);
   
   vec3 n = normalize(vNormal);
   mat3 TBN = compute_tangent_frame_O3(n, position,gl_TexCoord[0].xy);
   
   // Transform the tangent space normal into view space
   vec3 WorldMapNormal;
   WorldMapNormal.xyz = normalize(mul(TBN, tangentSpaceNormal));   
 
   //Here's a tricky part. Up till now, we have not taken into account the rotation of the model,
   //we've assumed that world space an object space are the same. They are not if the model is
   //rotated, so take the tangent frame normal and mult by the rotation matrix
   WorldMapNormal = gl_ModelViewMatrixInverse * vec4(WorldMapNormal, 0);
   WorldMapNormal.xyz = RotationMatrix * vec4(WorldMapNormal, 0);
   
   // normalize all of our incoming vectors
   vec3 lightDir = normalize(vLightDir);
   vec3 viewDir = normalize(vViewDir);
   vec3 reflectionDir = normalize(2.0 * dot(WorldMapNormal, lightDir) * WorldMapNormal - lightDir); 
   
   float NdotL = dot(WorldMapNormal.xyz, lightDir);
   NdotL = clamp(NdotL, 0.0, 1.0);
   float reflectionAngle =  dot(reflectionDir, viewDir);
   
   // Calculate the contributions from each shading component
   vec3 ambientColor = vec3(0.2, 0.2, 0.2) * baseColor.rgb;
   vec3 diffuseColor = NdotL * vec3(gl_LightSource[0].diffuse.rgb) * baseColor.rgb;
   vec3 specularColor = specColor.rgb * pow(max(0.0, reflectionAngle), 128.0);

   vec3 result = ambientColor + diffuseColor + specularColor;
   
   if (mode == 0)
   {
      gl_FragColor = vec4(result, 1.0);
   }
   else if (mode == 1)
   {
      gl_FragColor = baseColor;
   }
   else if (mode == 2)
   {
      gl_FragColor = bumpColor;
   }
   else if (mode == 3)
   {
      gl_FragColor = specColor;
   }
   else if (mode == 4)
   {
      gl_FragColor = vec4(NdotL, NdotL, NdotL, 1.0);
   }
   else if (mode == 5)
   {
      gl_FragColor = vec4(ambientColor, 1.0);
   }
   else if (mode == 6)
   {
      gl_FragColor = vec4(diffuseColor, 1.0);
   }
   else if (mode == 7)
   {
      gl_FragColor = vec4(specularColor, 1.0);
   }
   else if (mode == 8)
   {
      gl_FragColor = vec4(lightDir, 1.0);
   }
   else if (mode == 9)
   {
      gl_FragColor = vec4(reflectionDir, 1.0);
   } 
}


