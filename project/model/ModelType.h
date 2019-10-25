#ifndef MODEL_TYPE_H
#define MODEL_TYPE_H

#include <vector>

#include <glm/glm.hpp>
#include "Eigen/Eigen"

namespace smodel {

#define BLOCK_UNDEFINED 32767
#define VEC3_DIM 3
#define MAT4_DIM 4

typedef Eigen::Vector3f vec3;
typedef Eigen::Matrix4f mat4;

struct Sphere;
struct Bone;
struct Dof;
struct Tangent;

typedef struct Sphere Sphere;
typedef struct Sphere Center;
typedef struct Bone Bone;
typedef struct Dof Dof;
typedef struct Tangent Tangent;

enum SphereType {
    kCenter = 0,
    kAttachment = 1
};

enum DofType {
    kRotation = 0,
    kTranslation = 1
};

enum AxisType {
    kX = 0,
    kY = 1,
    kZ = 2
};

// 中心球
struct Sphere {
    int id;
    std::string name;
    float radius;
    vec3 position;
    SphereType type;
    int attached_bone_id;

    Bone *attached_bone;
    vec3 offset; // offset to it's attached Bones's center

    Sphere() : id(-1), name(""), radius(0.0f), position(vec3::Zero()), type(kCenter), attached_bone_id(-1)
    {
        attached_bone = nullptr;
        offset = vec3::Zero();
    }
    Sphere(int _id, std::string _name, float _radius, vec3 _position, SphereType _type, int _attached_bone_id)
      : id(_id),
        name(_name),
        radius(_radius),
        position(_position),
        type(_type),
        attached_bone_id(_attached_bone_id)
    {
        attached_bone = nullptr;
        offset = vec3::Zero();
    }
    bool isAttachment() { return type == SphereType::kAttachment && attached_bone_id >= 0; }
};

// 骨骼
struct Bone {
    int id;
    std::string name;
    int center_id;
    int parent_id;
    std::vector<int> children_ids;
    std::vector<int> attachment_ids;
    
    Center *center; // 该骨骼的root sphere
    Bone *parent;
    std::vector<Bone *> children;
    std::vector<Sphere *> attachments; // 连接在该骨骼上的其他sphere的中心，在骨骼变化过程中，其相对位置与本shpere中心保持不变

    float length;
    float radius_parent;
    float radius_child;

    mat4 init_local;
    mat4 local; // 相对于parent_id的坐标，由init_local赋值，随后根据theta变化
    mat4 global; // global矩阵

    Bone() : id(-1), name(""), center_id(-1), parent_id(-1)
    {
        center = nullptr;
        parent = nullptr;
        length = 0.0f;
        radius_parent = 0.0f;
        radius_child = 0.0f;
        init_local = mat4::Identity();
        local = mat4::Identity();
        global = mat4::Identity();
    }
    Bone(int _id, std::string _name, int _center_id, int _parent_id,
         std::vector<int> _children_ids, std::vector<int> _attachment_ids,
         mat4 _init_local)
        : id(_id),
        name(_name),
        center_id(_center_id),
        parent_id(_parent_id),
        children_ids(_children_ids),
        attachment_ids(_attachment_ids),
        init_local(_init_local)
    {
        center = nullptr;
        parent = nullptr;
        length = 0.0f;
        radius_parent = 0.0f;
        radius_child = 0.0f;
        local = mat4::Identity();
        global = mat4::Identity();
    }
    bool isModelRoot() { return parent_id == -1; }
    bool hasParent() { return parent_id >= 0; }
    bool hasChildren() { return !children_ids.empty(); }
    bool hasAttachments() { return !attachment_ids.empty(); }
};

// 自由度
struct Dof {
    int id;
    std::string name;
    DofType type;
    AxisType axis_type;
    int bone_id;
    float min;
    float max;
    float init_val;

    vec3 axis;
    Bone *bone;

    Dof() : id(-1), name(""), type(kRotation), axis_type(kX), bone_id(-1), min(-1.0f), max(1.0f), init_val(0.0f)
    {
        switch (axis_type) {
        case kX:
            axis = vec3::UnitX(); break;
        case kY:
            axis = vec3::UnitY(); break;
        case kZ:
            axis = vec3::UnitZ(); break;
        default:
            axis = vec3::Zero(); break;
        }
        bone = nullptr;
    }
    Dof(int _id, std::string _name, DofType _type, AxisType _axis_type, int _bone_id, float _min, float _max, float _init_val)
      : id(_id),
        name(_name),
        type(_type),
        axis_type(_axis_type),
        bone_id(_bone_id),
        min(_min), max(_max), init_val(_init_val)
    {
        switch (axis_type) {
        case kX:
            axis = vec3::UnitX(); break;
        case kY:
            axis = vec3::UnitY(); break;
        case kZ:
            axis = vec3::UnitZ(); break;
        default:
            axis = vec3::Zero(); break;
        }
        bone = nullptr;
    }
};

struct Tangent {
    glm::vec3 v1; glm::vec3 v2; glm::vec3 v3; glm::vec3 n;
    glm::vec3 u1; glm::vec3 u2; glm::vec3 u3; glm::vec3 m;

    Tangent() {
        v1 = glm::vec3(0, 0, 0); v2 = glm::vec3(0, 0, 0); v3 = glm::vec3(0, 0, 0);
        u1 = glm::vec3(0, 0, 0); u2 = glm::vec3(0, 0, 0); u3 = glm::vec3(0, 0, 0);
    }
};

typedef std::vector<Sphere> Centers;
typedef Eigen::Vector3i Block;
typedef std::vector<Block> Blocks;
typedef std::vector<Tangent> TangentPoints;

typedef std::vector<Bone> Bones;
typedef std::vector<float> Thetas;
typedef std::vector<Dof> Dofs;

} // namespace smodel;

#endif // MODEL_TYPE_H
