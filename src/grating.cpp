// Include standard headers
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
// #include <glm/glm.hpp>
// using namespace glm;

#include <util/utility_classes.hpp>
#include <util/event_client.hpp>
#include <util/initialize.hpp>
#include <util/parse_ini.hpp>
#include <util/program.hpp>
#include <util/run.hpp>

#ifdef USE_OGL_33
    #pragma message "Using OGL 3.3"
#else
    #pragma message "Using OGL 1.4"
#endif

#define HOST_IP "127.0.0.1"
#define HOST_PORT 65001

void usage()
{
    printf("Usage: grating_ex <options> <ini_filepath>\n");
    printf("Options:\n");
    printf("    -f: fullscreen\n");
}

int main(int narg, char** args)
{

    char* inipath;
    bool fullscreen = false;
    if (narg < 2)
    {
        usage();
        return -1;
    }
    else if (narg == 2)
    {
        inipath = args[1];
    }
    else if (narg > 2)
    {
        std::string opt(args[1]);
        if (opt == "-f") { fullscreen = true; }
        inipath = args[narg-1];
    }

    RunSpec rs(inipath);

    if (!rs.isvalid())
    {
        printf("[ERROR]: failed to open file \"%s\"\n", inipath);
        return -2;
    }

    rs.show();

    TCPClient client(HOST_IP, HOST_PORT);
    // LogClient client;

    if (!client.sync_connect())
    {
        printf("[ERROR]: failed to connect to host - %s:%d\n", HOST_IP, HOST_PORT);
        return -2;
    }

    int ret;
    GLFWwindow* window = init(fullscreen, ret);

    if ((ret < 0) || (window == NULL))
    {
        printf("[ERROR]: failed to init GLFW / GLEW - %d\n", ret);
        return ret;
    }

    glfwSwapInterval(1);

	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwPollEvents();

	// gray background
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // initialize VBO
    GLVBO vbo;

#ifdef USE_OGL_33
    // Create and compile our GLSL program from the shaders
    GLProgram program("./grating/vertex.glsl", "./grating/fragment.glsl");
#else
    GLProgram program("./grating/vertex-1.4.glsl", "./grating/fragment-1.4.glsl");
#endif

    if (program.get_id() < 1)
    {
        // Close OpenGL window and terminate GLFW
        printf("[ERROR]: failed to init GLSL programs\n");
        glfwTerminate();
        return -1;
    }

    // RUN TRIALS
    run(program, rs, &client, window);

    // send close signal to client
    client.sync_send(0xff);

    printf("[INFO]: left main render loop @ %f\n", glfwGetTime());

	// Cleanup VBO
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
