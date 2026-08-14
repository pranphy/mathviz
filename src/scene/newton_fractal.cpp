#include <vector>
#include <print>
#include <glm/glm.hpp>

#include "scene/newton_fractal.h"
#include "mandelbrot_vert.h"
#include "newton_general_frag.h"


NewtonScene::NewtonScene(int w, int h):
    //Scene(w, h, mandelbrot_vert_spirv, standing_wave_frag_spirv)
    Scene(w, h, mandelbrot_vert_spirv, newton_general_frag_spirv),
    //Scene(w, h, mandelbrot_vert_spirv, mandelbrot_frag_spirv)
    coeffs{{-1.0f,0.0f}, {0.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
    roots{{1.0f,0.0f},{-0.5f,std::sqrt(3.0f)/2.0f},{-0.5f,-std::sqrt(3.0f)/2.0f}},
    click_tol{0.3}
{
    max_iterations = 30;
    name = "newton_fractal";

    window_aspect = static_cast<float>(width)/height;

    x_min_factor = -1.0f;
    x_max_factor =  1.0f;

    coeffs=std::vector<std::complex<float>>{{-1.0f,0.0f}, {0.0f,0.0f},{ 0.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
    roots=std::vector<std::complex<float>> {{ 1.0f,0.0f}, {0.0f,1.0f},{-1.0f,0.0f},{0.0,-1.0f}},

    update_roots(0,  1.0, 0.00);
    param = 0;
    //update_roots(1, -1.0, 0.3);
}

void NewtonScene::mouse_drag(float dx,float dy){
    auto mouse_coord = get_mouse_coord();
    auto root_pos = get_root_pos(mouse_coord);
    if(root_pos < roots.size()){
        //auto froot = roots[root_pos];
        //std::println("The root {}. is moved from {:.3f} + {:.3f}i  --to-->  {:.3f} + {:.3f}i ",root_pos,froot.real(),froot.imag(),mouse_coord.real(), mouse_coord.imag());
        update_roots(root_pos, mouse_coord.real(), mouse_coord.imag());
        //std::println("All roots ");
        //for(auto& root : roots){
        //    std::print(" {:.4f} + {:.4f}i, ",root.real(),root.imag());
        //}
        //std::println(" \nThe coeffs are ");
        //for(auto& coeff: coeffs){
        //    std::print(" {:.4f} + {:.4f}i, ",coeff.real(),coeff.imag());
        //}
        //std::println(" ");
    } else {
        float cx = static_cast<float>((dx / width)  * scale * 2.0);
        float cy = static_cast<float>((dy / height) * scale * 2.0);
        x -= cx;
        y += cy;
    }

}

unsigned NewtonScene::get_root_pos(std::complex<float> root){
     //std::println("Checking against {}+{}i",root.real(),root.imag());
     //std::println("The sacle = {} and (x,y) = ({},{}).",scale,x,y);
    for(unsigned i = 0; i < roots.size(); i++){
         //std::println("The root {}. is at {}+{}i",i,roots[i].real(),roots[i].imag());

        if(abs(roots[i] - root ) < click_tol ){
            return i;
        }
    }
    return roots.size();
}

void NewtonScene::update_coefs(){

}

void NewtonScene::update_roots(unsigned n, float re, float im) {
    if (n < roots.size()) roots[n] = std::complex<float>{re, im};

    // Start with P(z) = 1 (constant term c_0 = 1)
    std::vector<std::complex<float>> cffs = {1.0f};

    for (const auto& r : roots) {
        // Multiplying by (z - r) for ascending order: c_new = z*c_old - r*c_old
        std::vector<std::complex<float>> new_cffs(cffs.size() + 1, 0.0f);

        // Constant term is -r * old_c0
        new_cffs[0] = -r * cffs[0];

        // Middle terms
        for (size_t i = 1; i < cffs.size(); ++i) {
            new_cffs[i] = cffs[i - 1] - r * cffs[i];
        }

        // Highest degree term gets the old leading coefficient
        new_cffs.back() = cffs.back();

        cffs = new_cffs;
    }
    coeffs = cffs;
}

/*
void NewtonScene::update_roots(unsigned n,float re, float im){
    //std::println("Updating the root at {} with {}+{}i",n,re,im);
    if (n < roots.size()) roots[n] = std::complex<float>{re,im};
    // Start with P(z) = 1, represented by the single coefficient [1.0]
    std::vector<std::complex<float>> cffs = {1.0};

    for (const auto& r : roots) {
        // Multiplying by (z - r) increases the degree by 1
        std::vector<std::complex<float>> new_cffs(cffs.size() + 1, 0.0);

        new_cffs[0] = cffs[0]; // Leading term is always 1

        for (size_t i = 1; i < new_cffs.size(); ++i) {
            std::complex<float> curr = (i < cffs.size()) ? cffs[i] : 0.0;
            std::complex<float> prev = cffs[i - 1];
            new_cffs[i] = curr - r * prev;
        }

        cffs = new_cffs;
    }
    coeffs = cffs;
}
*/



void NewtonScene::set_param(NewtonParam p){
    std::tie(x,y,scale,max_iterations) = p;
}

void NewtonScene::setup_uniforms(){
    //std::println("We have roots size={} params={} roots",roots.size(),param);
    //std::println("The coeffs are ({}+{}i), ({}+{}i) , ({}+{}i), ({}+{}i)",coeffs[0].real(),coeffs[0].imag(),coeffs[1].real(),coeffs[1].imag(),coeffs[2].real(),coeffs[2].imag(),coeffs[3].real(),coeffs[3].imag());
    //std::println("The roots are ({}+{}i), ({}+{}i) , ({}+{}i)",roots[0].real(),roots[0].imag(),roots[1].real(),roots[1].imag(),roots[2].real(),roots[2].imag());
    glUniform1ui(8, roots.size());
    //std::println("The size of roots is {} ",roots.size());
    for(unsigned i = 0; i < roots.size(); i++){
        glUniform2f(13+i,roots[i].real(),roots[i].imag());
    }
    for(unsigned i = 0; i <= roots.size(); i++){
        glUniform2f(20+i,coeffs[i].real(),coeffs[i].imag());
    }
    glUniform1f(30, param);
}

bool NewtonScene::save_video(GLFWwindow* /*window*/,int /* unused */){
    return false;
}

std::complex<float> NewtonScene::get_mouse_coord(){

    if (width > 0 && height > 0) {
        float c_real = ( static_cast<float>(cursor_x)/width  * 2.0 - 1.0 )*scale + x;
        float c_imag = (-static_cast<float>(cursor_y)/height * 2.0 + 1.0 )*scale/window_aspect + y;
        return {c_real,c_imag};
    }
    return {0.0,0.0};
}

bool NewtonScene::run(GLFWwindow* window,int mode){
    if(mode == 0){ // GUI is 0 and TUI is 1
        render();
        return true;
    } else {
        return save_video(window,0);
    }

    return false;
}

NewtonScene::~NewtonScene(){}
