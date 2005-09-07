//////////////////////////////////////////////
//A generic bump mapping shader
//by Bradley Anderegg
//////////////////////////////////////////////

uniform sampler2D texMap;
uniform sampler2D normalMap;


varying vec2 v_texCoord;
varying vec4 v_lvts;
varying vec4 v_halfVector;


void main (void)
{
	vec2 modTex = vec2(v_texCoord.x, -1.0 * v_texCoord.y);
    vec4 texColor = texture2D(texMap, modTex);
    vec4 bumpColor = texture2D(normalMap, modTex);

    //shift from 0 - 1 to -1 - 1	
    bumpColor *= 2.0;
    bumpColor -= 1.0;
    
    //calculate attenuation
    float atten = 1.0 / pow(length(v_lvts), 0.30);
    vec4 lvts = normalize(v_lvts);
    float c = dot(bumpColor, lvts);

    vec4 spec = normalize(v_halfVector);
    spec = dot(spec, bumpColor);
    spec =  3 * max((pow(spec, 2.0) - 0.75), vec4(0.0, 0.0, 0.0, 0.0));
	
	
    //calculate self shadowing term
    //vec4 selfShadow = 3.0 * (dot(bumpColor, lvts));
    //atten *= selfShadow;
    spec *= atten;	 

    //calculate and return the final texture color
    spec *= texColor;

	vec3 color = (c * texColor) + spec;
    gl_FragColor = vec4(color.xyz, 1.0);
     

}
