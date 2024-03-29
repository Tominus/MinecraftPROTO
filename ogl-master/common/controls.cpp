// Include GLFW

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 position = glm::vec3( 8, 260, 8 );

const glm::mat4& getViewMatrix()
{
	return ViewMatrix;
}
const glm::mat4& getProjectionMatrix()
{
	return ProjectionMatrix;
}
const glm::vec3& getPosition()
{
	return position;
}
const float& getPositionHeight()
{
	return position.y;
}

// Initial horizontal angle : toward -Z
//float horizontalAngle = 0.8f;
float horizontalAngle = -2.5f;

// Initial vertical angle : none
//float verticalAngle = -1.5f;
float verticalAngle = -0.5f;

// Initial Field of View
float initialFoV = 45.70f;

float speed = 12.0f; // 3 units / second
float mouseSpeed = 0.005f;

bool autoMove = false;



void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	/*if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		verticalAngle += deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		verticalAngle -= deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		horizontalAngle -= deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		horizontalAngle += deltaTime * speed;
	}*/


	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS)
		position += direction * deltaTime * speed;

	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
		position -= direction * deltaTime * speed;

	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS)
		position += right * deltaTime * speed;

	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
		position -= right * deltaTime * speed;


	if (glfwGetKey( window, GLFW_KEY_KP_ADD ) == GLFW_PRESS)
		speed += 1.f;
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		speed -= 1.f;
		speed = speed < 1.f ? 1.f : speed;
	}

	// Go up
	if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS)
	{
		position += glm::vec3(0, 1.0f, 0) * deltaTime * speed;
	}
	// Go down
	if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)
	{
		position -= glm::vec3(0, 1.0f, 0) * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		autoMove = !autoMove;
	}
	if (autoMove)
		position += glm::vec3(1, 0, 0) * deltaTime * speed;

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.01f, 10000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}