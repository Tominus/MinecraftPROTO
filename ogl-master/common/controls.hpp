#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
const glm::mat4& getViewMatrix();
const glm::mat4& getProjectionMatrix();
const glm::vec3& getPosition();

#endif