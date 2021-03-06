#include <dvigl/shader.h>

bool contains(std::unordered_map<std::string, std::string> dict, std::string key)
{
    return dict.find(key) != dict.end();
}

bool Shader::FindAttribLocation(std::string name) {
  if (!program_object) {
    return false;
  }
  GLint location = glGetAttribLocation(program_object, name.c_str());
  if (glGetError() != GL_NO_ERROR || location == -1) {  // name not found.
    return false;
  }
  attribs_[name] = location;
  return true;
}

GLint Shader::attrib(std::string name)
{
    auto it = attribs_.find(name);
    if (it != attribs_.end())
    {
        return it->second;
    }

    LOG("Attrib '%s' not found\n", name.c_str());
    return -1;
}

bool Shader::compile_and_link(std::unordered_map<std::string, std::string> source)
// (std::string vs_content, std::string fs_content)
{
    GLuint vs_ID = 0;
    GLuint fs_ID = 0;
    GLuint cs_ID = 0;
    GLuint tcs_ID = 0;
    GLuint tes_ID = 0;
    GLuint gs_ID = 0;

    if (contains(source, "vertex"))
    {
        std::string content = source["vertex"];
        vs_ID = compile(GL_VERTEX_SHADER, content);
        if (!vs_ID)
        {
            return false;
        }
    }

    if (contains(source, "fragment"))
    {
        std::string content = source["fragment"];
        fs_ID = compile(GL_FRAGMENT_SHADER, content);
        if (!fs_ID)
        {
            return false;
        }
    }

    if (contains(source, "compute"))
    {
        std::string content = source["compute"];
        cs_ID = compile(GL_COMPUTE_SHADER, content);
        if (!cs_ID)
        {
            return false;
        }
    }
    if (contains(source, "tess_control"))
    {
        std::string content = source["tess_control"];
        tcs_ID = compile(GL_TESS_CONTROL_SHADER, content);
        if (!tcs_ID)
        {
            return false;
        }
    }
    if (contains(source, "tess_eval"))
    {
        std::string content = source["tess_eval"];
        tes_ID = compile(GL_TESS_EVALUATION_SHADER, content);
        if (!tes_ID)
        {
            return false;
        }
    }
    if (contains(source, "geometry"))
    {
        std::string content = source["geometry"];
        gs_ID = compile(GL_GEOMETRY_SHADER, content);
        if (!gs_ID)
        {
            return false;
        }
    }

    // LOG("VS content: \"%s\"\n", vs_content.c_str());
    // LOG("FS content: \"%s\"\n", fs_content.c_str());

    if ((vs_ID + fs_ID + cs_ID + tcs_ID + tes_ID + gs_ID) == 0)
    {
        return false;
    }

    program_object = glCreateProgram();
    if (vs_ID)
        glAttachShader(program_object, vs_ID);
    if (fs_ID)
        glAttachShader(program_object, fs_ID);
    if (cs_ID)
        glAttachShader(program_object, cs_ID);
    if (tcs_ID)
        glAttachShader(program_object, tcs_ID);
    if (tes_ID)
        glAttachShader(program_object, tes_ID);
    if (gs_ID)
        glAttachShader(program_object, gs_ID);

    glBindAttribLocation(program_object, attr_pos_loc, "attr_pos");
    glBindAttribLocation(program_object, attr_normal_loc, "attr_normal");
    glBindAttribLocation(program_object, attr_texcoord_loc, "attr_texcoord");
    glBindAttribLocation(program_object, attr_tangent_loc, "attr_tangent");
    glBindAttribLocation(program_object, attr_bitangent_loc, "attr_bitangent");

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

void Shader::uniform3i(std::string name, glm::ivec3 value)
{
    int location = get_uniform_location(name);
    glUniform3iv(location, 1, glm::value_ptr(value));
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
