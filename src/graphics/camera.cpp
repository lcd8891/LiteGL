#include <LiteGL/graphics/camera.hpp>
#include <glm/ext.hpp>
#include "../window/window.hpp"

namespace LiteAPI{
    PerspectiveCamera::PerspectiveCamera(vector3<float> _position, float _fov):fov(_fov){
        this->position = glm::vec3(_position.x,_position.y,_position.z);
        this->updateVectors();
    }
    void PerspectiveCamera::rotate(vector3<float> _rotatinon){
        this->rotation = glm::rotate(this->rotation,_rotatinon.x,glm::vec3(1,0,0));
        this->rotation = glm::rotate(this->rotation,_rotatinon.y,glm::vec3(0,1,0));
        this->rotation = glm::rotate(this->rotation,_rotatinon.z,glm::vec3(0,0,1));
        this->updateVectors();
    }
    void PerspectiveCamera::updateVectors(){
        front = glm::vec3(rotation * glm::vec4(0,0,-1,1));
        right = glm::vec3(rotation * glm::vec4(1,0,0,1));
        up = glm::vec3(rotation * glm::vec4(0,1,0,1));
        dir = glm::vec3(rotation * glm::vec4(0,0,-1,1));
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
        return glm::perspective(fov, (float)PRIV_Window::window_size.x / (float)PRIV_Window::window_size.y, 0.1f, 1500.0f);
    }
}