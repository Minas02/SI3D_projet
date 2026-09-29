#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
// uniform mat4 mvMatrix;

layout(location= 0) in vec3 position;
// layout(location= 1) in vec2 texcoord;
// layout(location= 2) in vec3 normal;
// layout(location= 4) in uint material;

// out vec3 vertex_normal;
// flat out uint vertex_material;

void main() {
    gl_Position = mvpMatrix * vec4(position, 1);
    // vertex_normal = vec3(mvMatrix * vec4(normal, 0));
    // vertex_material = material;
}
#endif

#ifdef FRAGMENT_SHADER
// in vec3 vertex_normal;
// flat in uint vertex_material;

// vec3 light_dir = normalize(vec3(0, 0, 1));

// int steps = 4;
// vec3 colors[4] = vec3[](
// vec3(0.5,0.5,0.5),
// vec3(0.2,0.2,0.2),
// vec3(0.6,0.3,0.8),
// vec3(0.0,0.0,0.0));

void main() {

    // float cos_theta = dot(normalize(vertex_normal), light_dir);
    // vec3 color = vec3(1, 1, 1);
    //toon shading
    //gl_FragColor = floor(vec4(color * cos_theta, 1)* steps)/steps;
    // gl_FragColor = vec4(colors[vertex_material] * cos_theta, 1);
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
#endif