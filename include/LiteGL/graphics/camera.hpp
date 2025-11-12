#pragma once
#include <glm/matrix.hpp>
#include <LiteGL/system/types.hpp>

namespace LiteAPI{
    class Camera{
        public:
        glm::mat4 rotation;
        glm::vec3 position;

        virtual glm::mat4 getView();
        virtual glm::mat4 getProjection();
    };
    class PerspectiveCamera : public Camera{
        private:
        void updateVectors();
        public:
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 dir;
        glm::vec3 right;

        float fov;
        float zoom;
        PerspectiveCamera(vector3<float> _position, float _fov);
        void rotate(vector3<float> _rotatinon);

        virtual glm::mat4 getView() override;
        virtual glm::mat4 getProjection() override;
    };

    class IsometricCamera : public Camera{
        
    };
}