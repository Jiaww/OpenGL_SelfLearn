#include <GL\glew.h>
#include <GL/freeglut.h>
#include <gl\glut.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <SDL.h>
/* Using SDL2_image to load PNG & JPG in memory */
#include <SDL_image.h>
#define main SDL_main 
#include "shader_utils.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLint uniform_m_transform;
GLuint program;
GLint attribute_coord3d = -1, attribute_v_color = -1, attribute_texcoord = -1;
GLint uniform_fade;
GLuint vao;
GLuint vbo;
GLuint vbo_cube_vertices, vbo_cube_colors, vbo_cube_texcoords;
GLuint ibo_cube_elements;
int screen_width = 800, screen_height = 600;
GLint uniform_mvp;
//texture
GLuint texture_id, program_id;
GLint uniform_mytexture;

struct attributes {
	GLfloat coord3d[3];
	GLfloat v_color[3];
};
using namespace std;

// Any time the window is resized, this function gets called. It's setup to the 
// "glutReshapeFunc" in main.
void changeViewport(int w, int h) {
	glViewport(0, 0, w, h);

}

// In init_resources, we'll create our GLSL program. In render, we'll draw the triangle. In free_resources, we'll destroy the GLSL program.
bool init_resources() {
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	// Load texture
	SDL_Surface* res_texture = IMG_Load("test3.png");
	if (res_texture == NULL) {
		cerr << "IMG_Load: " << SDL_GetError() << endl;
		return false;
	}
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, // target
		0,  // level, 0 = base, no minimap,
		GL_RGBA, // internalformat
		res_texture->w,  // width
		res_texture->h,  // height
		0,  // border, always 0 in OpenGL ES
		GL_RGBA,  // format
		GL_UNSIGNED_BYTE, // type
		res_texture->pixels);
	SDL_FreeSurface(res_texture);


	// Cube:
	GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// top
		-1.0,  1.0,  1.0,
		1.0,  1.0,  1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
		// back
		1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		1.0,  1.0, -1.0,
		// bottom
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		// left
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		// right
		1.0, -1.0,  1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		1.0,  1.0,  1.0,
	};
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	//GLfloat cube_colors[] = {
	//	// front colors
	//	1.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0,
	//	0.0, 0.0, 1.0,
	//	1.0, 1.0, 1.0,
	//	// back colors
	//	1.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0,
	//	0.0, 0.0, 1.0,
	//	1.0, 1.0, 1.0,
	//};
	//glGenBuffers(1, &vbo_cube_colors);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	// set UV 
	GLfloat cube_texcoords[2 * 4 * 6] = {
		// front
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	for (int i = 1; i < 6; i++)
		memcpy(&cube_texcoords[i * 4 * 2], &cube_texcoords[0], 2 * 4 * sizeof(GLfloat));
	glGenBuffers(1, &vbo_cube_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
		// front
		0,  1,  2,
		2,  3,  0,
		// top
		4,  5,  6,
		6,  7,  4,
		// back
		8,  9, 10,
		10, 11,  8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	//// Setting data and vbo
	//struct attributes mesh[] = {
	//	{ { 0.0,  0.8, 0.0 },{ 1.0, 1.0, 0.0 } },
	//	{ { -0.8, -0.8, 0.0 },{ 0.0, 0.0, 1.0 } },
	//	{ { 0.8, -0.8, 0.0 },{ 1.0, 0.0, 0.0 } },
	//};
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);

	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	// vertex shader & fragment shader
	GLuint vs, fs;
	if ((vs = create_shader("test.vert.glsl", GL_VERTEX_SHADER)) == 0) {
		cerr << "Fail to create vertex shader";
		return false;
	}
	if ((fs = create_shader("test.frag.glsl", GL_FRAGMENT_SHADER)) == 0) {
		cerr << "Fail to create vertex shader";
		return false;
	}

	// initialize and link program
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(program);
		return false;
	}

	// bind in variables with their handles
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	attribute_name = "texcoord";
	attribute_texcoord = glGetAttribLocation(program, attribute_name);
	
	if (attribute_texcoord == -1) {
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	if (attribute_coord3d == -1) {
		cerr << "Could not bind attribute " << attribute_name << endl;
		return false;
	}
	if (attribute_v_color == -1) {
		cerr << "Could not bind attribute " << attribute_name << endl;
		//return false;
	}

	// bind uniform variable
	/*const char* uniform_name;
	uniform_name = "fade";
	uniform_fade = glGetUniformLocation(program, uniform_name);
	if (uniform_fade == -1) {
		cerr << "Could not bind uniform_fade " << uniform_name << endl;
		return false;
	}*/
	/*uniform_name = "m_transform";
	uniform_m_transform = glGetUniformLocation(program, uniform_name);
	if (uniform_m_transform == -1) {
		cerr << "Could not bind uniform " << uniform_name << endl;
		return false;
	}
	return true;*/
	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}
	return true;

}

void free_resources() {
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &texture_id);
}

void render(SDL_Window* window);

void logic() {
	//// alpha 0->1->0 every 5 seconds
	//float cur_fade = sinf(SDL_GetTicks() / 1000.0 * (2 * 3.14) / 5) / 2 + 0.5;
	//glUseProgram(program);
	//glUniform1f(uniform_fade, cur_fade);
	//float move = sinf(SDL_GetTicks() / 1000.0 * (2 * 3.14) / 5); // -1<->+1 every 5 seconds
	//float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
	//glm::vec3 axis_z(0, 0, 1);
	//glm::mat4 m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0))
	//	* glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z);
	//glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));

	// set mvp matrix
	float angle = SDL_GetTicks() / 1000.0 * 15;  // base 15° per second
	glm::mat4 anim = \
		glm::rotate(glm::mat4(1.0f), glm::radians(angle*3.0f), glm::vec3(1, 0, 0)) *  // X axis
		glm::rotate(glm::mat4(1.0f), glm::radians(angle*2.0f), glm::vec3(0, 1, 0)) *  // Y axis
		glm::rotate(glm::mat4(1.0f), glm::radians(angle*4.0f), glm::vec3(0, 0, 1));   // Z axis

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width / screen_height, 0.1f, 10.0f);
	glm::mat4 mvp = projection * view * model * anim;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void onResize(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

void mainLoop(SDL_Window* window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
		}
		if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			onResize(ev.window.data1, ev.window.data2);
		logic();
		render(window);
	}
}

void render(SDL_Window* window) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(program);
	// vertex position & color
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glEnableVertexAttribArray(attribute_coord3d);
	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
	glEnableVertexAttribArray(attribute_v_color);
	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_v_color, // attribute
		3,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
	glEnableVertexAttribArray(attribute_texcoord);
	glVertexAttribPointer(
		attribute_texcoord, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);
	// set Uniform
	//glUniform1f(uniform_fade, 0.1);

	// Activate texture
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	/* Push each element in buffer_vertices to the vertex shader */
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;  
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glEnable(GL_DEPTH_TEST);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	/* Display the result */
	SDL_GL_SwapWindow(window);
}
int main(int argc, char** argv) {
	// Initialize GLUT
	glutInit(&argc, argv);

	/* SDL-related initialising functions */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("My Textured Cube",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);

	/* Extension wrangler initialising */
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

	/* When all init functions run without errors,
	the program can initialise the resources */
	if (!init_resources())
		return EXIT_FAILURE;

	/* We can display something if everything goes OK */
	mainLoop(window);

	/* If the program exits in the usual way,
	free resources and exit with a success */
	free_resources();
	return EXIT_SUCCESS;
	
}