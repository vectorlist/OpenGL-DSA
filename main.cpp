#include "iostream"
#include "GLExtension.h"

LPCSTR vs = R"(
#version 450 core

out gl_PerVertex{ vec4 gl_Position;};

layout(location =0) in vec3 pos;
layout(location =1) in vec3 color;

out PS{
    vec4 col;
}ps;

void main(){
    gl_Position = vec4(pos, 1);
    ps.col = vec4(color,1);
};
)";

LPCSTR ps = R"(
#version 450 core

out vec4 outColor;

in PS{
    vec4 col;
}ps;

void main(){
    outColor = ps.col;
};

)";

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int main(int args, char* argv[])
{
    HWND hwnd = nullptr;

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = "Modern OpenGL";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(NULL, wc.lpszClassName, wc.lpszClassName, 
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, 500, 200, 800, 500, nullptr, 0, wc.hInstance, 0);

    GL::LoadGLExtension();
    auto context =  GL::CreateGLContext(hwnd);
    auto dc = GetDC(hwnd);

    uint32_t vertShader;
    uint32_t pixelShader;
    uint32_t pipeline;

    auto CompileShader = [](LPCSTR code, uint32_t type){
        GLint compiled = 0;
        GLuint shader;
        shader = glCreateShaderProgramv(type, 1, &code);

	    glProgramParameteri(shader, GL_PROGRAM_SEPARABLE, GL_TRUE);
	    glGetProgramiv(shader, GL_LINK_STATUS, &compiled);
	    if (compiled == GL_FALSE) {
		    std::string err;
		    err.reserve(512);
		    glGetProgramInfoLog(shader, 512, nullptr, (GLchar*)err.data());
		    glDeleteShader(shader);
		    MessageBox(NULL, err.data(), "Shader Compile Error", MB_OK);
	    }
        return shader;
    };

    glCreateProgramPipelines(1, &pipeline);

    vertShader = CompileShader(vs, GL_VERTEX_SHADER);
    pixelShader = CompileShader(ps, GL_FRAGMENT_SHADER);

    glBindProgramPipeline(pipeline);

    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT,vertShader);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT,pixelShader);
    

    struct vec3{
        float x,y,z;
    };
    struct Vertex{
        vec3 pos;
        vec3 color; 
    };

    const Vertex vertices[] = {
        {{-0.75,-0.75,0}, {1,0,0}},
        {{-.5,0.2f,0}, {1,0,1}},
        {{0, -0.5,0}, {0,1,1}},
    };

    const Vertex vertices1[] = {
        {{-0.5,-0.5,0}, {0,0,1}},
        {{0,1,0}, {0,0,1}},
        {{0.5,-0.5,0}, {0,0,1}},
    };



    uint32_t vbo,vbo1,vao;
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &vbo1);

    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(vbo1, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    //glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

    glCreateVertexArrays(1, &vao);
    
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, false, offsetof(Vertex, Vertex::color));
    glVertexArrayAttribBinding(vao, 1, 0);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    RECT rc{};
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    glViewport(0,0,w, h);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
            if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) PostQuitMessage(0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f,0.2f,0.2f,1.f);
        //glGetIntegerv();
        glBindVertexArray(vao);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glVertexArrayVertexBuffer(vao, 0, vbo1, 0, sizeof(Vertex));

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(dc);
    }
    
    GL::ValidateGLExtension(hwnd);
    
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
};

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
    return DefWindowProc(hwnd, msg, wp, lp);
}