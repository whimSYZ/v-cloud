#pragma once
#define GLEW_STATIC

#include "../external/glm/glm.hpp"
#include "../external/glm/gtc/matrix_transform.hpp"
#include "../external/glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// misc
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cfloat>

static GLuint compileShader(std::string filename, GLenum shader_type)
{
    std::ifstream t(filename);

    // read whole file into stringstream buffer
    std::string source_code;
    std::stringstream buffer;
    buffer << t.rdbuf();
    t.close();
    source_code = buffer.str();

    GLuint shader = glCreateShader(shader_type);

    const GLchar *source = (const GLchar *)source_code.c_str();

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[2048];
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        std::cout << "Error when compiling shader " <<  filename.c_str() << ":\n" << infoLog << std::endl;

        // We don't need the shader anymore.
        glDeleteShader(shader);
    }

    return shader;
}

static GLuint newShader(std::string vertexFile, std::string fragmentFile)
{
    // Read our shaders into the appropriate buffers
    GLuint vertexShader = compileShader(vertexFile, GL_VERTEX_SHADER);       // Get source code for vertex shader.
    GLuint fragmentShader = compileShader(fragmentFile, GL_FRAGMENT_SHADER); // Get source code for fragment shader.

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[512];
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        std::cout << "Error when linking shaders "<< vertexFile.c_str() << " " << fragmentFile.c_str() << ":\n" << infoLog << std::endl;
        // We don't need the program anymore.
        glDeleteProgram(program);

        return program;
    }
   std::cout << "Successfully linked vertex and fragment shaders " << vertexFile.c_str() << " " << fragmentFile.c_str() << std::endl;
    return program;
}

static unsigned int newCompute(std::string filename)
{
    GLuint shader = compileShader(filename, GL_COMPUTE_SHADER);
    GLuint program = glCreateProgram();

    glAttachShader(program, shader);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(shader);

    return program;
}

void setBool(unsigned int ID, const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void setInt(unsigned int ID, const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void setuInt(unsigned int ID, const std::string &name, unsigned int *value)
{
    glUniform1uiv(glGetUniformLocation(ID, name.c_str()), 1, value);
}

void setFloat(unsigned int ID, const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void setVec2(unsigned int ID, const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void setVec2(unsigned int ID, const std::string &name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void setVec3(unsigned int ID, const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void setVec3(unsigned int ID, const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void setVec4(unsigned int ID, const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void setVec4(unsigned int ID, const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void setMat2(unsigned int ID, const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void setMat3(unsigned int ID, const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void setMat4(unsigned int ID, const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
