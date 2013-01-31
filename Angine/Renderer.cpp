#include "Renderer.h"


Renderer::Renderer(HWND hwnd)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
        1,                     // version number  
        PFD_DRAW_TO_WINDOW |   // support window  
        PFD_SUPPORT_OPENGL |   // support OpenGL  
        PFD_DOUBLEBUFFER,      // double buffered  
        PFD_TYPE_RGBA,         // RGBA type  
        32,                    // 32-bit color depth  
        0, 0, 0, 0, 0, 0,      // color bits ignored  
        0,                     // no alpha buffer  
        0,                     // shift bit ignored  
        0,                     // no accumulation buffer  
        0, 0, 0, 0,            // accum bits ignored  
        24,                    // 24-bit z-buffer  
        0,                     // no stencil buffer  
        0,                     // no auxiliary buffer  
        PFD_MAIN_PLANE,        // main layer  
        0,                     // reserved  
        0, 0, 0                // layer masks ignored  
    };

    this->hdc = GetDC(hwnd);

 
    // get the best available match of pixel format for the device context   
    this->iPixelFormat = ChoosePixelFormat(this->hdc, &pfd); 
 
    // make that the pixel format of the device context  
    SetPixelFormat(this->hdc, this->iPixelFormat, &pfd);
  
    // call OpenGL APIs as desired ... 

}


bool Renderer::start(void)
{
    // create a rendering context  
    this->hglrc = wglCreateContext(this->hdc); 
 
    // make it the calling thread's current rendering context 
    wglMakeCurrent(this->hdc, this->hglrc);

    glClearColor(.2f, .5f, .0f, .0f);                   // Black Background

    return true;
}


void Renderer::update(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SwapBuffers(this->hdc);
}


void Renderer::stop(void)
{
    // when the rendering context is no longer needed ...   
    // make the rendering context not current  
    wglMakeCurrent(NULL, NULL);
 
    // delete the rendering context  
    wglDeleteContext(this->hglrc);
}


Renderer::~Renderer(void)
{
}
