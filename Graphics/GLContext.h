#pragma once

#include "GLExtension.h"

class GLContext{
    public:
    GLContext();

    void Create(int x, int y, HWND hwnd);
    void Release();

    void CreateVertexShader(const LPCSTR file, GLuint* shader);



    HGLRC mRC;
    HDC mDC;
    HWND mHwnd;
    UINT mPipeline;
};

