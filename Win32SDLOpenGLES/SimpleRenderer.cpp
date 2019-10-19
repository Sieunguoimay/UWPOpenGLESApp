//
// This file is used by the template to render a basic scene using GL.
#include<GLES3/gl3.h>


#include "SimpleRenderer.h"
#include "MathHelper.h"

// These are used by the shader compilation methods.
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>


GLuint CompileShader(GLenum type, const std::string &source,GLES2_Context*m_pContext)
{
    GLuint shader = m_pContext->glCreateShader(type);

    const char *sourceArray[1] = { source.c_str() };
    m_pContext->glShaderSource(shader, 1, sourceArray, NULL);
    m_pContext->glCompileShader(shader);

    GLint compileResult;
    m_pContext->glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

    if (compileResult == 0)
    {
        GLint infoLogLength;
        m_pContext->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        m_pContext->glGetShaderInfoLog(shader, (GLsizei)infoLog.size(), NULL, infoLog.data());

        std::string errorMessage = std::string("Shader compilation failed: ");
        errorMessage += std::string(infoLog.begin(), infoLog.end()); 

        throw std::runtime_error(errorMessage.c_str());
    }

    return shader;
}

GLuint CompileProgram(const std::string &vsSource, const std::string &fsSource,GLES2_Context*m_pContext)
{
    GLuint program = m_pContext->glCreateProgram();

    if (program == 0)
    {
        throw std::runtime_error("Program creation failed");
    }

    GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSource,m_pContext);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSource, m_pContext);

    if (vs == 0 || fs == 0)
    {
        m_pContext->glDeleteShader(fs);
        m_pContext->glDeleteShader(vs);
        m_pContext->glDeleteProgram(program);
        return 0;
    }

    m_pContext->glAttachShader(program, vs);
    m_pContext->glDeleteShader(vs);

    m_pContext->glAttachShader(program, fs);
    m_pContext->glDeleteShader(fs);

    m_pContext->glLinkProgram(program);

    GLint linkStatus;
    m_pContext->glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == 0)
    {
        GLint infoLogLength;
        m_pContext->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        m_pContext->glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

        std::string errorMessage = std::string("Program link failed: ");
        errorMessage += std::string(infoLog.begin(), infoLog.end()); 

        throw std::runtime_error(errorMessage.c_str());
    }

    return program;
}

SimpleRenderer::SimpleRenderer(GLES2_Context* context) :
	m_pContext(context),
    mWindowWidth(0),
    mWindowHeight(0),
    mDrawCount(0)
{
    // Vertex Shader source
    const std::string vs = R"(
        uniform mat4 uModelMatrix;
        uniform mat4 uViewMatrix;
        uniform mat4 uProjMatrix;
        attribute vec4 aPosition;
        attribute vec4 aColor;
        varying vec4 vColor;
        void main()
        {
            gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
            vColor = aColor;
        }
    )";

    // Fragment Shader source
    const std::string fs = R"(
        precision mediump float;
        varying vec4 vColor;
        void main()
        {
            gl_FragColor = vColor;
        }
    )";

    // Set up the shader and its uniform/attribute locations.
    mProgram = CompileProgram(vs, fs,m_pContext);
    mPositionAttribLocation = m_pContext->glGetAttribLocation(mProgram, "aPosition");
    mColorAttribLocation = m_pContext->glGetAttribLocation(mProgram, "aColor");
    mModelUniformLocation = m_pContext->glGetUniformLocation(mProgram, "uModelMatrix");
    mViewUniformLocation = m_pContext->glGetUniformLocation(mProgram, "uViewMatrix");
    mProjUniformLocation = m_pContext->glGetUniformLocation(mProgram, "uProjMatrix");

    // Then set up the cube geometry.
    GLfloat vertexPositions[] =
    {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
    };

    m_pContext->glGenBuffers(1, &mVertexPositionBuffer);
    m_pContext->glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    m_pContext->glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    GLfloat vertexColors[] =
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
    };

    m_pContext->glGenBuffers(1, &mVertexColorBuffer);
    m_pContext->glBindBuffer(GL_ARRAY_BUFFER, mVertexColorBuffer);
    m_pContext->glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);

    short indices[] =
    {
        0, 1, 2, // -x
        1, 3, 2,

        4, 6, 5, // +x
        5, 6, 7,

        0, 5, 1, // -y
        0, 4, 5,

        2, 7, 6, // +y
        2, 3, 7,
              
        0, 6, 4, // -z
        0, 2, 6,
              
        1, 7, 3, // +z
        1, 5, 7,
    };

    m_pContext->glGenBuffers(1, &mIndexBuffer);
    m_pContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    m_pContext->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

SimpleRenderer::~SimpleRenderer()
{
    if (mProgram != 0)
    {
        m_pContext->glDeleteProgram(mProgram);
        mProgram = 0;
    }

    if (mVertexPositionBuffer != 0)
    {
        m_pContext->glDeleteBuffers(1, &mVertexPositionBuffer);
        mVertexPositionBuffer = 0;
    }

    if (mVertexColorBuffer != 0)
    {
        m_pContext->glDeleteBuffers(1, &mVertexColorBuffer);
        mVertexColorBuffer = 0;
    }

    if (mIndexBuffer != 0)
    {
        m_pContext->glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
    }
}

void SimpleRenderer::Draw()
{

    m_pContext->glEnable(GL_DEPTH_TEST);
    m_pContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mProgram == 0)
        return;

    m_pContext->glUseProgram(mProgram);
	
    m_pContext->glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    m_pContext->glEnableVertexAttribArray(mPositionAttribLocation);
    m_pContext->glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    m_pContext->glBindBuffer(GL_ARRAY_BUFFER, mVertexColorBuffer);
    m_pContext->glEnableVertexAttribArray(mColorAttribLocation);
    m_pContext->glVertexAttribPointer(mColorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    MathHelper::Matrix4 modelMatrix = MathHelper::SimpleModelMatrix((float)mDrawCount / 1000.0f);
    m_pContext->glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, &(modelMatrix.m[0][0]));

    MathHelper::Matrix4 viewMatrix = MathHelper::SimpleViewMatrix();
    m_pContext->glUniformMatrix4fv(mViewUniformLocation, 1, GL_FALSE, &(viewMatrix.m[0][0]));

    MathHelper::Matrix4 projectionMatrix = MathHelper::SimpleProjectionMatrix(float(mWindowWidth) / float(mWindowHeight));
    m_pContext->glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, &(projectionMatrix.m[0][0]));

    // Draw 36 indices: six faces, two trianm_pContext->gles per face, 3 indices per trianm_pContext->gle
    m_pContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	m_pContext->glDrawElements(GL_TRIANGLES, (6 * 2) * 3, GL_UNSIGNED_SHORT, 0);

    mDrawCount += 1;
}

void SimpleRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
    m_pContext->glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;
}
