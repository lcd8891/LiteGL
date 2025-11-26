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
#include <stb_image.h>
#include <GL/glew.h>
#include "../buffer/buffer.hpp"

#define FROM_VOID(T,N,P) T *N = static_cast<T*>(P);
#define GET_ACCESSOR_INDEX(N) auto index = findAttribute(primitive,N);

namespace{
    std::optional<std::size_t> findAttribute(const fastgltf::Primitive& primitive, const std::string& name){
        for(const auto& attr : primitive.attributes){
            if(std::string(attr.name) == name){
                return attr.accessorIndex;
            }
        }
        return std::nullopt;
    }
    LiteAPI::Texture* _load_from_file(const std::string _path){
        unsigned int textureID;
        glGenTextures(1,&textureID);
        int width,heigth,component;

        unsigned char* imagedata = stbi_load(_path.c_str(),&width,&heigth,&component,0);
        if(imagedata){
            int format;
            switch(component){
                case 1:
                format = GL_RED;
                break;
                case 3:
                format = GL_RGB;
                break;
                case 4:
                format = GL_RGBA;
            }
            glBindTexture(GL_TEXTURE_2D,textureID);
            glTexImage2D(GL_TEXTURE_2D,0,format,width,heigth,0,format,GL_UNSIGNED_BYTE,imagedata);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            stbi_image_free(imagedata);
            return new LiteAPI::Texture(textureID);
        }else{
            throw std::runtime_error("Couldn't load texture: "+_path);
        }
    }

    const int MODEL_ATTRIBUTES[] = {3, 2, 4, 0};
}
//Attribute: position uv color

namespace LiteAPI{
    glm::vec3 Model::AnimationChannel::interpolatePosition(float animationTime) const {
        
    }

    glm::quat Model::AnimationChannel::interpolateRotation(float animationTime) const {
        
    }

    glm::vec3 Model::AnimationChannel::interpolateScale(float animationTime) const {
        
    }


    Model::Model(std::filesystem::path path){
        if(path.extension() == ".gltf" || path.extension() == ".glb"){
            std::filesystem::path respath = "./res/models/";
            respath/=path;
            loadGLTF(respath);
            texture = _load_from_file("./res/models/"+path.replace_extension(".png").string());
            PRIV::texture_buffer_set_mem(texture,"model:"+path.string());
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
        fastgltf::iterateSceneNodes(model,0,fastgltf::math::fmat4x4(),
            [&](fastgltf::Node &node,fastgltf::math::fmat4x4 matrix){
                processNode(&model,&node,&matrix,glm::mat4(1));
            }
        );
        refreshVertexArray();
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
        if(vertarr)delete vertarr;
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
    void Model::loadAnimations(void* gltfModel){

    }

    void Model::updateBoneTransforms(float animationTime){
        
    }

    void Model::applyAnimationToVertices(){

    }

    int Model::findBone(const std::string& name){
        
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
        if(mesh && !boneTransform.empty()){
            applyAnimationToVertices();
        }
    }

    void Model::getBone(std::string _id){
        
    }

    void Model::setAnimation(std::string name){

    }
}