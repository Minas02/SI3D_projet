
#version 430

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

in vec3 position;
out vec3 vertex_position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= position;
}
#endif


#ifdef FRAGMENT_SHADER

struct fragment
{
    float x, y, z;
    float r, g, b;
};

layout(std430, binding= 0) buffer fragments
{
    uint count;
    uint instance_count;
    uint vertex_base;
    uint instance_base;
    
    fragment array[];
};

in vec3 vertex_position;
out vec4 fragment_color;

//~ layout(early_fragment_tests) in;

void main( )
{
    vec3 t= normalize( dFdx(vertex_position) );
    vec3 b= normalize( dFdy(vertex_position) );
    vec3 normal= normalize( cross(t, b) );
    vec3 color= (normal + 1) / 2;
    fragment_color= vec4(color, 1);
    
    uint offset= atomicAdd(count, 1);
    
    array[offset].x= vertex_position.x;
    array[offset].y= vertex_position.y;
    array[offset].z= vertex_position.z;
    array[offset].r= color.r;
    array[offset].g= color.g;
    array[offset].b= color.b;
}
#endif
