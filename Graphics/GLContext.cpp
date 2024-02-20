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

void GLContext::PASetVertexBuffer(uint slot, uint buffer, uint offset, uint stride)
{
    mVertexBuffer = buffer;

    if(mVertexArray){
        glVertexArrayVertexBuffer(mVertexArray, slot, mVertexBuffer, offset, stride);
    }
}

void GLContext::PASetInputLayout(uint layout)
{
    mVertexArray = layout;
    //glVertexArrayVertexBuffer(mVertexArray, 0, mVertexBuffer, )
}

void GLContext::ReflectShader(uint shader)
{
    static const GLenum props[] = {GL_TYPE, GL_LOCATION };
    const int nProps = ARRAYSIZE(props);
    GLchar queryName[34] = {};
    GLint attribs[nProps] = {};

    int nInput{};

    glGetProgramInterfaceiv(shader, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &nInput);

    if(!nInput) return;

    for(int i = 0; i < nInput; ++i){
        glGetProgramResourceiv(shader, GL_PROGRAM_INPUT, i, nProps, props,
        nProps, nullptr, attribs);
        
    }




    //GL_FLOAT_VEC3
    //glGetProgramResourceName(shader, GL_PROGRAM_OUTPUT)
}
