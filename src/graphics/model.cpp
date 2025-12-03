#include <LiteGL/graphics/model.hpp>
#include <LiteGL/graphics/vertexarray.hpp>
#include <LiteGL/graphics/mesh.hpp>
#include <LiteGL/graphics/texture.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../system/priv_logger.hpp"
#include "../assets/buffer.hpp"
#include "graphics_tools.hpp"

#define FROM_VOID(T,N,P) T *N = static_cast<T*>(P);
#define GET_ACCESSOR_INDEX(N) auto index = findAttribute(primitive,N);

namespace{
    std::string make_str(const std::pmr::string &str){
        return std::string(str.data(),str.size());
    }
    std::optional<std::size_t> findAttribute(const fastgltf::Primitive& primitive, const std::string& name){
        for(const auto& attr : primitive.attributes){
            if(make_str(attr.name) == name){
                return attr.accessorIndex;
            }
        }
        return std::nullopt;
    }
    const int MODEL_ATTRIBUTES[] = {3, 2, 4, 0};
    glm::mat4 getNodeTransform(const fastgltf::Node& node) {
        if (std::holds_alternative<fastgltf::math::fmat4x4>(node.transform)) {
            const auto& mat = std::get<fastgltf::math::fmat4x4>(node.transform);
            return glm::make_mat4(mat.data());
        } else {
            const auto& trs = std::get<fastgltf::TRS>(node.transform);
            glm::vec3 t(trs.translation[0], trs.translation[1], trs.translation[2]);
            glm::quat r(trs.rotation[3], trs.rotation[0], trs.rotation[1], trs.rotation[2]);
            glm::vec3 s(trs.scale[0], trs.scale[1], trs.scale[2]);
            return glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
        }
    }

}

namespace LiteAPI{
    glm::vec3 Model::AnimationChannel::interpolatePosition(float animationTime) const {
        if (keyFrames.size() == 1) return keyFrames[0].translation;
        for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (animationTime < keyFrames[i + 1].timestamp) {
                float delta = keyFrames[i + 1].timestamp - keyFrames[i].timestamp;
                float factor = (animationTime - keyFrames[i].timestamp) / delta;
                return glm::mix(keyFrames[i].translation, keyFrames[i + 1].translation, factor);
            }
        }
        return keyFrames.back().translation;
    }

    glm::quat Model::AnimationChannel::interpolateRotation(float animationTime) const {
        if (keyFrames.size() == 1) return keyFrames[0].rotation;
        for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (animationTime < keyFrames[i + 1].timestamp) {
                float delta = keyFrames[i + 1].timestamp - keyFrames[i].timestamp;
                float factor = (animationTime - keyFrames[i].timestamp) / delta;
                return glm::slerp(keyFrames[i].rotation, keyFrames[i + 1].rotation, factor);
            }
        }
        return keyFrames.back().rotation;
    }

    glm::vec3 Model::AnimationChannel::interpolateScale(float animationTime) const {
        if (keyFrames.size() == 1) return keyFrames[0].scale;
        for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (animationTime < keyFrames[i + 1].timestamp) {
                float delta = keyFrames[i + 1].timestamp - keyFrames[i].timestamp;
                float factor = (animationTime - keyFrames[i].timestamp) / delta;
                return glm::mix(keyFrames[i].scale, keyFrames[i + 1].scale, factor);
            }
        }
        return keyFrames.back().scale;
    }
    Model::AnimationKeyFrame Model::AnimationChannel::interpolate(float animationTime) const {
        AnimationKeyFrame result;
        result.timestamp = animationTime;
        result.translation = interpolatePosition(animationTime);
        result.rotation = interpolateRotation(animationTime);
        result.scale = interpolateScale(animationTime);
        return result;
    }

    Model::Model(std::filesystem::path path){
        if(path.extension() == ".gltf" || path.extension() == ".glb"){
            std::filesystem::path respath = "./res/models/";
            respath/=path;
            loadGLTF(respath);
            texture = _load_texture_from_file("./res/models/"+path.replace_extension(".png").string());
            PRIV::texture_buffer_set_mem(texture,"__model:"+path.string());
        }else{
            system_logger->error() << "ModelLoader: model is not GLTF type";
        }
    }

    void Model::loadGLTF(const std::filesystem::path& path){
        auto buffer = fastgltf::GltfDataBuffer::FromPath(path);
        if(buffer.error() != fastgltf::Error::None){
            system_logger->error() << "ModelLoader: model load error: " << fastgltf::getErrorMessage(buffer.error());
            return;
        }
        fastgltf::Asset model;
        fastgltf::Parser parser;
        auto loaded = parser.loadGltf(buffer.get(),path.parent_path());
        if(loaded.error() != fastgltf::Error::None){
            system_logger->error() << "ModelLoader: model load error: " << fastgltf::getErrorMessage(loaded.error());
            return;
        }
        model = std::move(loaded.get());
        loadSkins(&model);
        fastgltf::iterateSceneNodes(model,0,fastgltf::math::fmat4x4(),
            [&](fastgltf::Node &node,fastgltf::math::fmat4x4 matrix){
                processNode(&model,&node,&matrix,glm::mat4(1));
            }
        );
        loadAnimations(&model);
        boneTransforms.resize(bones.size(), glm::mat4(1.0f));
        refreshVertexArray();
        original_vertices = vertices;
    }
    void Model::loadSkins(void* gltfModel) {
        FROM_VOID(fastgltf::Asset, model, gltfModel);
        
        for (const auto& skin : model->skins) {
            if (skin.joints.empty()) continue;
            std::vector<glm::mat4> inverseBindMatrices;
            if (skin.inverseBindMatrices.has_value()) {
                const auto& accessor = model->accessors[skin.inverseBindMatrices.value()];
                fastgltf::iterateAccessor<glm::mat4>(*model, accessor, [&](glm::mat4 matrix) {
                    inverseBindMatrices.push_back(matrix);
                });
            }
            for (size_t i = 0; i < skin.joints.size(); ++i) {
                auto nodeIndex = skin.joints[i];
                if (nodeIndex >= model->nodes.size()) continue;

                const auto& node = model->nodes[nodeIndex];
                std::string boneName = !node.name.empty() ? make_str(node.name)
                                                        : "bone_" + std::to_string(nodeIndex);

                glm::mat4 ibm = (i < inverseBindMatrices.size()) ? inverseBindMatrices[i] : glm::mat4(1.0f);
                int boneIndex = addBone(boneName, ibm, getNodeTransform(node));
                int parentIndex = -1;
                for (const auto& potentialParent : skin.joints) {
                    const auto& potentialParentNode = model->nodes[potentialParent];
                    for (auto childIdx : potentialParentNode.children) {
                        if (childIdx == nodeIndex) {
                            std::string parentName = !model->nodes[potentialParent].name.empty() 
                                ? make_str(model->nodes[potentialParent].name)
                                : "bone_" + std::to_string(potentialParent);
                            
                            auto parentIt = boneMapping.find(parentName);
                            if (parentIt != boneMapping.end()) {
                                parentIndex = parentIt->second;
                                break;
                            }
                        }
                    }
                    if (parentIndex != -1) break;
                }
                bones[boneIndex].parentIndex = parentIndex;
                if (parentIndex != -1) {
                    bones[parentIndex].children.push_back(boneIndex);
                }
            }
        }
    }
    int Model::addBone(const std::string& name, const glm::mat4& inverseBindMatrix, const glm::mat4& localTransform) {
        auto it = boneMapping.find(name);
        if (it != boneMapping.end()) {
            return it->second;
        }
        Bone bone;
        bone.name = name;
        bone.inverseBindMatrix = inverseBindMatrix;
        bone.transform = localTransform;
        int index = bones.size();
        bones.push_back(bone);
        boneMapping[name] = index;
        return index;
    }
    void Model::processNode(void *model_ptr,void *node_ptr,void *matrix_ptr,const glm::mat4& parentTransform) {
        FROM_VOID(fastgltf::Node,node,node_ptr)
        FROM_VOID(fastgltf::math::fmat4x4,matrix,matrix_ptr)
        FROM_VOID(fastgltf::Asset,model,model_ptr)

        glm::mat4 local_transform = glm::make_mat4(matrix->data());
        glm::mat4 transform = parentTransform * local_transform;
        if(node->meshIndex.has_value()){    
            fastgltf::Mesh &mesh = model->meshes[*node->meshIndex];
            processMesh(model_ptr,&mesh);
        }
    }

    void Model::processMesh(void *model_ptr, void *mesh_ptr){
        FROM_VOID(fastgltf::Asset,model,model_ptr)
        FROM_VOID(fastgltf::Mesh,mesh,mesh_ptr)
        for(fastgltf::Primitive& primitive : mesh->primitives){
            
            auto positionIndex = findAttribute(primitive,"POSITION");
            auto normalIndex = findAttribute(primitive,"NORMAL");
            auto texCoordIndex = findAttribute(primitive,"TEXCOORD_0");
            auto jointsIndex = findAttribute(primitive,"JOINTS_0");
            auto weightsIndex = findAttribute(primitive,"WEIGHTS_0");
            if(!positionIndex.has_value()){
                system_logger->error() << "ModelLoader: Mesh missing POSITION attribute";
                continue;
            }
            auto& positionAccessor = model->accessors[positionIndex.value()];
            if(!positionAccessor.bufferViewIndex.has_value()){
                system_logger->error() << "ModelLoader: Position accessor has no buffer view";
                continue;
            }
            std::vector<glm::vec3> positions;
            fastgltf::iterateAccessor<glm::vec3>(*model, positionAccessor, [&](glm::vec3 value) {
                positions.push_back(value);
            });
            std::vector<glm::vec3> normals;
            if(normalIndex.has_value()) {
                auto& normalAccessor = model->accessors[*normalIndex];
                fastgltf::iterateAccessor<glm::vec3>(*model, normalAccessor, [&](glm::vec3 value) {
                    normals.push_back(value);
                });
            }
            std::vector<glm::vec2> texCoords;
            if(texCoordIndex.has_value()) {
                auto& texCoordAccessor = model->accessors[*texCoordIndex];
                fastgltf::iterateAccessor<glm::vec2>(*model, texCoordAccessor, [&](glm::vec2 value) {
                    texCoords.push_back(value);
                });
            }
            std::vector<glm::u16vec4> joints;
            std::vector<glm::vec4> weights;
            if(jointsIndex.has_value() && weightsIndex.has_value()) {
                auto& jointsAccessor = model->accessors[*jointsIndex];
                auto& weightsAccessor = model->accessors[*weightsIndex];
                
                fastgltf::iterateAccessor<glm::u16vec4>(*model, jointsAccessor, [&](glm::u16vec4 value) {
                    joints.push_back(value);
                });
                
                fastgltf::iterateAccessor<glm::vec4>(*model, weightsAccessor, [&](glm::vec4 value) {
                    weights.push_back(value);
                });
            }
            std::vector<uint32_t> indices;
            if(primitive.indicesAccessor.has_value()) {
                auto& indicesAccessor = model->accessors[*primitive.indicesAccessor];
                
                if(indicesAccessor.componentType == fastgltf::ComponentType::UnsignedInt) {
                    fastgltf::iterateAccessor<uint32_t>(*model, indicesAccessor, [&](uint32_t value) {
                        indices.push_back(value);
                    });
                } else if(indicesAccessor.componentType == fastgltf::ComponentType::UnsignedShort) {
                    fastgltf::iterateAccessor<uint16_t>(*model, indicesAccessor, [&](uint16_t value) {
                        indices.push_back(static_cast<uint32_t>(value));
                    });
                } else if(indicesAccessor.componentType == fastgltf::ComponentType::UnsignedByte) {
                    fastgltf::iterateAccessor<uint8_t>(*model, indicesAccessor, [&](uint8_t value) {
                        indices.push_back(static_cast<uint32_t>(value));
                    });
                } else {
                    system_logger->warn() << "ModelLoader: Unsupported indices component type";
                }
            }
            if(!indices.empty()){
                for(size_t i = 0; i < indices.size(); ++i){
                    uint32_t index = indices[i];
                    Vertex vertex;
                    if(index < positions.size()){
                        vertex.position = positions[index];
                    } else {
                        system_logger->error() << "Index out of bounds for positions";
                        continue;
                    }
                    if(!normals.empty() && index < normals.size()){
                        vertex.normal = normals[index];
                    } else {
                        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                    if(!texCoords.empty() && index < texCoords.size()){
                        vertex.texCoord = texCoords[index];
                    } else {
                        vertex.texCoord = glm::vec2(0.0f, 0.0f);
                    }
                    if(!joints.empty() && !weights.empty() && 
                    index < joints.size() && index < weights.size()){
                        vertex.boneIndices = joints[index];
                        vertex.boneWeight = weights[index];
                    }
                    vertices.push_back(vertex);
                }
            }else{
                size_t vertexCount = positionAccessor.count;
                for(size_t i = 0; i < vertexCount; ++i){
                    Vertex vertex;
                    vertex.position = positions[i];
                    if(i < normals.size()){
                        vertex.normal = normals[i];
                    } else {
                        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    }
                    if(i < texCoords.size()){
                        vertex.texCoord = texCoords[i];
                    } else {
                        vertex.texCoord = glm::vec2(0.0f, 0.0f);
                    }
                    if(i < joints.size() && i < weights.size()){
                        vertex.boneIndices = joints[i];
                        vertex.boneWeight = weights[i];
                    }
                    vertices.push_back(vertex);
                }
            }
        }
    }
    void Model::refreshVertexArray(){
        if(vertarr){
            delete vertarr;
            vertarr = nullptr;
        }
        vertarr = new LiteAPI::VertexArray(9);
        std::vector<float> data;
        data.reserve(vertices.size() * 9);
        for(const auto& vertex : vertices){
            data.push_back(vertex.position.x);
            data.push_back(vertex.position.y);
            data.push_back(vertex.position.z);
            
            data.push_back(vertex.texCoord.x);
            data.push_back(vertex.texCoord.y);
            
            data.push_back((float)color.r / 255);
            data.push_back((float)color.g / 255);
            data.push_back((float)color.b / 255);
            data.push_back((float)color.a / 255);
        }
        vertarr->insert(data.data(), vertices.size());
        modified = false;
    }
    void Model::addBoneHierarchy(void* modelptr, int nodeIndex, int parentIndex) {
        FROM_VOID(fastgltf::Asset, model, modelptr)
        const auto& node = model->nodes[nodeIndex];
        std::string boneName = !node.name.empty() ? make_str(node.name)
                                                : "bone_" + std::to_string(nodeIndex);
        int boneIndex = findBone(boneName);
        if (boneIndex == -1) {
            boneIndex = addBone(boneName, glm::mat4(1.0f), getNodeTransform(node));
        } else {
            bones[boneIndex].transform = getNodeTransform(node);
        }
        bones[boneIndex].parentIndex = parentIndex;
        if (parentIndex >= 0) {
            bones[parentIndex].children.push_back(boneIndex);
        }
        for (auto childNodeIndex : node.children) {
            addBoneHierarchy(model, childNodeIndex, boneIndex);
        }
    }
    void Model::loadAnimations(void* gltfModel){
        FROM_VOID(fastgltf::Asset,model,gltfModel);
        for (const auto& fastgltfAnim : model->animations) {
            ModelAnimation animation;
            animation.name = make_str(fastgltfAnim.name);
            animation.duration = 0.0f;
            std::unordered_map<int, std::vector<std::pair<fastgltf::AnimationPath, const fastgltf::AnimationChannel*>>> channelsByBone;
            for (const auto& channel : fastgltfAnim.channels) {
                if (!channel.nodeIndex.has_value()) continue;
                auto nodeIndex = channel.nodeIndex.value();
                if (nodeIndex >= model->nodes.size()) continue;
                const auto& node = model->nodes[nodeIndex];
                std::string boneName;
                if(!node.name.empty()){
                    boneName = make_str(node.name);
                }else{
                    boneName = "bone_" + std::to_string(nodeIndex);
                }
                int boneIndex = findBone(boneName);
                if (boneIndex == -1) {
                    boneIndex = addBone(boneName);
                }
                const auto& sampler = fastgltfAnim.samplers[channel.samplerIndex];
                std::vector<float> timestamps;
                const auto& inputAccessor = model->accessors[sampler.inputAccessor];
                fastgltf::iterateAccessor<float>(*model, inputAccessor, [&](float time) {
                    timestamps.push_back(time);
                    animation.duration = std::max(animation.duration, time);
                });
                AnimationChannel animChannel;
                animChannel.boneName = boneName;
                animChannel.boneIndex = boneIndex;
                const auto& outputAccessor = model->accessors[sampler.outputAccessor];
                
                switch (channel.path) {
                    case fastgltf::AnimationPath::Translation: {
                        std::vector<glm::vec3> translations;
                        fastgltf::iterateAccessor<glm::vec3>(*model, outputAccessor, [&](glm::vec3 value) {
                            translations.push_back(value);
                        });
                        
                        for (size_t i = 0; i < timestamps.size() && i < translations.size(); ++i) {
                            AnimationKeyFrame keyFrame;
                            keyFrame.timestamp = timestamps[i];
                            keyFrame.translation = translations[i];
                            keyFrame.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                            keyFrame.scale = glm::vec3(1.0f);
                            animChannel.keyFrames.push_back(keyFrame);
                        }
                        break;
                    }
                    case fastgltf::AnimationPath::Rotation: {
                        std::vector<glm::vec4> rotations;
                        fastgltf::iterateAccessor<glm::vec4>(*model, outputAccessor, [&](glm::vec4 value) {
                            rotations.push_back(glm::vec4(value.w, value.x, value.y, value.z));
                        });
                        
                        for (size_t i = 0; i < timestamps.size() && i < rotations.size(); ++i) {
                            AnimationKeyFrame keyFrame;
                            keyFrame.timestamp = timestamps[i];
                            keyFrame.translation = glm::vec3(0.0f);
                            keyFrame.rotation = glm::quat(rotations[i].w, rotations[i].x, rotations[i].y, rotations[i].z);
                            keyFrame.scale = glm::vec3(1.0f);
                            animChannel.keyFrames.push_back(keyFrame);
                        }
                        break;
                    }
                    case fastgltf::AnimationPath::Scale: {
                        std::vector<glm::vec3> scales;
                        fastgltf::iterateAccessor<glm::vec3>(*model, outputAccessor, [&](glm::vec3 value) {
                            scales.push_back(value);
                        });
                        
                        for (size_t i = 0; i < timestamps.size() && i < scales.size(); ++i) {
                            AnimationKeyFrame keyFrame;
                            keyFrame.timestamp = timestamps[i];
                            keyFrame.translation = glm::vec3(0.0f);
                            keyFrame.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                            keyFrame.scale = scales[i];
                            animChannel.keyFrames.push_back(keyFrame);
                        }
                        break;
                    }
                    default:
                        continue;
                }
                
                if (!animChannel.keyFrames.empty()) {
                    animation.channels.push_back(animChannel);
                }
            }
            
            if (!animation.channels.empty()) {
                animations[animation.name] = animation;
            }
        }
    }
    void Model::update(float deltaTime) {
        if (!isPlaying || currentAnimation.empty()) return;
        auto it = animations.find(currentAnimation);
        if (it == animations.end()) {
            stopAnimation();
            return;
        }
        auto& animation = it->second;
        animation.currentTime += deltaTime;
        if (animation.currentTime > animation.duration) {
            animation.currentTime = fmod(animation.currentTime, animation.duration);
        }
        updateBoneTransforms(animation.currentTime);
        applyAnimationToVertices();
        modified = true;
    }


    void Model::updateBoneTransforms(float animationTime){
        if (currentAnimation.empty()) return;

        auto it = animations.find(currentAnimation);
        if (it == animations.end()) return;

        const auto& animation = it->second;
        boneTransforms.resize(bones.size(),glm::mat4(1));
        for (size_t i = 0; i < bones.size(); ++i) {
            if (bones[i].parentIndex == -1) {
                computeBoneTransform(animation, i, animationTime, glm::mat4(1.0f));
            }
        }
    }
    void Model::computeBoneTransform(const ModelAnimation& animation, int boneIndex, float animationTime, const glm::mat4& parentTransform) {
        if (boneIndex < 0 || boneIndex >= static_cast<int>(bones.size())) return;
        auto& bone = bones[boneIndex];
        glm::mat4 localTransform = bone.transform;
        for (const auto& ch : animation.channels) {
            if (ch.boneIndex != boneIndex || ch.keyFrames.empty()) continue;
            auto kf = ch.interpolate(animationTime);
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), kf.translation);
            glm::mat4 rotationMat = glm::mat4_cast(kf.rotation);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), kf.scale);
            
            localTransform = translationMat * rotationMat * scaleMat;
            break;
        }
        glm::mat4 globalTransform = parentTransform * localTransform;
        boneTransforms[boneIndex] = globalTransform * bone.inverseBindMatrix;
        
        for (int childIndex : bone.children) {
            computeBoneTransform(animation, childIndex, animationTime, globalTransform);
        }
    }

    void Model::applyAnimationToVertices(){
        if (boneTransforms.empty()) return;
        
        for (size_t i = 0; i < vertices.size(); ++i) {
            auto& src = original_vertices[i];
            auto& dst = vertices[i];
            glm::vec3 animatedPosition = glm::vec3(0.0f);
            glm::vec3 animatedNormal = glm::vec3(0.0f);

            for (int j = 0; j < 4; ++j) {
                if (src.boneIndices[j] >= 0 && src.boneIndices[j] < boneTransforms.size()) {
                    float weight = src.boneWeight[j];
                    const auto& transform = boneTransforms[src.boneIndices[j]];
                    animatedPosition += weight * (transform * glm::vec4(src.position, 1.0f));
                    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
                    animatedNormal += weight * (normalMatrix * src.normal);
                }
            }
            dst.position = animatedPosition;
            dst.normal = glm::normalize(animatedNormal);
        }
    }

    int Model::findBone(const std::string& name){
        auto it = boneMapping.find(name);
        return it != boneMapping.end() ? it->second : -1;
    }

    void Model::playAnimation(const std::string& name) {
        if (animations.find(name) != animations.end()) {
            currentAnimation = name;
            isPlaying = true;
            animations[name].playing = true;
            animations[name].currentTime = 0.0f;
        } else {
            system_logger->warn() << "Animation not found: " << name;
        }
    }
    void Model::stopAnimation() {
        isPlaying = false;
        if (!currentAnimation.empty()) {
            auto it = animations.find(currentAnimation);
            if (it != animations.end()) {
                it->second.playing = false;
            }
        }
    }
    void Model::setAnimation(std::string name) {
        playAnimation(name);
    }

    Texture* Model::getModelTexture(){
        return texture;
    }

    Model::~Model(){
        if(vertarr){
            delete vertarr;
        }
    }
    
    const VertexArray* Model::getVertices(){
        if(modified){
            refreshVertexArray();
        }
        return vertarr;
    }

    Mesh* Model::createStaticMesh(){
        LiteAPI::Mesh *mehs = new LiteAPI::Mesh(vertarr,MODEL_ATTRIBUTES);
        return mehs;
    }

    void Model::updateModelMesh(DynamicMesh* mesh){
        if(mesh && !boneTransforms.empty()){
            applyAnimationToVertices(); 
        }
    }
    void Model::refreshDynamicMesh(DynamicMesh *mesh){
        mesh->reload(vertarr);
    }
}