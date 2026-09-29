#include "projet.h"

int Projet::init() {
	m_program = read_program("shaders/shader.glsl");
	program_print_errors(m_program);

	m_scene = read_mesh("data/bistro/exterior.obj");
	
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
	program_uniform(m_program, "mvpMatrix", mvp);

	draw(m_scene, model, view, projection);

	return 1;
}