// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;


#include <util/initialize.hpp>
#include <util/program.hpp>
#include <util/parse_ini.hpp>

#include "utility_classes.hpp"

/* 2 * PI */
#define TWOPI 6.2831855f

/* Conversion factor from degrees to radians: */
#define DEG2RAD 0.017453292f

int get_refresh_rate()
{
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    return mode->refreshRate;
}

int main(int narg, char** args)
{
    int ret;
    GLFWwindow* window = init(false, ret);

    if ((ret < 0) || (window == NULL))
    {
        printf("[ERROR]: failed to init GLFW / GLEW - %d\n", ret);
        return ret;
    }

    glfwSwapInterval(1);
    float refresh_interval = 1.0f / (float)get_refresh_rate();

    float g_orientation = 0.0f;
    float g_temporal_frequency = 0.0f * TWOPI;
    float g_spatial_frequency = 0.01f * TWOPI;
    float g_radius = 200.0f;
    float g_contrast = 1.0f;

    if (narg > 1)
    {
        g_orientation = atof(args[1]) * DEG2RAD;
        if (narg > 2)
        {
            // cycles-per-second to radians-per-second
            g_temporal_frequency = atof(args[2]) * TWOPI;
        }
        if (narg > 3)
        {
            // cycles-per-pixel to radians-per-pixel
            g_spatial_frequency = atof(args[3]) * TWOPI;
        }
        if (narg > 4)
        {
            g_contrast = atof(args[4]);
        }
        if (narg > 5)
        {
            g_radius = atof(args[5]);
        }
    }

    printf("O: %f | T: %f | S: %f | C: %f | R: %f\n",
        g_orientation, g_temporal_frequency, g_spatial_frequency, g_contrast, g_radius);

	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwPollEvents();

	// gray background
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // initialize VBO
    GLVBO vbo;

	// Create and compile our GLSL program from the shaders
	// GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    /* ---------------------------------------------------------------------- */

#ifdef USE_OGL_33
    // Create and compile our GLSL program from the shaders
    GLProgram program("./grating/vertex.glsl", "./grating/fragment.glsl");
#else
    GLProgram program("./grating/vertex-1.4.glsl", "./grating/fragment-1.4.glsl");
#endif

    if (program.getID() < 1)
    {
        // Close OpenGL window and terminate GLFW
        printf("[ERROR]: failed to init GLSL programs\n");
        glfwTerminate();
        return -1;
    }

    /* ---------------------------------------------------------------------- */

    // Get a handle for our uniform program variables
    GLint center = program.getUniform("center");
    GLint radius = program.getUniform("radius");
    GLint freq = program.getUniform("freq");
    GLint contrast = program.getUniform("contrast");
    GLint ori = program.getUniform("ori");

    GLint phase = program.getAttribute("phase");

    GLint vertex = program.getAttribute("vertexPosition");
    /* ---------------------------------------------------------------------- */

    static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    static const float position[2] = {(float)width / 2.0f, (float)height / 2.0f};

    // rad-per-frame = rad-per-sec * sec-per-frame
    float rad_per_frame = g_temporal_frequency * refresh_interval;
    printf("Rad-per-frame: %f [%f]\n", rad_per_frame, refresh_interval);

    unsigned int sleep_duration = (unsigned int)(refresh_interval * 1000000.0f) - 6000;
    printf("Sleep duration: %d\n", (int)sleep_duration);

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    sleep(2);

    float cur_phase = TWOPI;
    float tstart = glfwGetTime();
    float tnow, tlast, t1, t2;

    t1 = tnow = tstart;

    int inc = 0;

	do
    {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
        program.use();

        glUniform2fv(center, 1 , position);
        glUniform1f(radius, g_radius);
        glUniform1f(freq, g_spatial_frequency);
        glUniform1f(contrast, g_contrast);
        glUniform1f(ori, g_orientation);

        glVertexAttrib1f(phase, cur_phase);

		// 1rst attribute buffer : vertices
#ifdef USE_OGL_33
		glEnableVertexAttribArray(vertex);
#else
        glEnableVertexAttribArray(0);
#endif
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
#ifdef USE_OGL_33
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
#else
            vertex,
#endif
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// draw window size rect
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 4 indices starting at 0 -> 1 rectangle

		glDisableVertexAttribArray(vertex);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        tlast = tnow;
        tnow = glfwGetTime();

        // make 0deg = right, 90deg = up, etc...
        cur_phase -= (tnow - tlast) * g_temporal_frequency;

        if (cur_phase <= 0.0f)
        {
            cur_phase = TWOPI;
        }

        if (tnow - tstart > 5.0f)
        {
            break;
        }

        usleep(sleep_duration);

        if (inc % 10 == 0)
        {
            float t2 = glfwGetTime();
            printf("%d: %f\n", inc, (t2 - t1) / 10);
            t1 = t2;
        }

        ++inc;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

   printf("[INFO]: left main render loop @ %f\n", glfwGetTime());

   // glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
   glClear( GL_COLOR_BUFFER_BIT );
   glfwSwapBuffers(window);

   printf("[INFO]: screen cleared @ %f\n", glfwGetTime());
   sleep(1);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
    vbo.close();
    program.close();
    printf("[INFO]: deleted buffers @ %f\n", glfwGetTime());

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwDestroyWindow(window);
    printf("[INFO]: destroyed window @ %f\n", glfwGetTime());

	// Close OpenGL window and terminate GLFW
    printf("[INFO]: terminated GLFW @ %f\n", glfwGetTime());
    glfwTerminate();

	return 0;
}
