#include "EventProcessor.h"

#include <math.h>

#define PI 3.14159265358979323846264338327950288419716939937510582097

static char key_pressed_bitmap[32];
static float move_unit = 0.05;

void processEvent(SDL_Window *window)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
			{
				SDL_DestroyWindow(window);
				SDL_Quit();
				exit(0);
				break;
			}
			case SDL_APP_TERMINATING:
			case SDL_APP_LOWMEMORY:
			case SDL_APP_WILLENTERBACKGROUND:
			case SDL_APP_DIDENTERBACKGROUND:
			case SDL_APP_DIDENTERFOREGROUND:
			{
				break;
			}
			case SDL_WINDOWEVENT:
			case SDL_SYSWMEVENT:
			{
				break;
			}
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_TEXTEDITING:
			case SDL_TEXTINPUT:
			case SDL_KEYMAPCHANGED:
			{
				processKeyboardEvent(e);
				break;
			}
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
			{
				processMouseEvent(e);
				break;
			}
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
			{
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			{
				break;
			}
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
			{
				break;
			}
			case SDL_DOLLARGESTURE:
			case SDL_DOLLARRECORD:
			case SDL_MULTIGESTURE:
			{
				break;
			}
			case SDL_CLIPBOARDUPDATE:
			{
				break;
			}
			case SDL_DROPFILE:
			case SDL_DROPTEXT:
			case SDL_DROPBEGIN:
			case SDL_DROPCOMPLETE:
			{
				break;
			}
			case SDL_AUDIODEVICEADDED:
			case SDL_AUDIODEVICEREMOVED:
			{
				break;
			}
			case SDL_RENDER_TARGETS_RESET:
			case SDL_RENDER_DEVICE_RESET:
			{
				break;
			}
			case SDL_USEREVENT:
			case SDL_LASTEVENT:
			{
				break;
			}
			default:
			{
				break;
			}
		}
	}

	if (ifKeyPressed(SDL_SCANCODE_LSHIFT))
	{
		move_unit = 0.0005;
	}
	else
	{
		move_unit = 0.05;
	}
	if (ifKeyPressed(SDL_SCANCODE_W))
	{
		eye_x += move_unit * cos(forward_h);
		eye_z += move_unit * sin(forward_h);
	}
	if (ifKeyPressed(SDL_SCANCODE_S))
	{
		eye_x -= move_unit * cos(forward_h);
		eye_z -= move_unit * sin(forward_h);
	}
	if (ifKeyPressed(SDL_SCANCODE_A))
	{
		eye_x += move_unit * sin(forward_h);
		eye_z -= move_unit * cos(forward_h);
	}
	if (ifKeyPressed(SDL_SCANCODE_D))
	{
		eye_x -= move_unit * sin(forward_h);
		eye_z += move_unit * cos(forward_h);
	}
	if (ifKeyPressed(SDL_SCANCODE_ESCAPE))
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(0);
	}
}

static void processKeyboardEvent(SDL_Event e)
{
	switch (e.type)
	{
		case SDL_KEYDOWN:
		{
			pressKey(e.key.keysym.scancode);
			break;
		}
		case SDL_KEYUP:
		{
			releaseKey(e.key.keysym.scancode);
			break;
		}
		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT:
		case SDL_KEYMAPCHANGED:
		default:
		{
			break;
		}
	}
}

static void processMouseEvent(SDL_Event e)
{
	switch (e.type)
	{
		case SDL_MOUSEMOTION:
		{
			forward_v = fminf(forward_v - e.motion.yrel * 0.01f, PI / 2 - 0.000000001f);
			forward_v = fmaxf(forward_v, -PI / 2 + 0.000000001f);
			forward_h = fmodf(forward_h + e.motion.xrel * 0.01f, 2 * PI);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
		{
			eye_y += e.wheel.y * move_unit;
			break;
		}
		default:
		{
			break;
		}
	}
}

static void pressKey(SDL_Scancode key)
{
	key_pressed_bitmap[key / 8] |= 1 << (key % 8);
}

static void releaseKey(SDL_Scancode key)
{
	key_pressed_bitmap[key / 8] &= ~(1 << (key % 8));
}

static int ifKeyPressed(SDL_Scancode key)
{
	return key_pressed_bitmap[key / 8] & (1 << (key % 8));
}
