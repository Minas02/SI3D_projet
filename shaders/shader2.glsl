
#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;
layout(location= 4) in uint material;

out vec3 vertex_normal;
out vec2 vertex_textcoord;
flat out uint vertex_material;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);

    vertex_normal = vec3(mvMatrix * vec4(normal, 0));
    vertex_textcoord = texcoord;
    vertex_material = material;
}
#endif

#ifdef FRAGMENT_SHADER
uniform sampler2D colorTexture;

in vec3 vertex_normal;
in vec2 vertex_textcoord;

flat in uint vertex_material;

vec3 lightDir = normalize(vec3(0,0,1));
vec3 colorPalette[4] = vec3[](
vec3(1.0,0.5,0.8),
vec3(0.5,0.5,0.5),
vec3(1.0,1.0,0.0),
vec3(0.2,0.2,1.0)
);

void main( )
{
    float cos_theta = dot(normalize(vertex_normal), lightDir);

    if(vertex_textcoord.x > 0)
    colorPalette[0] = texture(colorTexture, vertex_textcoord).rgb;

    int steps = 3;

    gl_FragColor= floor(vec4(colorPalette[vertex_material] * cos_theta, 1) * steps) / steps;
}
#endif