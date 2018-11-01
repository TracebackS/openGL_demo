#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ShaderLoader.h"

GLuint shaders[6] = {0};
GLuint program = 0;

void loadShader(const char *shader)
{
	char shader_type[5];
	strncpy(shader_type, shader + strlen(shader) - 4, 5);

	char fn[MAX_FILENAME_LEN] = "./shaders/";
	strcat(fn, shader_type);
	strcat(fn, "/");
	strcat(fn, shader);

	FILE *fp = fopen(fn, "r");
	if (!fp)
	{
		perror("loadShader: cannot open file"); //TODO
		return;
	}
	fseek(fp, 0, SEEK_END);
	long file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = (char *)malloc(file_len + 1);
	fread(src, 1, file_len, fp);
	src[file_len] = '\0';
	fclose(fp);

	int type;

	if (!strncmp(shader_type, "vert", 4))
	{
		shaders[VERT] = glCreateShader(GL_VERTEX_SHADER);
		type = VERT;
	}
	else if (!strncmp(shader_type, "tesc", 4))
	{
		shaders[TESC] = glCreateShader(GL_TESS_CONTROL_SHADER);
		type = TESC;
	}
	else if (!strncmp(shader_type, "tese", 4))
	{
		shaders[TESE] = glCreateShader(GL_TESS_EVALUATION_SHADER);
		type = TESE;
	}
	else if (!strncmp(shader_type, "geom", 4))
	{
		shaders[GEOM] = glCreateShader(GL_GEOMETRY_SHADER);
		type = GEOM;
	}
	else if (!strncmp(shader_type, "frag", 4))
	{
		shaders[FRAG] = glCreateShader(GL_FRAGMENT_SHADER);
		type = FRAG;
	}
//	else if (!strncmp(shader_type, "comp", 4))
//	{
//		shaders[COMP] = glCreateShader(GL_COMPUTE_SHADER);
//		type = COMP;
//	}
	else
	{
		free(src);
		return;
	}
	const char *s = src;
	glShaderSource(shaders[type], 1, &s, NULL);
	glCompileShader(shaders[type]);
	free(src);
}

void useProgram()
{
	program = glCreateProgram();
	int i;
	for (i = 0; i < 6; i++)
	{
		if (glIsShader(shaders[i]))
		{
			glAttachShader(program, shaders[i]);
			glDeleteShader(shaders[i]);
		}
	}
	glLinkProgram(program);
	glUseProgram(program);
	glDeleteProgram(program);
}
