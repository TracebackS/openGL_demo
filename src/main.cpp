#include "ShaderLoader.h"
#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_query.hpp>
#include <cmath>
#include <utility>

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
	vPosition = 0
};

const GLfloat pi = 3.141591653589793238462643383279502f;
GLfloat sphere[20][20][2][3];
bool mouse_check = false;
bool key_w = false;
bool key_s = false;
bool key_a = false;
bool key_d = false;

glm::vec3 eye = glm::vec3(0.0f, 0.0f, -1.5f);
GLfloat forward_h = 0.0f;
GLfloat forward_v = pi;

glm::mat4 ModelS = glm::mat4(1.0f);
glm::mat4 ModelR = glm::mat4(1.0f);
glm::mat4 ModelT = glm::mat4(1.0f);

glm::mat4 clip_view = glm::perspective(pi / 2, 4.0f / 3.0f, 0.01f, 100.0f);

static void APIENTRY simple_print_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
										   const GLchar *message, const void *userParam)
{
	printf("Debug message  '%s'\n", message);
}

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

void init(void);
void display(void);

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_Window *window = SDL_CreateWindow("openGL demo",
										  0, // x position
										  0, // y position
										  1920, // wideth
										  1080, // height
										  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		fprintf(stderr, "Could not create a window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_CreateContext(window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	printf("OpenGL Version %s loaded.\n", glGetString(GL_VERSION));
	SDL_SetRelativeMouseMode(SDL_TRUE);

	init();
	SDL_Event e;
	while (true)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 0;
				}
				case SDL_MOUSEMOTION:
				{
					forward_v = std::fmin(forward_v - e.motion.yrel * 0.01f, pi / 2 - 0.00000001f);
					forward_v = std::fmax(forward_v, -pi / 2 + 0.00000001f);
					forward_h = fmod(forward_h + e.motion.xrel * 0.01f, 2 * pi);
					break;
				}
				case SDL_MOUSEWHEEL:
				{
					eye.y += e.wheel.y * 0.01f;
					break;
				}
				case SDL_KEYDOWN:
				{
					switch (e.key.keysym.scancode)
					{
						case SDL_SCANCODE_W:
						{
							key_w = true;
							break;
						}
						case SDL_SCANCODE_S:
						{
							key_s = true;
							break;
						}
						case SDL_SCANCODE_A:
						{
							key_a = true;
							break;
						}
						case SDL_SCANCODE_D:
						{
							key_d = true;
							break;
						}
						case SDL_SCANCODE_ESCAPE:
						{
							SDL_DestroyWindow(window);
							SDL_Quit();
							return 0;
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case SDL_KEYUP:
				{
					switch (e.key.keysym.scancode)
					{
						case SDL_SCANCODE_W:
						{
							key_w = false;
							break;
						}
						case SDL_SCANCODE_S:
						{
							key_s = false;
							break;
						}
						case SDL_SCANCODE_A:
						{
							key_a = false;
							break;
						}
						case SDL_SCANCODE_D:
						{
							key_d = false;
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
		if (key_w)
		{
			eye += 0.05f * glm::vec3(cos(forward_h), 0.0f, sin(forward_h));
		}
		if (key_s)
		{
			eye -= 0.05f * glm::vec3(cos(forward_h), 0.0f, sin(forward_h));
		}
		if (key_a)
		{
			eye -= 0.05f * glm::vec3(-sin(forward_h), 0.0f, cos(forward_h));
		}
		if (key_d)
		{
			eye += 0.05f * glm::vec3(-sin(forward_h), 0.0f, cos(forward_h));
		}
		display();
		SDL_GL_SwapWindow(window);
	}
}

void init(void)
{
//	glEnable(GL_DEBUG_OUTPUT);
//	glDebugMessageCallback(simple_print_callback, NULL);
//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	loadShader("default.vert");
	loadShader("default.frag");
	useProgram();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLfloat z_last = -0.7;
	for (int i = 0; i < 20; i++)
	{
		GLfloat z = 0.7 * sin(((i + 1.0f - 10.0f) / 20.0f) * pi);
		for (int j = 0; j < 20; j++)
		{
			GLfloat x = 0.7 * cos((j / 10.0f) * pi);
			GLfloat y = 0.7 * sin((j / 10.0f) * pi);
			sphere[i][j][0][0] = x * cos(((i - 10.0f) / 20.0f) * pi);
			sphere[i][j][0][2] = y * cos(((i - 10.0f) / 20.0f) * pi);
			sphere[i][j][0][1] = z_last;
			sphere[i][j][1][0] = x * cos(((i + 1.0f - 10.0f) / 20.0f) * pi);
			sphere[i][j][1][2] = y * cos(((i + 1.0f - 10.0f) / 20.0f) * pi);
			sphere[i][j][1][1] = z;
		}
		z_last = z;
	}
}

void display(void)
{
	GLfloat bg_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	glClearBufferfv(GL_COLOR, 0, bg_color);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 View = glm::lookAt(eye, eye + glm::vec3(cos(forward_h), forward_v, sin(forward_h)), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = ModelT * ModelR * ModelS;

	glm::mat4 clip_model = clip_view * View * Model;
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(clip_model));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 20 * 20 * 2);

	GLfloat axis[6][3] = {
		{-100.0f, 0.0f, 0.0f},
		{100.0f, 0.0f, 0.0f},
		{0.0f, -100.0f, 0.0f},
		{0.0f, 100.0f, 0.0f},
		{0.0f, 0.0f, -100.0f},
		{0.0f, 0.0f, 100.0f}
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 6);
}
