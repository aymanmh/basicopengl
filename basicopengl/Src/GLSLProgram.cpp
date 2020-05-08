#include "GLSLProgram.h"

#include "glutils.h"

#include <fstream>

using std::ifstream;
using std::ios;
using std::string;

#include <sstream>
#include <sys/stat.h>
#include <vector>

namespace GLSLShaderInfo {
	std::map<std::string, GLSLShader::GLSLShaderType> extensions = {
		{".vs",   GLSLShader::VERTEX},
		{".vert", GLSLShader::VERTEX},
		{"_vert.glsl", GLSLShader::VERTEX},
		{".vert.glsl", GLSLShader::VERTEX },
		{".gs",   GLSLShader::GEOMETRY},
		{".geom", GLSLShader::GEOMETRY},
		{ ".geom.glsl", GLSLShader::GEOMETRY },
		{".tcs",  GLSLShader::TESS_CONTROL},
		{ ".tcs.glsl",  GLSLShader::TESS_CONTROL },
		{ ".tes",  GLSLShader::TESS_EVALUATION },
		{".tes.glsl",  GLSLShader::TESS_EVALUATION},
		{".fs",   GLSLShader::FRAGMENT},
		{".frag", GLSLShader::FRAGMENT},
		{"_frag.glsl", GLSLShader::FRAGMENT},
		{".frag.glsl", GLSLShader::FRAGMENT},
		{".cs",   GLSLShader::COMPUTE},
		{ ".cs.glsl",   GLSLShader::COMPUTE }
	};
}

GLSLProgram::GLSLProgram() : handle_m(0), linked_m(false) {}

GLSLProgram::~GLSLProgram()
{
	if (handle_m == 0) return;
	detachAndDeleteShaderObjects();
	// Delete the program
	glDeleteProgram(handle_m);
}

void GLSLProgram::detachAndDeleteShaderObjects() {
	// Detach and delete the shader objects (if they are not already removed)
	GLint numShaders = 0;
	glGetProgramiv(handle_m, GL_ATTACHED_SHADERS, &numShaders);
	std::vector<GLuint> shaderNames(numShaders);
	glGetAttachedShaders(handle_m, numShaders, NULL, shaderNames.data());
	for (GLuint shader : shaderNames) {
		glDetachShader(handle_m, shader);
		glDeleteShader(shader);
	}
}

void GLSLProgram::compileShader(const char* fileName)
{

	// Check the file name's extension to determine the shader type
	string ext = getExtension(fileName);
	GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
	auto it = GLSLShaderInfo::extensions.find(ext);
	if (it != GLSLShaderInfo::extensions.end()) {
		type = it->second;
	}
	else {
		string msg = "Unrecognized extension: " + ext;
		throw GLSLProgramException(msg);
	}

	// Pass the discovered shader type along
	compileShader(fileName, type);
}

string GLSLProgram::getExtension(const char* name)
{
	string nameStr(name);

	size_t dotLoc = nameStr.find_last_of('.');
	if (dotLoc != string::npos) {
		std::string ext = nameStr.substr(dotLoc);
		if (ext == ".glsl") {

			size_t loc = nameStr.find_last_of('.', dotLoc - 1);
			if (loc == string::npos) {
				loc = nameStr.find_last_of('_', dotLoc - 1);
			}
			if (loc != string::npos) {
				return nameStr.substr(loc);
			}
		}
		else {
			return ext;
		}
	}
	return "";
}

void GLSLProgram::compileShader(const char* fileName,
	GLSLShader::GLSLShaderType type)
{
	if (!fileExists(fileName)) {
		string message = string("Shader: ") + fileName + " not found.";
		throw GLSLProgramException(message);
	}

	if (handle_m <= 0) {
		handle_m = glCreateProgram();
		if (handle_m == 0) {
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	ifstream inFile(fileName, ios::in);
	if (!inFile) {
		string message = string("Unable to open: ") + fileName;
		throw GLSLProgramException(message);
	}

	// Get file contents
	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();

	compileShader(code.str(), type, fileName);
}

void GLSLProgram::compileShader(const string& source,
	GLSLShader::GLSLShaderType type,
	const char* fileName)
{
	if (handle_m <= 0) {
		handle_m = glCreateProgram();
		if (handle_m == 0) {
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	GLuint shaderHandle = glCreateShader(type);

	const char* c_code = source.c_str();
	glShaderSource(shaderHandle, 1, &c_code, NULL);

	// Compile the shader
	glCompileShader(shaderHandle);

	// Check for errors
	int result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) {
		// Compile failed, get log
		std::string msg;
		if (fileName) {
			msg = string(fileName) + ": shader compliation failed\n";
		}
		else {
			msg = "Shader compilation failed.\n";
		}

		int length = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			std::string log(length, ' ');
			int written = 0;
			glGetShaderInfoLog(shaderHandle, length, &written, &log[0]);
			msg += log;
		}
		throw GLSLProgramException(msg);
	}
	else {
		// Compile succeeded, attach shader
		glAttachShader(handle_m, shaderHandle);
	}
}

void GLSLProgram::link()
{
	if (linked_m) return;
	if (handle_m <= 0) throw GLSLProgramException("Program has not been compiled.");

	glLinkProgram(handle_m);
	int status = 0;
	std::string errString;
	glGetProgramiv(handle_m, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		// Store log and return false
		int length = 0;
		glGetProgramiv(handle_m, GL_INFO_LOG_LENGTH, &length);
		errString += "Program link failed:\n";
		if (length > 0) {
			std::string log(length, ' ');
			int written = 0;
			glGetProgramInfoLog(handle_m, length, &written, &log[0]);
			errString += log;
		}
	}
	else {
		findUniformLocations();
		linked_m = true;
	}

	detachAndDeleteShaderObjects();

	if (GL_FALSE == status) throw GLSLProgramException(errString);
}

void GLSLProgram::findUniformLocations()
{
	uniformLocations_m.clear();

	GLint numUniforms = 0;
	// For OpenGL 4.3 and above, use glGetProgramResource
	glGetProgramInterfaceiv(handle_m, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	for (GLint i = 0; i < numUniforms; ++i) {
		GLint results[4];
		glGetProgramResourceiv(handle_m, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] != -1) continue;  // Skip uniforms in blocks
		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];
		glGetProgramResourceName(handle_m, GL_UNIFORM, i, nameBufSize, NULL, name);
		uniformLocations_m[name] = results[2];
		delete[] name;
	}
}

void GLSLProgram::use()
{
	if (handle_m <= 0 || (!linked_m))
		throw GLSLProgramException("Shader has not been linked");
	glUseProgram(handle_m);
}

int GLSLProgram::getHandle()
{
	return handle_m;
}

bool GLSLProgram::isLinked()
{
	return linked_m;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char* name)
{
	glBindAttribLocation(handle_m, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char* name)
{
	glBindFragDataLocation(handle_m, location, name);
}

void GLSLProgram::setUniform(const char* name, float x, float y, float z)
{
	GLint loc = getUniformLocation(name);
	glUniform3f(loc, x, y, z);
}

void GLSLProgram::setUniform(const char* name, const glm::vec3& v)
{
	this->setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char* name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void GLSLProgram::setUniform(const char* name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

void GLSLProgram::setUniform(const char* name, const glm::mat4& m)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char* name, const glm::mat3& m)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::setUniform(const char* name, float val)
{
	GLint loc = getUniformLocation(name);
	glUniform1f(loc, val);
}

void GLSLProgram::setUniform(const char* name, int val)
{
	GLint loc = getUniformLocation(name);
	glUniform1i(loc, val);
}

void GLSLProgram::setUniform(const char* name, GLuint val)
{
	GLint loc = getUniformLocation(name);
	glUniform1ui(loc, val);
}

void GLSLProgram::setUniform(const char* name, bool val)
{
	int loc = getUniformLocation(name);
	glUniform1i(loc, val);
}

void GLSLProgram::printActiveUniforms()
{
	// For OpenGL 4.3 and above, use glGetProgramResource
	GLint numUniforms = 0;
	glGetProgramInterfaceiv(handle_m, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	printf("Active uniforms:\n");
	for (int i = 0; i < numUniforms; ++i) {
		GLint results[4];
		glGetProgramResourceiv(handle_m, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] != -1) continue;  // Skip uniforms in blocks
		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];
		glGetProgramResourceName(handle_m, GL_UNIFORM, i, nameBufSize, NULL, name);
		printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
		delete[] name;
	}
}

void GLSLProgram::printActiveUniformBlocks()
{
	GLint numBlocks = 0;

	glGetProgramInterfaceiv(handle_m, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
	GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
	GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };
	GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

	for (int block = 0; block < numBlocks; ++block) {
		GLint blockInfo[2];
		glGetProgramResourceiv(handle_m, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
		GLint numUnis = blockInfo[0];

		char* blockName = new char[blockInfo[1] + 1];
		glGetProgramResourceName(handle_m, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
		printf("Uniform block \"%s\":\n", blockName);
		delete[] blockName;

		GLint* unifIndexes = new GLint[numUnis];
		glGetProgramResourceiv(handle_m, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);

		for (int unif = 0; unif < numUnis; ++unif) {
			GLint uniIndex = unifIndexes[unif];
			GLint results[3];
			glGetProgramResourceiv(handle_m, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

			GLint nameBufSize = results[0] + 1;
			char* name = new char[nameBufSize];
			glGetProgramResourceName(handle_m, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
			printf("    %s (%s)\n", name, getTypeString(results[1]));
			delete[] name;
		}

		delete[] unifIndexes;
	}
}

void GLSLProgram::printActiveAttribs()
{
	// >= OpenGL 4.3, use glGetProgramResource
	GLint numAttribs;
	glGetProgramInterfaceiv(handle_m, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	printf("Active attributes:\n");
	for (int i = 0; i < numAttribs; ++i) {
		GLint results[3];
		glGetProgramResourceiv(handle_m, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];
		glGetProgramResourceName(handle_m, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
		printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
		delete[] name;
	}
}

const char* GLSLProgram::getTypeString(GLenum type)
{
	// There are many more types than are covered here, but
	// these are the most common in these examples.
	switch (type) {
	case GL_FLOAT:
		return "float";
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_DOUBLE:
		return "double";
	case GL_INT:
		return "int";
	case GL_UNSIGNED_INT:
		return "unsigned int";
	case GL_BOOL:
		return "bool";
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	default:
		return "?";
	}
}

void GLSLProgram::validate()
{
	if (!isLinked())
		throw GLSLProgramException("Program is not linked");

	GLint status;
	glValidateProgram(handle_m);
	glGetProgramiv(handle_m, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status) {
		// Store log and return false
		int length = 0;
		string logString;

		glGetProgramiv(handle_m, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			char* c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(handle_m, length, &written, c_log);
			logString = c_log;
			delete[] c_log;
		}

		throw GLSLProgramException(string("Program failed to validate\n") + logString);

	}
}

bool GLSLProgram::fileExists(const string& fileName)
{
	struct stat info;
	int ret = -1;

	ret = stat(fileName.c_str(), &info);
	return 0 == ret;
}