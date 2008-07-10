//////////////////////////////////////////////
//A generic bump mapping shader
//by Bradley Anderegg
//////////////////////////////////////////////

//let tex unit 0 be the texture map and tex unit 1 be the normal map
uniform sampler2D texMap;
uniform sampler2D normalMap;

/**
*we carry over the tex coordinate 
*the light vector in tangent space for the lighting
*and the half vector from the reflecting angle from the viewer 
*to the light pos off of the surface, this is for calculating 
*the specular component
*/
varying vec2 v_texCoord;
varying vec4 v_lvts;
varying vec4 v_halfVector;



void main (void)
{
    
   //the texColor is the color sampled from the texture
   vec4 texColor = texture2D(texMap, v_texCoord);

   //the bumpColor is our normal from the normal map texture
   vec4 bumpColor = texture2D(normalMap, v_texCoord);

   /**
   * the values in color range from 0-1 but normals range from -1 to 1
   * shift values from 0 - 1 to -1 - 1	
   */
   bumpColor *= 2.0;
   bumpColor -= 1.0;
    
   /**
    * take the dot product of the normal and our light vector 
    * in tangent space
    * NOTE: not normalizing v_lvts in the vertex shader helps to retain accuracy during hardware interpolation
    */
   vec4 lvts = normalize(v_lvts);
   float c = dot(bumpColor, lvts);

   //calculate specular contribution 
   float spec = 0.0;
   if(c > 0.0) 
   {
   
	   vec4 specComp = normalize(v_halfVector);

	   float spec = max( dot(bumpColor, specComp), 0.0);
	   spec = 1.5 * pow(spec, 20.0);

   }
   
   //get the final color value
   vec4 color = vec4((c + spec) * texColor);
   gl_FragColor = vec4(color.xyz, 1.0);
     

}
