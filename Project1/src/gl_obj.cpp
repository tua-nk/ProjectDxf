#include "gl_obj.h"
#include <sstream>
#include <iostream>
#include <fstream>

GLuint CreateShaderProgram(const std::string& sname, int stageFlag) {
    if (stageFlag == 0)
    {
        stageFlag = ShaderStageFlag_vs | ShaderStageFlag_fs;
    }

    GLuint program = glCreateProgram();
    int status;

    GLuint vert = glCreateShader(GL_VERTEX_SHADER), frag = glCreateShader(GL_FRAGMENT_SHADER), geo = glCreateShader(GL_GEOMETRY_SHADER);

    if (stageFlag & ShaderStageFlag_vs)
    {
        std::stringstream vs;
        vs << std::ifstream("res/" + sname + ".vs").rdbuf();

        std::string vcode{ vs.str() };
        const char* vstr = vcode.c_str();
        glShaderSource(vert, 1, &vstr, nullptr);
        glCompileShader(vert);

        glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetShaderInfoLog(vert, 1024, &log_length, message);
            std::cout << message << std::endl;
        }

        glAttachShader(program, vert);
    }

    if (stageFlag & ShaderStageFlag_vs)
    {
        std::stringstream fs;
        fs << std::ifstream("res/" + sname + ".fs").rdbuf();
        std::string fcode{ fs.str() };
        const char* fstr = fcode.c_str();

        glShaderSource(frag, 1, &fstr, nullptr);
        glCompileShader(frag);

        glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetShaderInfoLog(frag, 1024, &log_length, message);
            std::cout << message << std::endl;
        }
        glAttachShader(program, frag);
    }

    if (stageFlag & ShaderStageFlag_gs)
    {
        std::stringstream gs;
        gs << std::ifstream("res/" + sname + ".gs").rdbuf();
        std::string gcode{ gs.str() };
        const char* gstr = gcode.c_str();

        glShaderSource(geo, 1, &gstr, nullptr);
        glCompileShader(geo);

        glGetShaderiv(geo, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei log_length = 0;
            GLchar message[1024];
            glGetShaderInfoLog(geo, 1024, &log_length, message);
            std::cout << message << std::endl;
        }
        glAttachShader(program, geo);
    }

    glLinkProgram(program);

    glGetShaderiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(program, 1024, &log_length, message);
        std::cout << message << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteShader(geo);
    return program;
}
