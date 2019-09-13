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
#include <util/event_client.hpp>
#include <util/initialize.hpp>
#include <util/program.hpp>
#include <util/msequence.hpp>

#include <util/parse_ini.hpp>

#ifdef USE_OGL_33
    #pragma message "Using OGL 3.3"
#else
    #pragma message "Using OGL 1.4"
#endif

void usage(const char* prg)
{
    printf("Usage: %s <options> <ini_filepath>\n", prg);
    printf("    -f: fullscreen\n");
    printf("    -d: dump text of first frame\n");
    printf("    -h: show help\n");
}

bool get_run_params(const char* filepath, PMap& p)
{
    p["repeats"] = 1.0f;
    p["frames-per-term"] = 4.0f;
    p["element-size"] = 32.0f;
    p["frame-trigger"] = 1.0f;

    std::vector<float> v;
    std::vector<uint8_t> t;
    std::string a;

    return parse_ini(filepath, p, a, v, t);
}

void dump_frame(MSequence& seq, int kframe = 0)
{
    int foo = kframe;
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

int main(int narg, char** args)
{
    bool fullscreen = false;
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

    PMap params;
    if (!get_run_params(args[narg-1], params))
    {
        printf("[ERROR]: failed to read ini file: %s\n", args[narg-1]);
        return -4;
    }

    MSequence seq("./m-sequence.bin");
    if (!seq.isvalid())
    {
        printf("[ERROR]: failed to read file: %s\n", "./m-sequence.bin");
        return -3;
    }

    // TCPClient trigger(HOST_IP, HOST_PORT);
    LogClient trigger;

    if (!trigger.sync_connect())
    {
        printf("[ERROR]: failed to connect to host\n");
        return -2;
    }

    if (dump)
    {
        dump_frame(seq, 0);
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

    // unpack / pre-calculate all the parameters we need to draw
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    printf("SCREEN SIZE: %d x %d\n", width, height);
    static const float center[2] = {(float)width / 2.0f, (float)height / 2.0f};

    int element_size = (int)params["element-size"];
    int frames_per_term = (int)params["frames-per-term"];
    int repeats = (int)params["repeats"];

    float hw = (element_size / (float)width) * 16.0f;
    float hh = (element_size / (float)height) * 16.0f;

    printf("SQUARE SIZE: %f x %f ", hw, hh);

    int square_width = (int)floor(hw * (float)width);
    int square_height = (int)floor(hh * (float)height);

    printf("(%d x %d)\n", square_width, square_height);

    int origin[2] = {
        (width - square_width) / 2,
        (height - square_height) / 2
    };

    if (hw > 1.0f) { hw = 1.0f; }
    if (hh > 1.0f) { hh = 1.0f; }

    // the rectangular frame
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

    printf("[INFO]: running m-sequence for %ld frames\n", (int64_t)(seq.length() * repeats));
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

    const uint8_t frame_trigger = (uint8_t)params["frame-trigger"];

    for (size_t kframe = 0; kframe < (seq.length() * repeats); ++kframe)
    {
        if (kframe % frames_per_term == 0)
        {
            pattern_array = seq.get_frame(kframe);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        program.use();

        glUniform1i(px_side_length, element_size);
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

        if (kframe % frames_per_term == 0)
        {
            // NOTE TODO FIXME
            trigger.sync_send(frame_trigger);
        }

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
