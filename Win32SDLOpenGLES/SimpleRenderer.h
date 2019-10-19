#pragma once

#include <string.h>

#ifdef __APPLE__
#include <unistd.h>
#include <sys/resource.h>

#include <OpenGLES/ES2/gl.h>
#else // __ANDROID__ or _WIN32

// OpenGL ES includes
#include <SDL2/SDL_opengles2.h>
#include"GLES2_Context.h"
#endif

class SimpleRenderer
{
	GLES2_Context* m_pContext = NULL;
public:
    SimpleRenderer(GLES2_Context*context);
    ~SimpleRenderer();
    void Draw();
    void UpdateWindowSize(GLsizei width, GLsizei height);

private:
    GLuint mProgram;
    GLsizei mWindowWidth;
    GLsizei mWindowHeight;

    GLint mPositionAttribLocation;
    GLint mColorAttribLocation;

    GLint mModelUniformLocation;
    GLint mViewUniformLocation;
    GLint mProjUniformLocation;

    GLuint mVertexPositionBuffer;
    GLuint mVertexColorBuffer;
    GLuint mIndexBuffer;

    int mDrawCount;
};
