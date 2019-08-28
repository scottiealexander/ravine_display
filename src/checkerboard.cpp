// Include standard headers
#include <stdio.h>
#include <math.h>
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
#include <util/initialize.hpp>
#include <util/program.hpp>
#include <util/msequence.hpp>

#define ELEMENT_SIZE 32

#ifdef USE_OGL_33
    #pragma message "Using OGL 3.3"
#else
    #pragma message "Using OGL 1.4"
#endif

void usage(const char* prg)
{
    printf("Usage: %s <options>\n", prg);
    printf("    -f: fullscreen\n");
    printf("    -r [n=1]: repeat sequence <n> times\n");
    printf("    -t [n=1]: frames-per-term (update display every <n> frames)\n");
    printf("    -d: dump text of first frame\n");
    printf("    -h: show help\n");
}

int main(int narg, char** args)
{
    bool fullscreen = false;
    int repeats = 1;
    int frames_per_term = 1;
    bool dump = false;
    if (narg > 1)
    {
        for (int k = 1; k < narg; ++k)
        {
            std::string arg(args[k]);
            if (arg == "-f")
            {
                fullscreen = true;
            }
            else if (arg == "-r" && (k+1) < narg)
            {
                repeats = atoi(args[k+1]);
            }
            else if (arg == "-t" && (k+1) < narg)
            {
                frames_per_term = atoi(args[k+1]);
            }
            else if (arg == "-d")
            {
                dump = true;
            }
            else if (arg == "-h")
            {
                usage(args[0]);
                return 0;
            }
        }
    }

    MSequence seq("./m-sequence.bin");
    if (!seq.isvalid())
    {
        printf("[ERROR]: failed to read file: %s\n", "./m-sequence.bin");
        return -3;
    }
    else
    {
        printf("[INFO]: running m-sequence for %ld frames\n", (int64_t)(seq.length() * repeats));
    }

    if (dump)
    {
        int foo = 0;
        for (int k = 0; k < 256; ++k)
        {
            if (k % 16 == 0)
            {
                printf("\n");
            }
            printf("%d ", seq.get_bit(foo));
            foo += 128;
        }
        printf("\n");
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
    GLProgram program("./checkerboard/vertex.glsl", "./checkerboard/fragment.glsl");
#else
    GLProgram program("./checkerboard/vertex-1.4.glsl", "./checkerboard/fragment-1.4.glsl");
#endif

    if (program.get_id() < 1)
    {
        // Close OpenGL window and terminate GLFW
        printf("[ERROR]: failed to init GLSL programs\n");
        glfwTerminate();
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    printf("SCREEN SIZE: %d x %d\n", width, height);
    static const float center[2] = {(float)width / 2.0f, (float)height / 2.0f};

    float hw = (ELEMENT_SIZE / (float)width) * 16.0f;
    float hh = (ELEMENT_SIZE / (float)height) * 16.0f;

    int square_height = (int)floor(hh * (float)height);
    int square_width = (int)floor(hw * (float)width);

    int origin[2] = {
        (width - square_width) / 2,
        (height - square_height) / 2
    };

    printf("SQUARE HEIGHT = %d\n", square_height);
    printf("HH = %f\n", hh);
    printf("SQUARE WIDTH = %d\n", square_width);
    printf("HW = %f\n", hw);
    printf("ORIGIN: [%d, %d]\n", origin[0], origin[1]);

    static const GLfloat g_vertex_buffer_data[] = {
		-hw, -hh, 0.0f,
		 hw, -hh, 0.0f,
         hw,  hh, 0.0f,
		-hw,  hh, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLint pattern = program.get_uniform("pattern");
    GLint px_side_length = program.get_uniform("px_side_length");
    GLint px_height = program.get_uniform("px_height");

    GLint px_origin = program.get_uniform("px_origin");
#ifndef USE_OGL_33
    GLint vertex = program.get_attribute("vertexPosition");
#endif

    int32_t* pattern_array;

    // // 16x16 identity matrix
    // int32_t pattern_array[8] = {
    //     131073,
    //     524292,
    //     2097168,
    //     8388672,
    //     33554688,
    //     134218752,
    //     536875008,
    //     -2147467264
    // };

    for (size_t kframe = 0; kframe < (seq.length() * repeats); ++kframe)
    {
        if (kframe % frames_per_term == 0)
        {
            pattern_array = seq.get_frame(kframe);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        program.use();

        glUniform1i(px_side_length, ELEMENT_SIZE);
        glUniform1i(px_height, square_height);
        glUniform1iv(pattern, 8, pattern_array);
        glUniform2iv(px_origin, 1, origin);

#ifdef USE_OGL_33
        glEnableVertexAttribArray(0);
#else
        glEnableVertexAttribArray(vertex);
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

#ifdef USE_OGL_33
        glDisableVertexAttribArray(0);
#else
        glDisableVertexAttribArray(vertex);
#endif
        glfwSwapBuffers(window);

        // if (kframe % frames_per_term == 0)
        // {
        //     // NOTE TODO FIXME
        //     // send frame trigger
        // }

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwWindowShouldClose(window) > 0)
        {
            break;
        }
    }

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