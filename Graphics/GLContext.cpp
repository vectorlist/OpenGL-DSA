#include "GLContext.h"

GLContext::GLContext()
{
}

void GLContext::Create(int x, int y, HWND hwnd)
{
    mHwnd = hwnd;
    GL::LoadGLExtension();
    mRC = GL::CreateGLContext(mHwnd);
    mDC = GetDC(mHwnd);

    glCreateProgramPipelines(1, &mPipeline);
    glBindProgramPipeline(mPipeline);
}

void GLContext::Release()
{
    glDeleteProgramPipelines(1, &mPipeline);
    GL::ValidateGLExtension(mHwnd);
}

void CompileFromCode(LPCSTR code, khronos_uint32_t type, _Out_ GLuint* shader){
    GLint compiled{};
    *shader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &code);

    glProgramParameteri(*shader, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glGetProgramiv(*shader, GL_LINK_STATUS, &compiled);

    if(compiled == GL_FALSE){
        char err[256]{};
        glGetProgramInfoLog(*shader, 256, nullptr, err);
        glDeleteProgram(*shader);
        MessageBox(nullptr, err, "Compile Error : ", MB_OK);
    }
}

void GLContext::CreateVertexShader(const LPCSTR file, GLuint *shader)
{
       
}
