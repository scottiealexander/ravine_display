#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "initialize.hpp"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow* init(bool fullscreen, int& ret)
{
    ret = 0;

    glfwSetErrorCallback(error_callback);

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf(stderr, "Failed to initialize GLFW\n" );
		getchar();
		ret = -1;
        return NULL;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing (4 samples-per-pixel)
#ifdef USE_OGL_33
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE); // OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want old OpenGL
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#endif
    GLFWmonitor* monitor;
    int width, height;

    if (fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width - 20;
        height = mode->height;
    }
    else
    {
        monitor = NULL;
        width = 1024;
        height = 768;
    }

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(width, height, "V-STIM", monitor, NULL);
	if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        ret = -1;
        return NULL;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
    glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
    {
    	fprintf(stderr, "Failed to initialize GLEW\n");
    	getchar();
    	glfwTerminate();
    	ret = -1;
        return NULL;
    }

    return window;
}
