#ifndef VIEWPORT_HPP_
#define VIEWPORT_HPP_

glm::mat4 viewport_transform(float x, float y, float width, float height,
    int window_width, int window_height, float& pixel_x, float& pixel_y)
{
	// Map OpenGL coordinates (-1,-1) to window coordinates (x,y),
	// (1,1) to (x + width, y + height).

	// Calculate how to translate the x and y coordinates:
	float x_offset = (2.0 * x + (width - window_width)) / window_width;
	float y_offset = (2.0 * y + (height - window_height)) / window_height;

	// Calculate how to rescale the x and y coordinates:
	float x_scale = width / window_width;
	float y_scale = height / window_height;

	// Calculate size of pixels in OpenGL coordinates
	pixel_x = 2.0 / width;
	pixel_y = 2.0 / height;

	return glm::scale(
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(x_offset, y_offset, 0.0f)
        ),
        glm::vec3(x_scale, y_scale, 1.0f)
    );
}

#endif
