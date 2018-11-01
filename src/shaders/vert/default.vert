#version 420 core
layout (location = 0) in vec4  v_position;
uniform mat4 clip_model;
void main()
{
	gl_Position = clip_model * v_position;
}
