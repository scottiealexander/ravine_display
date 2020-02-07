#ifndef RUN_HPP_
#define RUN_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utility_classes.hpp"
#include "event_client.hpp"
#include "run_spec.hpp"

/* 2 * PI */
#define TWOPI 6.2831855f

/* Conversion factor from degrees to radians: */
#define DEG2RAD 0.017453292f

#define MICROSEC_PER_SECOND 1000000.0f

void run(GLProgram& program, RunSpec& rs, EventClient* trigger, GLFWwindow* window);

#endif
