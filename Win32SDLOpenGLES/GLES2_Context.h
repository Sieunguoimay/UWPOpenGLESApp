#pragma once
#include<SDL2/SDL.h>
#include<GLES3/gl3.h>
typedef struct GLES2_Context
{

#define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;

	SDL_PROC(void, glActiveTexture, (GLenum))
		SDL_PROC(void, glAttachShader, (GLuint, GLuint))
		SDL_PROC(void, glBindAttribLocation, (GLuint, GLuint, const char*))
		SDL_PROC(void, glBindTexture, (GLenum, GLuint))
		SDL_PROC(void, glBlendEquationSeparate, (GLenum, GLenum))
		SDL_PROC(void, glBlendFuncSeparate, (GLenum, GLenum, GLenum, GLenum))
		SDL_PROC(void, glClear, (GLbitfield))
		SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
		SDL_PROC(void, glCompileShader, (GLuint))
		SDL_PROC(GLuint, glCreateProgram, (void))
		SDL_PROC(GLuint, glCreateShader, (GLenum))
		SDL_PROC(void, glDeleteProgram, (GLuint))
		SDL_PROC(void, glDeleteShader, (GLuint))
		SDL_PROC(void, glDeleteTextures, (GLsizei, const GLuint*))
		SDL_PROC(void, glDisable, (GLenum))
		SDL_PROC(void, glDisableVertexAttribArray, (GLuint))
		SDL_PROC(void, glDrawArrays, (GLenum, GLint, GLsizei))
		SDL_PROC(void, glDrawElements, (GLenum, GLsizei, GLenum, const GLvoid*))
		SDL_PROC(void, glEnable, (GLenum))
		SDL_PROC(void, glEnableVertexAttribArray, (GLuint))
		SDL_PROC(void, glFinish, (void))
		SDL_PROC(void, glGenFramebuffers, (GLsizei, GLuint*))
		SDL_PROC(void, glGenTextures, (GLsizei, GLuint*))
		SDL_PROC(void, glGetBooleanv, (GLenum, GLboolean*))
		SDL_PROC(const GLubyte*, glGetString, (GLenum))
		SDL_PROC(GLenum, glGetError, (void))
		SDL_PROC(void, glGetIntegerv, (GLenum, GLint*))
		SDL_PROC(void, glGetProgramiv, (GLuint, GLenum, GLint*))
		SDL_PROC(void, glGetShaderInfoLog, (GLuint, GLsizei, GLsizei*, char*))
		SDL_PROC(void, glGetShaderiv, (GLuint, GLenum, GLint*))
		SDL_PROC(GLint, glGetUniformLocation, (GLuint, const char*))
		SDL_PROC(void, glLinkProgram, (GLuint))
		SDL_PROC(void, glPixelStorei, (GLenum, GLint))
		SDL_PROC(void, glReadPixels, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*))
		SDL_PROC(void, glScissor, (GLint, GLint, GLsizei, GLsizei))
		SDL_PROC(void, glShaderBinary, (GLsizei, const GLuint*, GLenum, const void*, GLsizei))
#if __NACL__ || __ANDROID__
		SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar**, const GLint*))
#else
		SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar* const*, const GLint*))
#endif
		SDL_PROC(void, glTexImage2D, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
		SDL_PROC(void, glTexParameteri, (GLenum, GLenum, GLint))
		SDL_PROC(void, glTexSubImage2D, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
		SDL_PROC(void, glUniform1i, (GLint, GLint))
		SDL_PROC(void, glUniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat))
		SDL_PROC(void, glUniformMatrix4fv, (GLint, GLsizei, GLboolean, const GLfloat*))
		SDL_PROC(void, glUseProgram, (GLuint))
		SDL_PROC(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void*))
		SDL_PROC(void, glViewport, (GLint, GLint, GLsizei, GLsizei))
		SDL_PROC(void, glBindFramebuffer, (GLenum, GLuint))
		SDL_PROC(void, glFramebufferTexture2D, (GLenum, GLenum, GLenum, GLuint, GLint))
		SDL_PROC(GLenum, glCheckFramebufferStatus, (GLenum))
		SDL_PROC(void, glDeleteFramebuffers, (GLsizei, const GLuint*))
		SDL_PROC(GLint, glGetAttribLocation, (GLuint, const GLchar*))
		SDL_PROC(void, glGetProgramInfoLog, (GLuint, GLsizei, GLsizei*, GLchar*))
		SDL_PROC(void, glGenBuffers, (GLsizei, GLuint*))
		SDL_PROC(void, glDeleteBuffers, (GLsizei, const GLuint*))
		SDL_PROC(void, glBindBuffer, (GLenum, GLuint))
		SDL_PROC(void, glBufferData, (GLenum, GLsizeiptr, const GLvoid*, GLenum))
		SDL_PROC(void, glBufferSubData, (GLenum, GLintptr, GLsizeiptr, const GLvoid*))
#undef SDL_PROC

} GLES2_Context;

static int LoadContext(GLES2_Context* data)
{
#if SDL_VIDEO_DRIVER_UIKIT
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_ANDROID
#define __SDL_NOGETPROCADDR__
#elif SDL_VIDEO_DRIVER_PANDORA
#define __SDL_NOGETPROCADDR__
#endif

#if defined __SDL_NOGETPROCADDR__
#define SDL_PROC(ret,func,params) data->func=func;
#else
	SDL_Log("Hey1\n");

#define SDL_PROC(ret,func,params) \
    do { \
		SDL_Log("Hey2.5\n"); \
        data->func =  (ret (__stdcall*)params)SDL_GL_GetProcAddress(#func); \
		SDL_Log("Hey2.6\n"); \
        if ( ! data->func ) { \
			SDL_Log("Hey2.7\n"); \
            return SDL_SetError("Couldn't load GLES2 function %s: %s", #func, SDL_GetError()); \
        } \
		SDL_Log("Hey3\n"); \
	} while ( 0 );

#endif /* __SDL_NOGETPROCADDR__ */
	SDL_Log("Hey2\n"); \

	SDL_PROC(void, glActiveTexture, (GLenum))
		SDL_Log("Hey4\n"); \
		SDL_PROC(void, glAttachShader, (GLuint, GLuint))
		SDL_PROC(void, glBindAttribLocation, (GLuint, GLuint, const char*))
		SDL_PROC(void, glBindTexture, (GLenum, GLuint))
		SDL_PROC(void, glBlendEquationSeparate, (GLenum, GLenum))
		SDL_PROC(void, glBlendFuncSeparate, (GLenum, GLenum, GLenum, GLenum))
		SDL_PROC(void, glClear, (GLbitfield))
		SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
		SDL_PROC(void, glCompileShader, (GLuint))
		SDL_PROC(GLuint, glCreateProgram, (void))
		SDL_PROC(GLuint, glCreateShader, (GLenum))
		SDL_PROC(void, glDeleteProgram, (GLuint))
		SDL_PROC(void, glDeleteShader, (GLuint))
		SDL_PROC(void, glDeleteTextures, (GLsizei, const GLuint*))
		SDL_PROC(void, glDisable, (GLenum))
		SDL_PROC(void, glDisableVertexAttribArray, (GLuint))
		SDL_PROC(void, glDrawArrays, (GLenum, GLint, GLsizei))
		SDL_PROC(void, glDrawElements, (GLenum, GLsizei, GLenum, const GLvoid*))
		SDL_PROC(void, glEnable, (GLenum))
		SDL_PROC(void, glEnableVertexAttribArray, (GLuint))
		SDL_PROC(void, glFinish, (void))
		SDL_PROC(void, glGenFramebuffers, (GLsizei, GLuint*))
		SDL_PROC(void, glGenTextures, (GLsizei, GLuint*))
		SDL_PROC(void, glGetBooleanv, (GLenum, GLboolean*))
		SDL_PROC(const GLubyte*, glGetString, (GLenum))
		SDL_PROC(GLenum, glGetError, (void))
		SDL_PROC(void, glGetIntegerv, (GLenum, GLint*))
		SDL_PROC(void, glGetProgramiv, (GLuint, GLenum, GLint*))
		SDL_PROC(void, glGetShaderInfoLog, (GLuint, GLsizei, GLsizei*, char*))
		SDL_PROC(void, glGetShaderiv, (GLuint, GLenum, GLint*))
		SDL_PROC(GLint, glGetUniformLocation, (GLuint, const char*))
		SDL_PROC(void, glLinkProgram, (GLuint))
		SDL_PROC(void, glPixelStorei, (GLenum, GLint))
		SDL_PROC(void, glReadPixels, (GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*))
		SDL_PROC(void, glScissor, (GLint, GLint, GLsizei, GLsizei))
		SDL_PROC(void, glShaderBinary, (GLsizei, const GLuint*, GLenum, const void*, GLsizei))
#if __NACL__ || __ANDROID__
		SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar**, const GLint*))
#else
		SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar* const*, const GLint*))
#endif
		SDL_PROC(void, glTexImage2D, (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*))
		SDL_PROC(void, glTexParameteri, (GLenum, GLenum, GLint))
		SDL_PROC(void, glTexSubImage2D, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
		SDL_PROC(void, glUniform1i, (GLint, GLint))
		SDL_PROC(void, glUniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat))
		SDL_PROC(void, glUniformMatrix4fv, (GLint, GLsizei, GLboolean, const GLfloat*))
		SDL_PROC(void, glUseProgram, (GLuint))
		SDL_PROC(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void*))
		SDL_PROC(void, glViewport, (GLint, GLint, GLsizei, GLsizei))
		SDL_PROC(void, glBindFramebuffer, (GLenum, GLuint))
		SDL_PROC(void, glFramebufferTexture2D, (GLenum, GLenum, GLenum, GLuint, GLint))
		SDL_PROC(GLenum, glCheckFramebufferStatus, (GLenum))
		SDL_PROC(void, glDeleteFramebuffers, (GLsizei, const GLuint*))
		SDL_PROC(GLint, glGetAttribLocation, (GLuint, const GLchar*))
		SDL_PROC(void, glGetProgramInfoLog, (GLuint, GLsizei, GLsizei*, GLchar*))
		SDL_PROC(void, glGenBuffers, (GLsizei, GLuint*))
		SDL_PROC(void, glDeleteBuffers, (GLsizei, const GLuint*))
		SDL_PROC(void, glBindBuffer, (GLenum, GLuint))
		SDL_PROC(void, glBufferData, (GLenum, GLsizeiptr, const GLvoid*, GLenum))
		SDL_PROC(void, glBufferSubData, (GLenum, GLintptr, GLsizeiptr, const GLvoid*))
#undef SDL_PROC
		return 0;
}
