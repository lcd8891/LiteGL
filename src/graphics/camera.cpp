#include <LiteGL/graphics/camera.hpp>
#include <glm/ext.hpp>
#include "../window/window.hpp"

namespace LiteAPI{
    void Camera::setCull(float near,float far){
        nearCull=near;farCull=far;
    }
    void Camera::setRotation(rotation _rot){
        this->resetRotationMatrix();
        this->rotateTo(_rot);
    }
    rotation Camera::getRotation(){
        glm::vec3 euler = glm::eulerAngles(glm::quat_cast(rotation_mat));
        return {euler.x, euler.y, euler.z};
    }
    PerspectiveCamera::PerspectiveCamera(vector3<float> _position, float _fov):fov(glm::radians(_fov)){
        this->position = glm::vec3(_position.x,_position.y,_position.z);
        this->updateVectors();
    }
    void PerspectiveCamera::setFOV(float _fov){
        fov = glm::radians(_fov);
    }
    void Camera::rotateTo(rotation _rotatinon){
        this->resetRotationMatrix();
        this->rotation_mat = glm::rotate(this->rotation_mat,_rotatinon.p,glm::vec3(0,1,0));
        this->rotation_mat = glm::rotate(this->rotation_mat,_rotatinon.y,glm::vec3(1,0,0));
        this->rotation_mat = glm::rotate(this->rotation_mat,_rotatinon.r,glm::vec3(0,0,1));
        this->updateVectors();
    }
    void Camera::resetRotationMatrix(){
        rotation_mat = glm::mat4(1);
    }
    void PerspectiveCamera::updateVectors(){
        front = glm::vec3(rotation_mat * glm::vec4(0,0,-1,1));
        right = glm::vec3(rotation_mat * glm::vec4(1,0,0,1));
        up = glm::vec3(rotation_mat * glm::vec4(0,1,0,1));
        dir = glm::vec3(rotation_mat * glm::vec4(0,0,-1,1));
        dir.y = 0;
        float len = length(dir);
        if (len > 0.0f){
            dir.x /= len;
            dir.z /= len;
        }
    }
    glm::mat4 PerspectiveCamera::getView(){
	    return glm::lookAt(position, position+front, up);
    };
    glm::mat4 PerspectiveCamera::getProjection(){
        return glm::perspective(fov, (float)PRIV_Window::window_size.x / (float)PRIV_Window::window_size.y, nearCull, farCull);
    }
}