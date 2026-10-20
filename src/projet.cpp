#include "projet.h"

int Projet::init() {
    m_program = read_program("shaders/shader.glsl");
    program_print_errors(m_program);

    m_scene = read_mesh("data/bistro/exterior.obj");

    const Materials &materials = m_scene.materials();

    std::vector<bool> mark(materials.filename_count(), false);

    m_diffuse.resize(256);

    for (int i = 0; i < materials.count(); i++) {
        const Material &material = materials(i);
        if (i < 256) {
            m_diffuse[i] = material.diffuse;
        }

        if (material.diffuse_texture != -1) {
            mark[material.diffuse_texture] = true;
        }
    }

    std::vector<ImageData> diffuse_images(mark.size());

#pragma omp parallel for schedule(dynamic, 1)
    for (size_t i = 0; i < mark.size(); i++) {
        if (mark[i]) {
            diffuse_images[i] = read_image_data(materials.filename(i));
        }
    }

    std::vector<GLuint> textures(mark.size());
    for (size_t i = 0; i < mark.size(); i++) {
        if (mark[i]) {
            textures[i] = make_texture(0, diffuse_images[i], GL_SRGB8_ALPHA8);
        }
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    return 0;
}

int Projet::quit() {
    m_scene.release();

    return 0;
}

int Projet::render() {
    update_camera(m_camera);

    Transform model = Identity();
    Transform view = m_camera.view();
    Transform projection = m_camera.projection(window_width(), window_height(), 45);

    glViewport(0, 0, window_width(), window_height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_program);

    // if (key_state('r')) {
    // 	reload_program(m_program);
    // 	program_print_errors(m_program);
    // }

    Transform mvp = projection * view * model;
    Transform mv = view * model;

    program_uniform(m_program, "mvpMatrix", mvp);
    program_uniform(m_program, "mvMatrix", mv);
    // program_uniform(m_program, "materialsCount", m_scene.materials().count());
    program_uniform(m_program, "diffuseColor", m_diffuse);

    draw(m_scene, model, view, projection);

    return 1;
}