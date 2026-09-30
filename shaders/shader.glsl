#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

layout (location = 0) in vec3 position;
// layout(location= 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
// layout(location= 4) in uint material;

out vec3 vertex_normal;
// flat out uint vertex_material;

void main() {
    gl_Position = mvpMatrix * vec4(position, 1);
    vertex_normal = vec3(mvMatrix * vec4(normal, 0));
    // vertex_material = material;
}
#endif

#ifdef FRAGMENT_SHADER
 in vec3 vertex_normal;
// flat in uint vertex_material;

vec3 light_dir = normalize(vec3(0, 1, 0));
vec3 color = vec3(1, 1, 1);

void main() {
    float cos_theta = dot(normalize(vertex_normal), light_dir);
    gl_FragColor = vec4(color * cos_theta, 1);
}
#endif