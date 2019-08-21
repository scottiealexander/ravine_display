#include <cmath>
#include <cinttypes>
#include <unistd.h>

#include "initialize.hpp"
#include "run.hpp"

int get_refresh_rate()
{
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    return mode->refreshRate;
}

void run(GLProgram& program, RunSpec& rs, GLFWwindow* window)
{
    float frames_per_second = static_cast<float>(get_refresh_rate());
    float refresh_interval = 1.0 / frames_per_second;

    glfwSwapInterval(1);

    /* ---------------------------------------------------------------------- */
    // Get a handle for our uniform program variables
    GLint center = program.get_uniform("center");
    GLint radius = program.get_uniform("radius");
    GLint freq = program.get_uniform("freq");
    GLint contrast = program.get_uniform("contrast");
    GLint ori = program.get_uniform("ori");

    GLint phase = program.get_attribute("phase");
#ifndef USE_OGL_33
    GLint vertex = program.get_attribute("vertexPosition");
#endif
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

    uint32_t interframe_sleep = static_cast<uint32_t>(floor((refresh_interval * MICROSEC_PER_SECOND) - 6000.0f));

    float g_duration = rs.get("duration");
    float g_blank = rs.get("blank");
    uint32_t intertrial_sleep = static_cast<uint32_t>(floor(g_blank * MICROSEC_PER_SECOND));

    // glfw report an integer frame rate, so for a ~60Hz monitor it reports
    // 59, so add 1 to get the fewest number of frame of at least g_duration
    int32_t frames_per_trial = static_cast<uint32_t>(ceil(g_duration * (frames_per_second + 1.0f)));

    // blank the screen
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    for (int ktrial = 0; ktrial < rs.length(); ++ktrial)
    {
        // grating parameters for this trial: varying value is automatically
        // updated by the RunSpec object
        float g_radius = rs.get("radius");
        float g_spatial_frequency = rs.get("spatial_frequency") * TWOPI;
        float g_contrast = rs.get("contrast");
        float g_orientation = rs.get("orientation") * DEG2RAD;

        // temporal_frequency (phase change in rad-per-frame)
        // rad-per-frame = rad-per-sec * sec-per-frame
        float g_temporal_frequency = rs.get("temporal_frequency") * TWOPI;
        float rad_per_frame = g_temporal_frequency * refresh_interval;

        float tstart, tnow, tlast, t1, t2;
        float cur_phase = TWOPI;
        float ifi_total = 0.0f;
        int count = 0;

        // inter-trial blank period
        usleep(intertrial_sleep);

        tstart = t1 = tnow = glfwGetTime();

        for (int kframe = 0; kframe < frames_per_trial; ++kframe)
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

            // 1st attribute buffer : vertices
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

            if (kframe == 0)
            {
                // tstart = t1 = tnow = glfwGetTime();
                /* NOTE TODO: SEND STIM ON TRIGGER*/
            }

            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();

            tlast = tnow;
            tnow = glfwGetTime();

            // subtraction makes 0 deg = right and 90 deg = up, etc...
            cur_phase -= rad_per_frame;

            if (cur_phase <= 0.0f)
            {
                cur_phase = TWOPI;
            }

            usleep(interframe_sleep);

            if ((kframe % 10 == 0) && kframe > 0)
            {
                float t2 = glfwGetTime();
                ifi_total += (t2 - t1) / 10;
                t1 = t2;
                ++count;
            }

        } // Check if the ESC key was pressed or the window was closed
        // while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        //        glfwWindowShouldClose(window) == 0 );

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);

        /* NOTE TODO: SEND STIM OFF TRIGGER*/
        printf("Trial duration: %f\n", glfwGetTime() - tstart);
        printf("Mean ifi: %f\n", ifi_total / count);
   }

   printf("[INFO]: screen cleared @ %f\n", glfwGetTime());
   sleep(1);

   glDeleteBuffers(1, &vertexbuffer);
}
