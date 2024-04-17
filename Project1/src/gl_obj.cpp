#include "gl_obj.h"
#include <sstream>
#include <iostream>
#include <fstream>

GLuint CreateShaderProgram(const std::string& sname) {

	GLuint program = glCreateProgram();

	GLuint vert = glCreateShader(GL_VERTEX_SHADER), frag = glCreateShader(GL_FRAGMENT_SHADER);

	std::stringstream vs, fs;
	vs << std::ifstream("res/" + sname + ".vert").rdbuf();
	fs << std::ifstream("res/" + sname + ".frag").rdbuf();
	std::string vcode{ vs.str() }, fcode{ fs.str() };

	const char* vstr = vcode.c_str(), * fstr = fcode.c_str();
	glShaderSource(vert, 1, &vstr, nullptr);
	glShaderSource(frag, 1, &fstr, nullptr);

	glCompileShader(vert);
	glCompileShader(frag);

	int status;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vert, 1024, &log_length, message);
		std::cout << message << std::endl;
	}

	glGetShaderiv(frag, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(frag, 1024, &log_length, message);
		std::cout << message << std::endl;
	}

	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	glGetShaderiv(frag, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(frag, 1024, &log_length, message);
		std::cout << message << std::endl;

	}

	glDeleteShader(vert);
	glDeleteShader(frag);
	return program;
}
