#ifndef PROJET_H
#define PROJET_H


#include <iostream>
#include <string>
#include <vector>

#include "app.h"
#include "app_camera.h"
#include "color.h"
#include "draw.h"
#include "mat.h"
#include "materials.h"
#include "mesh.h"
#include "orbiter.h"
#include "program.h"
#include "texture.h"
#include "uniforms.h"
#include "wavefront.h"

class Projet : public App {
  public:
    Projet() : App(1024, 640) {}
    int init();
    int quit();
    int render();

  protected:
    Mesh m_scene;

    std::vector<Color> m_diffuse;

    GLuint m_program;
    GLuint m_vao;

    Orbiter m_camera;
	
	const int MAX_MATERIALS = 128;
};

#endif