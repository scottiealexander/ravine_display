#include <stdio.h>
// #include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
// using namespace glm;

#include <common/controls.hpp>

#include <util/initialize.hpp>
#include <util/program.hpp>
#include <util/bmp.hpp>

#include "utility_classes.hpp"

int main(void)
{
    int ret;
    window = init(ret);

    if ((ret < 0) || (window == NULL))
    {
        return ret;
    }

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
	glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // initialize VBO
    GLVBO vbo;

    /* ---------------------------------------------------------------------- */

    // // An array of 3 vectors which represents 3 vertices
    // static const GLfloat g_vertex_buffer_data[] =
    // {
    //    -1.0f, -1.0f, 0.0f,
    //     1.0f, -1.0f, 0.0f,
    //     0.0f,  1.0f, 0.0f,
    // };

#include "data.hpp"


    GLFloatBuffer vertexBuffer(3, 36);
    vertexBuffer.setData(g_vertex_buffer_data);


    GLFloatBuffer uvBuffer(2, 36);
    uvBuffer.setData(g_uv_buffer_data);

    /* ---------------------------------------------------------------------- */

    // Create and compile our GLSL program from the shaders
    GLProgram program("05/vertex.glsl", "05/fragment.glsl");

    if (program.getID() < 1)
    {
        // Close OpenGL window and terminate GLFW
        glfwTerminate();
        return -1;
    }

    /* ---------------------------------------------------------------------- */

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(program.getID(), "MVP");

    // // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// // Or, for an ortho camera :
    // // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    //
    // // Camera matrix
    // glm::mat4 View = glm::lookAt
    // (
    //     glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
    //     glm::vec3(0,0,0),  // and looks at the origin
    //     glm::vec3(0,1,0)   // Head is up (set to 0,-1,0 to look upside-down)
    // );
    //
    // // Model matrix : an identity matrix (model will be at the origin)
    // glm::mat4 Model = glm::mat4(1.0f);
    //
    // // glm::mat4 Scale = glm::scale(2.0f, 2.0f, 2.0f);
    // // glm::mat4 Translate = glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 0.0f));
    // // glm::mat4 Model = glm::scale(Translate, glm::vec3(3.0f, 3.0f, 3.0f));
    //
    // // Our ModelViewProjection : multiplication of our 3 matrices
    // glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

    /* ---------------------------------------------------------------------- */

    // GLTexture texture1(BMPImage("05/uvtemplate.bmp"));
    GLTexture texture1(DDSTexture("05/uvtemplate.DDS"));

    // Get a handle for our "textureSampler" uniform
    GLuint TextureID = glGetUniformLocation(program.getID(), "textureSampler");

    /* ---------------------------------------------------------------------- */

	do
    {
		// glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        // glUseProgram(programID);
        program.use();

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
		// in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		texture1.bind();
		// Set our "textureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        vertexBuffer.bind();
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        uvBuffer.bind();
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 12*3 vertices total -> 1 cube
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );

    /* ---------------------------------------------------------------------- */

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    /* ---------------------------------------------------------------------- */

	return 0;
}
