#pragma once

#include "GLConfig.h"

class GLContext{
    public:
    GLContext();

    void Create(int x, int y, HWND hwnd);
    void Release();

    void CreateVertexShader(const LPCSTR file, GLuint* shader);

    //Pipeline Assbley
    void PASetVertexBuffer(uint slot, uint buffer, uint offset, uint stride);
    void PASetInputLayout(uint layout);

    void ReflectShader(uint shader);

    HGLRC mRC;
    HDC mDC;
    HWND mHwnd;

    struct PipelineAssambly;

    uint mPipeline = 0;
    uint mVertexArray = 0;
    uint mVertexBuffer = 0;     //cureent vbo
};

