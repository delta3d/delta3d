//////////////////////////////////////////////
//A generic bump mapping shader
//by Bradley Anderegg
//////////////////////////////////////////////


uniform vec4 lightPos;
uniform vec4 eyePosition;



varying vec2 v_texCoord;
varying vec4 v_lvts;
varying vec4 v_halfVector;

void main(void)
{

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    vec4 v0 = gl_Vertex;
    vec4 v1 = gl_MultiTexCoord0;
    vec4 v2 = gl_MultiTexCoord1;
    
    vec4 normal = vec4(gl_Normal.xyz, 0.0);
    
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
    	vec4 denom =  (-s2.x * s1.y) + (s1.x * s2.y);
    	
    	/*
    		//compute the tangent
    	*/
    	vec4 tangent = s1.y * -q1;
    	vec4 temp = s2.y * q0;
    	tangent += temp;
    	tangent /= denom;
    	tangent.w = 0.0;
    	tangent = normalize(tangent);
    	
    	/*
    		//gram-schmidt tangent
    		//tan - (normal * (tan dot normal))
    	*/
    	float tandotn = dot(tangent, normal);
    	tandotn *= normal;
    	tangent -= tandotn;
    	
    	/*
    		  //compute the binormal
    	*/
    	vec4 binormal = s1.x * q1;
    	temp = s2.x * q0;
    	binormal -= temp;
    	binormal /= denom;
    	binormal.w = 0.0;
    	binormal = normalize(binormal);
    	
    	/*
    		gram-schmidt binormal
    		bi - (normal * (bi dot normal) - (tan * (bi dot tan)))
    	*/
    	float bidotn = dot(binormal, normal);
    	bidotn *= normal;
    	float bidottan = dot(binormal, tangent);
    	bidottan *= tangent;
    	bidotn -= bidottan;
    	binormal -= bidotn;
    	
    	/*
    		multiply by tangent basis
    	*/
    	
    	//vec4 lvts = mul(lv, tangent_basis);
    	vec4 lvts;
    	lvts.x = dot(lv, tangent);
    	lvts.y = dot(lv, binormal);
    	lvts.z = dot(lv, normal);
    	lvts.w = 0.0;
    	
    	/*
    	  now compute the specular component
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




