#include "EventProcessor.h"
#include "ShaderLoader.h"
#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_query.hpp>
#include <utility>

#define DEBUG 0

enum VAO_IDs
{
	Triangles,
	NumVAOs
};

enum Buffer_IDs
{
	ArrayBuffer,
	NumBuffers
};

enum Attrib_IDs
{
	VertexPosition = 0,
	VertexColor,
};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLfloat pi = 3.141591653589793238462643383279502f;

glm::mat4 ModelS = glm::mat4(1.0f);
glm::mat4 ModelR = glm::mat4(1.0f);
glm::mat4 ModelT = glm::mat4(1.0f);

glm::mat4 clip_view = glm::perspective(pi / 2, 4.0f / 3.0f, 0.01f, 100.0f);

#if DEBUG
static void APIENTRY simple_print_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
										   const GLchar *message, const void *userParam)
{
	printf("Debug message  '%s'\n", message);
}
#endif

void init(void);
void display(void);
int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_Window *window = SDL_CreateWindow("openGL demo", 0, 0, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GL_CreateContext(window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

#if DEBUG
	printf("OpenGL Version %s loaded.\n", glGetString(GL_VERSION));
#endif

	init();
	while (true)
	{
		processEvent(window);
		display();
		SDL_GL_SwapWindow(window);
	}
}
void init(void)
{
#if DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(simple_print_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
	glEnable(GL_DEPTH_TEST);

	glGenBuffers(NumBuffers, Buffers);
	glGenVertexArrays(NumVAOs, VAOs);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBindVertexArray(VAOs[Triangles]);

	loadShader("default.vert");
	loadShader("default.frag");
	useProgram();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	const GLfloat axis[6][3] = {{-100.0f, 0.0f, 0.0f}, {100.0f, 0.0f, 0.0f},  {0.0f, -100.0f, 0.0f},
								{0.0f, 100.0f, 0.0f},  {0.0f, 0.0f, -100.0f}, {0.0f, 0.0f, 100.0f}};
	GLfloat sphere[200][200][2][3];
	GLfloat z_last = -0.7;
	for (int i = 0; i < 200; i++)
	{
		GLfloat z = 0.7 * sin(((i + 1.0f - 100.0f) / 200.0f) * pi);
		for (int j = 0; j < 200; j++)
		{
			GLfloat x = 0.7 * cos((j / 100.0f) * pi);
			GLfloat y = 0.7 * sin((j / 100.0f) * pi);
			sphere[i][j][0][0] = x * cos(((i - 100.0f) / 200.0f) * pi);
			sphere[i][j][0][2] = y * cos(((i - 100.0f) / 200.0f) * pi);
			sphere[i][j][0][1] = z_last;
			sphere[i][j][1][0] = x * cos(((i + 1.0f - 100.0f) / 200.0f) * pi);
			sphere[i][j][1][2] = y * cos(((i + 1.0f - 100.0f) / 200.0f) * pi);
			sphere[i][j][1][1] = z;
		}
		z_last = z;
	}

	GLfloat floor[4][3] = {
		{-100.0f, -5.0f, -100.0f}, {-100.0f, -5.0f, 100.0f}, {100.0f, -5.0f, -100.0f}, {100.0f, -5.0f, 100.0f}};

	glVertexAttribPointer(VertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VertexPosition);
	glVertexAttrib4f(VertexColor, 1.0f, 1.0f, 1.0f, 1.0f);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere) + sizeof(axis) + sizeof(floor), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sphere), sphere);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sphere), sizeof(axis), axis);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sphere) + sizeof(axis), sizeof(floor), floor);
}

void display(void)
{
	GLfloat bg_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 View = glm::lookAt(glm::vec3(eye_x, eye_y, eye_z),
								 glm::vec3(eye_x, eye_y, eye_z) + glm::vec3(cos(forward_h), forward_v, sin(forward_h)),
								 glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = ModelT * ModelR * ModelS;
	glm::mat4 clip_model = clip_view * View * Model;

	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(clip_model));
	glUniform3f(4, 0.3f, 0.3f, 0.3f);
	glUniform3f(5, 0.0f, 0.5f, 0.5f);
	glUniform3f(6, 1.0f, 1.0f, 1.0f);
	glUniform3f(7, cos(forward_h), forward_v, sin(forward_h));
	glUniform1f(8, 20.0f);
	glUniform1f(9, 0.5f);
	glUniform3f(10, light_x, light_y, light_z);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 200 * 200 * 2);
	glDrawArrays(GL_LINES, 200 * 200 * 2, 6);
	glDrawArrays(GL_TRIANGLE_STRIP, 200 * 200 * 2 + 6, 4);
}
