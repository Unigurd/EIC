#include "Camera.hpp"


Camera::Camera(float fov, int height, int width, float zNear, float zFar) {
    float aspect = (float)width / (float)height;
    projection = glm::perspective(fov, aspect, zNear, zFar); // Not changed again since it is constant.
    translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 6.0f));
    rotationX = 1.0f; rotationY = 1.0f;
    updateViewProj();
    wireframe = false;
    backfaceCulling = true;
    glEnable(GL_CULL_FACE);
}


void Camera::updateViewProj(){
    glm::mat4 rotMatX = glm::rotate(glm::mat4(1.0f), glm::radians(360 * rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotMatY = glm::rotate(glm::mat4(1.0f), glm::radians(360 * rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::inverse(rotMatY * rotMatX * translation);
    viewProj = projection * view;
}

void Camera::translate(glm::vec3 trans) {
    translation = glm::translate(translation, trans);
    updateViewProj();
}

void Camera::rotate(glm::vec3 rot) {
    rotationX += rot[0]; rotationY += rot[1];
    // Make sure vertical rotation doesn't exceed +- 90 degrees
    float gap = 0.001f;
    if (rotationX >= 1.25f) {
        rotationX = 1.25f - gap;
    }

    if (rotationX <= 0.75f) {
        rotationX = 0.75f + gap;
    }

    updateViewProj();
 }

// Returns the ViewProjMatrix for use in the main loop
glm::mat4 Camera::ViewProjMatrix(){
    return viewProj;
}

void Camera::toggleBackfaceCulling() {
    if (backfaceCulling = backfaceCulling != true) { glEnable(GL_CULL_FACE); }
    else { glDisable(GL_CULL_FACE); }
}

void Camera::toggleWireframe() {
    if (wireframe = wireframe != 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
}


