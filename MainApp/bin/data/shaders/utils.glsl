#define PI 3.14159265359
#define TWO_PI 6.28318530717958647693

float map( float value, float inMin, float inMax, float outMin, float outMax )
{
    return ( (value - inMin ) / ( inMax - inMin ) * ( outMax - outMin ) ) + outMin; 
}

float mapClamp( float value, float inMin, float inMax, float outMin, float outMax )
{
    float outVal = ( (value - inMin ) / ( inMax - inMin ) * ( outMax - outMin ) ) + outMin;
    if(outMax < outMin){
        if( outVal < outMax ) outVal = outMax;
        else if( outVal > outMin )outVal = outMin;
    }else{
        if( outVal > outMax ) outVal = outMax;
        else if( outVal < outMin ) outVal = outMin;
    }
    return outVal;
}

float limit( float p, float limit ) {
    if( p > limit ) {
        p = limit;
    }
    else if( p < -limit ) {
        p = -limit;
    }
    return p;
}

vec3 limit3( in vec3 p, float limit ) {
    p.x = clamp( p.x, -limit, limit );
    p.y = clamp( p.y, -limit, limit );
    p.z = clamp( p.z, -limit, limit );
    return p; 
}

float distance_squared2(in vec2 a, in vec2 b ) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy;
}

float distance_squared3(in vec3 a, in vec3 b ) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx*dx + dy*dy + dz*dz;
}

float length_squared2( in vec2 a ) {
    return (a.x*a.x + a.y*a.y);
}

float length_squared3( in vec3 a ) {
    return (a.x*a.x + a.y*a.y + a.z*a.z);
}

float atan2( in vec2 a ) {
    bool s = (abs(a.x) > abs(a.y));
    return mix( 3.14159265359/2.0 - atan(a.x,a.y), atan(a.y,a.x), s);
}

vec3 normalizeVec3( in vec3 av ) {
    float clen = length(av);
    if( clen != 0.0 ) {
        av = av/clen;
    } else {
//        av = vec3(1.0,0.0,0.0);
    }
    return av;
}

vec2 normalizeVec2( in vec2 av ) {
    float clen = length(av);
    if( clen != 0.0 ) {
        av = av/clen;
    } else {
        //        av = vec3(1.0,0.0,0.0);
    }
    return av;
}

mat4 lookAt (vec3 eye, vec3 aim, vec3 up)
{
    vec3 f = normalize (aim - eye);
    vec3 s = normalize (cross (f, up));
    vec3 u = cross (s, f);
    mat4 view = mat4 (vec4 (s.x, u.x, -f.x, 0.0),
                      vec4 (s.y, u.y, -f.y, 0.0),
                      vec4 (s.z, u.z, -f.z, 0.0),
                      vec4 (0.0, 0.0, 0.0, 1.0));
    return view;
}
float rand(float n){return fract(sin(n) * 43758.5453123);}

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
