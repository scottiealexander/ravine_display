#include <stdio.h>
#include <math.h>
#include <limits>
// #include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/controls.hpp>

#include <util/initialize.hpp>
#include <util/program.hpp>
// #include <util/bmp.hpp>

#include "utility_classes.hpp"
#include "viewport.hpp"

#define NPT 2000
#define NTICKMAX 42
#define TWOPI 6.283185307179586

float offset_x = 0.0f;
float scale_x = 1.0f;
float offset_y = 0.0f;
float scale_y = 1.0f;
int mode = 0;

const int bordersize = 10;
const int ticksize = 10;

void keyPressHandler()
{
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
		// scale_x *= 1.25f;
        offset_y += 0.01;
	}
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
		// scale_x /= 1.25f;
        offset_y -= 0.01;
	}
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
		offset_x += 0.01f;
	}
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
		offset_x -= 0.01f;
	}
    else if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
    {
        offset_x = 0.0f;
		scale_x = 1.0f;
    }
}

int main(void)
{
    int ret;
    window = init(ret);

    if ((ret < 0) || (window == NULL))
    {
        return ret;
    }

    glfwMakeContextCurrent(window);

    GLfloat LineRange[2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,LineRange);
    printf("Minimum Line Width: %f\n", LineRange[0]);
    printf("Maximum Line Width: %f\n", LineRange[1]);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// white background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // initialize VBO
    GLVBO vbo;

    /* ---------------------------------------------------------------------- */

    // Create and compile our GLSL program from the shaders
    GLProgram program("plot/vertex.glsl", "plot/fragment.glsl");

    if (program.getID() < 1)
    {
        // Close OpenGL window and terminate GLFW
        glfwTerminate();
        return -1;
    }

    /* ---------------------------------------------------------------------- */

    // Get a handle for our uniform program variables
    GLint transform = program.getUniform("transform");
    GLint color = program.getUniform("color");

    /* ---------------------------------------------------------------------- */

    GLPointBuffer data;
    Point* ptr = new Point[NPT];
    float ymax = std::numeric_limits<float>::lowest();
    float ymin = std::numeric_limits<float>::infinity();

    // fill in our data
	for (size_t k = 0; k < NPT; ++k)
    {
		float x = (k - 0.0f) / 2000.0f;

		ptr[k].x = x;
		ptr[k].y = (10.0 * sin(x * 100.0f) / (1.0f + 100.0f * x * x)) + 2.0;
        if (ptr[k].y > ymax)
        {
            ymax = ptr[k].y;
        }

        if (ptr[k].y < ymin)
        {
            ymin = ptr[k].y;
        }
	}

    // printf("min_y = %f | max_y = %f\n", ymin, ymax);

    scale_x = 2.0f / (ptr[NPT-1].x - ptr[0].x);
    offset_x = (-ptr[0].x * scale_x) -1.0f;

    if (ymax > ymin)
    {
        scale_y = 2.0f / (ymax - ymin);
        offset_y = (-ymin * scale_y) - 1.0f;
    }
    else
    {
        scale_y = 1.0;
        offset_y = -ymin;
    }

    data.setData(ptr, NPT, GL_STATIC_DRAW);
    delete[] ptr;

    printf("scale_x = %f | offset_x = %f\n", scale_x, offset_x);
    printf("scale_y = %f | offset_y = %f\n", scale_y, offset_y);

    /* ---------------------------------------------------------------------- */

    static const Point border[4] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
    GLPointBuffer frame;
    frame.setData(border, 4, GL_STATIC_DRAW);

    GLPointBuffer tick_buffer;

    /* ---------------------------------------------------------------------- */
    int window_width, window_height;
    float pixel_x, pixel_y;

    // float last_scale = scale_x;
    // float last_offset_x = offset_x;
    // float last_offset_y = offset_y;

	do
    {
        keyPressHandler();

        // if ((offset_y != last_offset_y) || (offset_x != last_offset_x))
        // {
        //     printf("offset_x = %f | offset_y = %f\n", offset_x, offset_y);
        //     last_offset_x = offset_x;
        //     last_offset_y = offset_y;
        // }

        glfwGetFramebufferSize(window, &window_width, &window_height);

		// glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        // glUseProgram(programID);
        program.use();

        // Set our viewport, this will clip geometry
    	glViewport(
            bordersize + ticksize,
            bordersize + ticksize,
            window_width - bordersize * 2 - ticksize,
            window_height - bordersize * 2 - ticksize
        );

    	// Set the scissor rectangle,this will clip fragments
    	glScissor(
            bordersize + ticksize,
            bordersize + ticksize,
            window_width - bordersize * 2 - ticksize,
            window_height - bordersize * 2 - ticksize
        );

    	glEnable(GL_SCISSOR_TEST);

    	// Set our coordinate transformation matrix
        // Sacle first, then translate (THIS IS CRITICAL! and appears to have
        // been incorrect in the tutorial...)
        glm::mat4 xfm = glm::scale(
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(offset_x, offset_y, 0.0f)
            ),
            glm::vec3(scale_x, scale_y, 1.0f)
        );

    	glUniformMatrix4fv(transform, 1, GL_FALSE, glm::value_ptr(xfm));

    	// Set the color to red
    	GLfloat red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    	glUniform4fv(color, 1, red);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        // glEnable(GL_POINT_SMOOTH);
        // glPointSize(5.0f);
        data.bind();
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           2,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
        glDrawArrays(GL_LINE_STRIP, 0, data.getLength());
        // glDrawArrays(GL_POINTS, 0, data.getLength());
        // glDisable(GL_POINT_SMOOTH);

        glViewport(0, 0, window_width, window_height);
        glDisable(GL_SCISSOR_TEST);

        /* ---------------------------------------------------------------- */
    	/* Draw the borders */

    	// Calculate a transformation matrix that gives us the same normalized device coordinates as above
    	xfm = viewport_transform(
            bordersize + ticksize,
            bordersize + ticksize,
            window_width - bordersize * 2 - ticksize,
            window_height - bordersize * 2 - ticksize,
            window_width,
            window_height,
            pixel_x,
            pixel_y
        );

    	// Tell our vertex shader about it
    	glUniformMatrix4fv(transform, 1, GL_FALSE, glm::value_ptr(xfm));

    	// Set the color to black
    	GLfloat black[4] = {0, 0, 0, 1};
    	glUniform4fv(color, 1, black);

    	// Draw a border around our graph
    	frame.bind();
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    	glDrawArrays(GL_LINE_LOOP, 0, frame.getLength());

        /* ---------------------------------------------------------------- */
        /* Draw the y tick marks */

        Point ticks[NTICKMAX];

        for (int k = 0; k <= 20; k++)
        {
        	float y = -1 + k * 0.1;
        	float tickscale = (k % 10) ? 0.5 : 1;

        	ticks[k * 2].x = -1;
        	ticks[k * 2].y = y;
        	ticks[k * 2 + 1].x = -1 - ticksize * tickscale * pixel_x;
        	ticks[k * 2 + 1].y = y;
        }

        tick_buffer.setData(ticks, NTICKMAX, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_LINES, 0, NTICKMAX);

        /* ---------------------------------------------------------------- */
    	/* Draw the x tick marks */

    	float tickspacing = 0.1 * powf(10, -floor(log10(scale_x)));	// desired space between ticks, in graph coordinates
    	float left = -1.0 / scale_x - offset_x;	    // left edge, in graph coordinates
    	float right = 1.0 / scale_x - offset_x;	    // right edge, in graph coordinates
    	int left_k = ceil(left / tickspacing);	    // index of left tick, counted from the origin
    	int right_k = floor(right / tickspacing);	// index of right tick, counted from the origin
    	float rem = left_k * tickspacing - left;	// space between left edge of graph and the first tick

    	float firsttick = -1.0 + rem * scale_x;	// first tick in device coordinates

    	int nticks = right_k - left_k + 1;	// number of ticks to show

    	if (nticks > 21) { nticks = 21; } // should not happen

    	for (int k = 0; k < nticks; k++)
        {
    		float x = firsttick + k * tickspacing * scale_x;
    		float tickscale = ((k + left_k) % 10) ? 0.5 : 1;

    		ticks[k * 2].x = x;
    		ticks[k * 2].y = -1;
    		ticks[k * 2 + 1].x = x;
    		ticks[k * 2 + 1].y = -1 - ticksize * tickscale * pixel_y;
    	}

    	tick_buffer.setData(ticks, nticks * 2, GL_DYNAMIC_DRAW);
    	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    	glDrawArrays(GL_LINES, 0, nticks * 2);

        /* ---------------------------------------------------------------- */
        glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

    /* ---------------------------------------------------------------------- */

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    /* ---------------------------------------------------------------------- */

	return 0;
}
