#pragma once
#include <glm/matrix.hpp>
#include <LiteGL/system/types.hpp>

namespace LiteAPI{
    class Camera{
        private:
        virtual void updateVectors() = 0;
        public:
        Camera(){}
        glm::mat4 rotation_mat;
        glm::vec3 position;
        float nearCull = 0.1;
        float farCull = 1500;

        void resetRotationMatrix();
        void setCull(float near,float far);
        void setRotation(rotation _rot);
        void rotateTo(rotation _rotatinon);
        rotation getRotation();
        virtual glm::mat4 getView() = 0;
        virtual glm::mat4 getProjection() = 0;
    };
    class PerspectiveCamera : public Camera{
        private:
        virtual void updateVectors() override;
        float fov;
        public:
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 dir;
        glm::vec3 right;
        
        PerspectiveCamera(vector3<float> _position, float _fov);
        void setFOV(float _fov);
        virtual glm::mat4 getView() override;
        virtual glm::mat4 getProjection() override;
    };

    class OrtographicCamera : public Camera{
        
    };
}