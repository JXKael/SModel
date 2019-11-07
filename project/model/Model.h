#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "ModelType.h"

#include "Eigen/Eigen"
#include "Eigen/Geometry"

namespace smodel {

class Model {
public:
    glm::vec3 camera_ray;

    // Bones
    Centers centers;
    Bones bones;
    Blocks blocks;
    TangentPoints tangent_points;

    // DOFs
    Dofs dofs; // 自由度
    Thetas theta; // 自由度当前参数

    bool has_parent;

public:
    explicit Model();
    explicit Model(const std::string &name);
    explicit Model(const smodel::Model &model);
    ~Model();

    void Clear();
    inline void SetHasParent(const bool &has_parent) { this->has_parent = has_parent; }
    Thetas GetInitThetas();
    inline const Thetas &GetTheta() { return theta; }

    void MoveToInit();
    void Move(const Thetas &thetas);
    void Move(const Bone &parent_bone);
    void UpdateCentersPosition();
    // 根据block的连接的center的半径重新排序block组合，半径大的序号小
    void ReindexBlocksByCenterRadii();
    void ComputeTangentPoints();
    void Resize(float uniform_scaling_factor, float width_scaling_factor, float thickness_scaling_factor);

    void PrintModel();
    void WriteModel(std::string data_path, int frame_number = 0);

    inline int CentersSize() { return (int)this->centers.size(); }
    inline int BonesSize() { return (int)this->bones.size(); }
    inline int DofsSize() { return (int)this->dofs.size(); }

protected:
    void computeTangentPoint(const glm::vec3 &camera_ray, glm::vec3 &c1, const glm::vec3 &c2, const glm::vec3 &c3, float r1, float r2, float r3,
        glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &u1, glm::vec3 &u2, glm::vec3 &u3, glm::vec3 &n, glm::vec3 &m);
    void transformJoints(const Thetas &thetas);
    void translate(Bone &bone, const smodel::vec3 &t);
    void rotate(Bone & phalange, const smodel::vec3 &axis, float angle);
    // 递归更新每个phalange及其children的global坐标
    void updateBonesTransformMatrix(Bone &bones);
};

} // namespace smodel

#endif // MODEL_H
