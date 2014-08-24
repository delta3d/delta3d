
float calculateDistance(mat4 modelViewMatrix, vec4 vertex)
{
   vec4 ecPosition = modelViewMatrix * vertex;
   vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
   float dist = length(ecPosition3);
   return dist;
}

void normalizeLight(mat4 InverseMVM, vec4 position, out vec3 lightNormal)
{
   lightNormal = normalize(vec3(InverseMVM * position));
}

//returns -1.0 if x < 0, and 1.0 if x >= 0
float signGreaterEqualZero(float x)
{
    return step(0.0, x) * 2.0 - 1.0;
}

void sphereMap(in vec3 eye, in vec3 normal, out vec2 ReflectTexCoord)
{
   float m;
   vec3 r,u;

   u = normalize(eye);
   r = reflect(u,normal);
   m = 2.0 * sqrt(dot(r.xy,r.xy) + ((r.z + 1.0) * (r.z + 1.0)));

   ReflectTexCoord = vec2(r.x/m + 0.5,r.y/m + 0.5);
}
