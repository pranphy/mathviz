#include "scene/scene.h"

void Scene::step_right(){
    float step = 0.01f * scale;
    x += step;
}

void Scene::step_left(){
    float step = 0.01f * scale;
    x -= step;
}

void Scene::step_up(){
    float step = 0.01f * scale;
    y += step;
}

void Scene::step_down(){
    float step = 0.01f * scale;
    y -= step;
}

void Scene::zoom_in(){
    float step = 0.01f * scale;
    scale -= step;
}

void Scene::zoom_out(){
    float step = 0.01f * scale;
    scale += step;
}

void Scene::iterate_up(){
    max_iterations++;
}

void Scene::iterate_down(){
    if (max_iterations > 0) max_iterations--;
}

Scene::~Scene(){}
