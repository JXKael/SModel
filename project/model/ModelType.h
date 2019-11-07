#ifndef MODEL_TYPE_H
#define MODEL_TYPE_H

#include <vector>

#include <glm/glm.hpp>
#include "Eigen/Eigen"

namespace smodel {

#define BODY "body"
#define RIGHT_HAND "right_hand"
#define LEFT_HAND "left_hand"

#define BLOCK_UNDEFINED 32767
#define VEC3_DIM 3
#define MAT4_DIM 4
#define ROOT_NAME "Root"

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

enum DofFreeType {
    kFree = 0,
    kUnderCtrl = 1
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

    Bone *bone;

    Bone *attached_bone;
    vec3 offset; // offset to it's attached Bones's center

    Sphere() : id(-1), name(""), radius(0.0f), position(vec3::Zero()), type(kCenter) {
        bone = nullptr;
        attached_bone = nullptr;
        offset = vec3::Zero();
    }
    Sphere(int _id, std::string _name, float _radius, vec3 _position, SphereType _type)
      : id(_id), name(_name), radius(_radius), position(_position), type(_type) {
        bone = nullptr;
        attached_bone = nullptr;
        offset = vec3::Zero();
    }
    const bool isAttachment() const { return type == SphereType::kAttachment; }
};

// 骨骼
struct Bone {
    int id;
    std::string name;
    int center_id;
    int parent_id;
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

    Bone() : id(-1), name(""), center_id(-1), parent_id(-1) {
        center = nullptr;
        parent = nullptr;
        length = 0.0f;
        radius_parent = 0.0f;
        radius_child = 0.0f;
        init_local = mat4::Identity();
        local = mat4::Identity();
        global = mat4::Identity();
    }
    Bone(int _id, std::string _name, int _center_id, int _parent_id, std::vector<int> _attachment_ids, mat4 _init_local)
        : id(_id), name(_name), center_id(_center_id), parent_id(_parent_id), attachment_ids(_attachment_ids), init_local(_init_local)
    {
        center = nullptr;
        parent = nullptr;
        length = 0.0f;
        radius_parent = 0.0f;
        radius_child = 0.0f;
        local = mat4::Identity();
        global = mat4::Identity();
    }
    const bool isModelRoot() const { return parent_id == -1 && name == ROOT_NAME; }
    const bool hasParent() const { return parent_id >= 0; }
    const bool hasChildren() const { return !children.empty(); }
    const bool hasAttachments() const { return !attachment_ids.empty(); }
};

// 自由度
struct Dof {
    int id;
    std::string name;
    DofType type;
    DofFreeType free_type;
    AxisType axis_type;
    int bone_id;
    float min;
    float max;
    float init_val;

    vec3 axis;
    Bone *bone;

    Dof() : id(-1), name(""), type(kRotation), free_type(kFree), axis_type(kX), min(-1.0f), max(1.0f), init_val(0.0f), bone_id(-1)
    {
        axis = ParseToVec(axis_type);
        bone = nullptr;
    }
    Dof(int _id, std::string _name, DofType _type, DofFreeType _free_type, AxisType _axis_type, float _min, float _max, float _init_val, int _bone_id)
      : id(_id),
        name(_name),
        type(_type),
        free_type(_free_type),
        axis_type(_axis_type),
        min(_min), max(_max), init_val(_init_val),
        bone_id(_bone_id)
    {
        axis = ParseToVec(axis_type);
        bone = nullptr;
    }
    vec3 ParseToVec(AxisType axis_type) {
        switch (axis_type) {
        case kX: return vec3::UnitX();
        case kY: return vec3::UnitY();
        case kZ: return vec3::UnitZ();
        default: return vec3::Zero();
        }
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
