#version 330

in vec3 vertex_normal;
flat in uint vertex_material;

vec3 light_dir = normalize(vec3(0, 0, 1));

//toon shading
//int steps = 4;
vec3 colors[4] = vec3[](
vec3(0.5,0.5,0.5),
vec3(0.2,0.2,0.2),
vec3(0.6,0.3,0.8),
vec3(0.0,0.0,0.0));

void main() {

    float cos_theta = dot(normalize(vertex_normal), light_dir);
    vec3 color = vec3(1, 1, 1);
    //toon shading
    //gl_FragColor = floor(vec4(color * cos_theta, 1)* steps)/steps;
    gl_FragColor = vec4(colors[vertex_material] * cos_theta, 1);
}