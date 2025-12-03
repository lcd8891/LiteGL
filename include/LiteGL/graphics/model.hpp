#pragma once
#include <string>
#include <filesystem>
#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/system/types.hpp>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace LiteAPI{
    class VertexArray;
    class Texture;
    class DynamicMesh;
    class Model{
        public:
        struct Bone{
            std::string name;
            glm::mat4 transform;
            glm::mat4 inverseBindMatrix;
            int parentIndex = -1;
            std::vector<int> children;
        };
        struct AnimationKeyFrame{
            float timestamp;
            glm::vec3 translation;  
            glm::quat rotation;
            glm::vec3 scale;
        };
        struct AnimationChannel{
            std::string boneName;
            int boneIndex = -1;
            std::vector<AnimationKeyFrame> keyFrames;

            glm::vec3 interpolatePosition(float animationTime) const;
            glm::quat interpolateRotation(float animationTime) const;
            glm::vec3 interpolateScale(float animationTime) const;
            AnimationKeyFrame interpolate(float animationTime) const;
        };
        struct ModelAnimation{
            std::string name;
            float duration = 0.f;
            float tps = 1.f;
            std::vector<AnimationChannel> channels;
            bool playing = false;
            float currentTime = 0;
        };
        struct Vertex{
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;
            glm::ivec4 boneIndices = {-1,-1,-1,-1};
            glm::vec4 boneWeight = {0,0,0,0};
        };

        private:
        color4 color = {255,255,255,255};
        LiteAPI::VertexArray *vertarr = nullptr;
        LiteAPI::Texture *texture;

        std::unordered_map<std::string,ModelAnimation> animations;  
        std::unordered_map<std::string,int> boneMapping;
        std::vector<Bone> bones;
        std::vector<Vertex> vertices;
        std::vector<Vertex> original_vertices;
        std::vector<glm::mat4> boneTransforms;
        std::string currentAnimation;
        bool isPlaying = 0, modified = false;
        
        void loadGLTF(const std::filesystem::path& path);
        void processNode(void *model_ptr,void *node_ptr,void *matrix_ptr,const glm::mat4& parentTransform);
        void processMesh(void *model_ptr, void *mesh_ptr);
        void loadAnimations(void* gltfModel);
        void loadSkins(void* gltfModel);

        void updateBoneTransforms(float animationTime);
        void applyAnimationToVertices();
        void refreshVertexArray();
        int findBone(const std::string& name);
        int addBone(const std::string& name, const glm::mat4& inverseBindMatrix = glm::mat4(1.0f), const glm::mat4& localTransform = glm::mat4(1.0f));
        void playAnimation(const std::string& name);
        void stopAnimation();
        void addBoneHierarchy(void* modelptr, int nodeIndex, int parentIndex);

        void updateAnimation(float deltaTime);
        AnimationKeyFrame getInterpolatedKeyFrame(const AnimationChannel& channel, float animationTime) const;
        void computeBoneTransform(const ModelAnimation& animation, int boneIndex, float animationTime, const glm::mat4& parentTransform);

        public:
        Model(std::filesystem::path path);
        ~Model();
        
        const VertexArray* getVertices();
        Mesh* createStaticMesh();
        Texture* getModelTexture();
        void updateModelMesh(DynamicMesh* mesh);

        void setAnimation(std::string name);
        void update(float deltaTime);
        void refreshDynamicMesh(DynamicMesh *mesh);

        const std::vector<Bone>& getBones() const { return bones; }
        const std::unordered_map<std::string, ModelAnimation>& getAnimations() const { return animations; }
    };
}