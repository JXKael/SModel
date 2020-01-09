#ifndef MODEL_CTRL_H
#define MODEL_CTRL_H

#include "Model.h"
#include "ModelSemantics.h"

namespace smodel {

class ModelCtrl {
private:
    // config in file
    const int id_;
    const std::string name_;
    const std::string data_dir_;
    const int parent_id_;
    const int parent_bone_id_;

    Model *model;
    ModelSemantics *semantics;
    ModelCtrl *parent_;
    std::vector<ModelCtrl *> children_;

    int selected_centerid;
    int mask_val_;

public:
    // constructor & destructor
    ModelCtrl() = delete;
    ModelCtrl(const int& id, const std::string &name, const std::string &data_path, const int &parent_id, const int &to_boneid);
    ~ModelCtrl();

public:
    inline void SetModel(Model *model) { this->model = model; }
    inline Model *GetModel() { return model; }
    inline void SetModelSemantics(ModelSemantics *model_semantics) { this->semantics = model_semantics; }
    inline ModelSemantics *GetModelSemantics() { return semantics; }
    void Init();
    void MoveToInit();
    void Move(const Thetas &thetas);
    void Move(const Bone &parent_bone);
    void Update();
    inline const int GetDofsSize() { return model->DofsSize(); }
    inline const Centers &GetCenters() const { return model->centers; }
    inline const Bones &GetBones() const { return model->bones; }
    Bone GetBone(const int &boneid) const;
    Bone GetBone(const std::string &name) const;
    Bone GetRootBone() const;
    inline const Blocks &GetBlocks() const { return model->blocks; }
    inline const BlockColors &GetBlockColors() const { return model->block_colors; }
    inline const Dofs &GetDofs() const { return model->dofs; }
    Thetas GetInitThetas() { return model->GetInitThetas(); }
    inline const Thetas &GetThetas() const { return model->theta; }
    inline const TangentPoints &GetTangentPoints() const { return model->tangent_points; }

    void AddChild(ModelCtrl *child, const int to_boneid = -1);

    inline void SetSelectedCenterid(int centerid = -1) { selected_centerid = centerid; }
    inline int GetSelectedCenterid() const { return selected_centerid; }

    inline const std::string &GetName() const { return this->name_; }

    inline const bool HasParent() const { return this->parent_id_ >= 0; }

    void SetMaskValue(const int &mask_val);
    const int GetMaskValue() const;


    inline const int GetParentId() const { return this->parent_id_; }
    inline const int GetParentBoneid() const { return this->parent_bone_id_; }
private:
    void MoveChildren();
    void UpdateChildren();
};

typedef std::map<int, smodel::ModelCtrl *> ModelCtrlMap;
typedef std::map<std::string, smodel::ModelCtrl *> models_map;

} // namespace smodel

#endif // MODEL_CTRL_H
