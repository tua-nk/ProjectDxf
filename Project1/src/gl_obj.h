#pragma once
#include <glad/glad.h>
#include <string>

enum ShaderStageFlag {
    ShaderStageFlag_vs = 1,
    ShaderStageFlag_fs = 2,
    ShaderStageFlag_gs = 4
};

GLuint CreateShaderProgram(const std::string& sname, int stageFlag = 0);