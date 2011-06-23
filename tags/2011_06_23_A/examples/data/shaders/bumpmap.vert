//////////////////////////////////////////////
//A generic bump mapping shader
//by Bradley Anderegg
//////////////////////////////////////////////

/**
* our uniforms are the light position
* and the eye position 
*/
uniform vec4 lightPos;
uniform vec4 eyePosition;

/**
* we carry over the tex coordinate 
* the light vector in tangent space for the lighting
* and the half vector from the reflecting angle from the viewer 
* to the light pos off of the surface, this is for calculating 
* the specular component
*/
varying vec2 v_texCoord;
varying vec4 v_lvts;
varying vec4 v_halfVector;



void main(void)
{
   //transform our vector into screen space
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
   /**
    * Note: v0 = the vertex
    *		   v1 = first set of tex coords = the adjacent vertex
    *		   v2 = second set of tex coords = the opposite vertex
    */
   vec4 v0 = gl_Vertex;
   vec4 v1 = gl_MultiTexCoord0;
   vec4 v2 = gl_MultiTexCoord1;
    
   vec4 normal = vec4(gl_Normal.xyz, 0.0);
    
   /**
    * Note: tc = third set of tex coords = this verts tex coords
    *		   tc1 = fourth set of tex coords = the adjacent verts tex coords
    *		   tc2 = fifth set of tex coords = the opposite verts tex coords
    */
   vec4 tc = gl_MultiTexCoord2;
   vec4 tc1 = gl_MultiTexCoord3;
   vec4 tc2 = gl_MultiTexCoord4; 
    				   
   //setup our variables
   vec4 s1 = tc1 - tc;
   vec4 s2 = tc2 - tc;	
   vec4 q0 = v1 - v0;
   vec4 q1 = v2 - v0;
   vec4 lv = lightPos - v0;
   lv.w = 0.0;
   
   //XXX: vec4 denom =  (-s2.x * s1.y) + (s1.x * s2.y);
   vec4 denom = vec4((-s2.x * s1.y) + (s1.x * s2.y));
    
   /**
    *	compute the tangent
    */
   vec4 tangent = s1.y * -q1;
   vec4 temp = s2.y * q0;
   tangent += temp;
   tangent /= denom;
   tangent.w = 0.0;
   tangent = normalize(tangent);
    
   /**
    *	gram-schmidt tangent
    *	tan - (normal * (tan dot normal))
    */
   float tandotn = dot(tangent, normal);
   vec4 tempNormal = vec4(normal * tandotn);
   tangent -= tempNormal;
    
   /**
    * compute the binormal
    */
   vec4 binormal = s1.x * q1;
   temp = s2.x * q0;
   binormal -= temp;
   binormal /= denom;
   binormal.w = 0.0;
   binormal = normalize(binormal);
    
   /**
    * gram-schmidt binormal
    *	bi - (normal * (bi dot normal) - (tan * (bi dot tan)))
    */
   float bidotn = dot(binormal, normal);
   tempNormal = vec4(normal * bidotn);
   float bidottan = dot(binormal, tangent);
   vec4 tempTan = vec4(tangent * bidottan); 
   vec4 tempBi = vec4(tempNormal - tempTan);
   binormal -= tempBi;
    
   /**
    *	multiply light vector by tangent basis
    */
   vec4 lvts;
   lvts.x = dot(lv, tangent);
   lvts.y = dot(lv, binormal);
   lvts.z = dot(lv, normal);
   lvts.w = 0.0;
    
   /**
    *	now compute the specular component
    *	from the half vector in tangent space
    */
   vec4 halfVector = lv;
   halfVector += (eyePosition - v0);
   halfVector.w = 0.0;
   halfVector = normalize(halfVector);
   vec4 spec; 
   spec.x = dot(halfVector, tangent);
   spec.y = dot(halfVector, binormal);
   spec.z = dot(halfVector, normal);
   spec.w = 0.0;	
   normal.w = 0.0;
    	
   //set output vars
   v_texCoord = tc.xy;
   v_lvts = lvts;
   v_halfVector = spec;
   
}




