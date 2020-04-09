#include <dvigl/shader.h>


bool Shader::compile_and_link(std::string vs_content, std::string fs_content)
{
    GLuint vs_ID;
    GLuint fs_ID;
    vs_ID = compile(GL_VERTEX_SHADER, vs_content);
    fs_ID = compile(GL_FRAGMENT_SHADER, fs_content);
    if ((!vs_ID) || (!fs_ID))
    {
        return false;
    }

    program_object = glCreateProgram();
    glAttachShader(program_object, vs_ID);
    glAttachShader(program_object, fs_ID);

    glBindAttribLocation(program_object, attr_pos_loc, "attr_pos");
    glBindAttribLocation(program_object, attr_normal_loc, "attr_normal");
    glBindAttribLocation(program_object, attr_tangent_loc, "attr_tangent");
    glBindAttribLocation(program_object, attr_texcoord_loc, "attr_texcoord");

    glLinkProgram(program_object);

    GLint linked = 0;
    glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            infoLog[infoLen - 1] = '\0';
            glGetProgramInfoLog(program_object, infoLen, NULL, infoLog);
            LOG("Error on linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program_object);
        return false;
    }
    return true;
}

GLuint Shader::compile(GLenum type, std::string content)
{
    GLint compiled;
    GLuint shaderID;

    shaderID = glCreateShader(type);

    const char* src = content.c_str();
    glShaderSource(shaderID, 1, &src, NULL);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shaderID, infoLen, NULL, infoLog);
            LOG("Error on compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shaderID);
        return 0;
    }
    return shaderID;
}

void Shader::bind() { glUseProgram(program_object); }

void Shader::uniformMatrix4(std::string name, glm::mat4 value)
{
    int location = get_uniform_location(name);
    glUniformMatrix4fv(location, 1 /*only setting 1 matrix*/, false /*transpose?*/, glm::value_ptr(value));
}

void Shader::uniformMatrix4(std::string name, std::vector<glm::mat4>* values)
{
    int location = get_uniform_location(name);
    glUniformMatrix4fv(location, values->size(), false /*transpose?*/, (float*)values->data());
}

void Shader::uniformMatrix3(std::string name, glm::mat3 value)
{
    int location = get_uniform_location(name);
    glUniformMatrix3fv(location, 1 /*only setting 1 matrix*/, false /*transpose?*/, glm::value_ptr(value));
}

void Shader::uniform1i(std::string name, int value)
{
    int location = get_uniform_location(name);
    glUniform1i(location, value);
}

void Shader::uniform1f(std::string name, float value)
{
    int location = get_uniform_location(name);
    glUniform1f(location, value);
}

void Shader::uniform1b(std::string name, bool value)
{
    int location = get_uniform_location(name);
    glUniform1i(location, value ? 1 : 0);
}

void Shader::uniform3f(std::string name, glm::vec3 value)
{
    int location = get_uniform_location(name);
    glUniform3fv(location, 1, glm::value_ptr(value));
}

GLint Shader::get_uniform_location(std::string name)
{
    GLint location = glGetUniformLocation(program_object, name.c_str());
    if (location == INVALID_UNIFORM_LOCATION)
    {
        LOG("Warning! Unable to get the location of uniform '%s'\n", name.c_str());
    }
    return location;
}

void Shader::release() { glDeleteProgram(program_object); }
