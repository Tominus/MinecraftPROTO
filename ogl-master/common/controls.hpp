#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

void computeMatricesFromInputs();
const glm::mat4& getViewMatrix();
const glm::mat4& getProjectionMatrix();
const glm::vec3& getPosition();
const float& getPositionHeight();

#endif