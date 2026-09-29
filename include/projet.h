#ifndef PROJET_H
#define PROJET_H

#include "wavefront.h"
#include "program.h"
#include "uniforms.h"
#include "texture.h"

#include "draw.h"        
#include "app.h"        
#include "orbiter.h"
#include "app_camera.h"
#include "mat.h"

#include <vector>
#include <string>
#include <iostream>

class Projet : public App {
public:
    Projet() : App(1024, 640) {}
    int init();
	int quit();
	int render();

protected:
	Mesh m_scene;

	GLuint m_program;
	GLuint m_vao;

	Orbiter m_camera;
};

#endif