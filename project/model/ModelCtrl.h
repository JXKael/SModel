#ifndef MODEL_CTRL_H
#define MODEL_CTRL_H

#include "Model.h"
#include "ModelConfigLoader.h"
#include "ModelSemantics.h"

namespace smodel {

class ModelCtrl {
private:
    Model model;
    ModelConfigLoader *config_loader;
    ModelSemantics *semantics;

    const std::string name_;
    const std::string project_path_;

    ModelCtrl *parent_;
    int parent_bone_id_;
    std::vector<ModelCtrl *> children_;

    int selected_centerid;

public:
    // constructor & destructor
    explicit ModelCtrl();
    explicit ModelCtrl(const std::string &project_path);
    ModelCtrl(const std::string &name, const std::string &project_path);
    ~ModelCtrl();

public:
    inline Model &GetModel() { return model; }

public:
    void Init();
    void MoveToInit();
    void Move(const Thetas &thetas);
    void Move(const Bone &parent_bone);
    void Update();
    inline const int GetDofsSize() { return model.DofsSize(); }
    inline const Centers &GetCenters() const { return model.centers; }
    inline const Bones &GetBones() const { return model.bones; }
    Bone GetBone(const int &boneid) const;
    Bone GetBone(const std::string &name) const;
    Bone GetRootBone() const;
    inline const Blocks &GetBlocks() const { return model.blocks; }
    inline const BlockColors &GetBlockColors() const { return model.block_colors; }
    inline const Dofs &GetDofs() const { return model.dofs; }
    Thetas GetInitThetas() { return model.GetInitThetas(); }
    inline const Thetas &GetThetas() const { return model.theta; }
    inline const TangentPoints &GetTangentPoints() const { return model.tangent_points; }

    void AddChild(ModelCtrl &child, const int to_boneid = -1);

    inline void SetSelectedCenterid(int centerid = -1) { selected_centerid = centerid; }
    inline int GetSelectedCenterid() const { return selected_centerid; }

    inline const std::string &GetName() const { return this->name_; }

    inline const bool HasParent() const { return model.has_parent; }
private:
    void MoveChildren();
    void UpdateChildren();
};

} // namespace smodel

#endif // MODEL_CTRL_H
