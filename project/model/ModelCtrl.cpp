#include "ModelCtrl.h"

using namespace smodel;

ModelCtrl::ModelCtrl() {
    config_loader = new ModelConfigLoader();
    semantics = new ModelSemantics(model);
    parent_bone_id_ = -1;
    selected_centerid = -1;
}

ModelCtrl::ModelCtrl(const std::string &data_path): data_path_(data_path) {
    config_loader = new ModelConfigLoader();
    semantics = new ModelSemantics(model);
    parent_bone_id_ = -1;
    selected_centerid = -1;
}

ModelCtrl::~ModelCtrl() {
    model.Clear();

    if (nullptr != config_loader)
        delete config_loader;
    config_loader = nullptr;

    if (nullptr != semantics)
        delete semantics;
    semantics = nullptr;

    children_.clear();
}

void ModelCtrl::Init() {
    model.Clear();
    model.centers = config_loader->LoadCenters(data_path_ + "/centers.csv");
    model.bones = config_loader->LoadBones(data_path_ + "/bones.csv");
    model.blocks = config_loader->LoadBlocks(data_path_ + "/blocks.csv");
    model.dofs = config_loader->LoadDofs(data_path_ + "/dofs.csv");

    semantics->Init();
}

void ModelCtrl::MoveToInit() {
    model.MoveToInit();
    MoveChildren();
}

void ModelCtrl::Move(const smodel::Thetas &thetas) {
    model.Move(thetas);
    MoveChildren();
}

void ModelCtrl::Move(const Bone &parent_bone) {
    model.Move(parent_bone);
    MoveChildren();
}

void ModelCtrl::MoveChildren() {
    for (ModelCtrl *child : children_) {
        if (nullptr != child) {
            const Bone &parent_bone = this->GetBone(child->parent_bone_id_);
            child->Move(parent_bone);
        }
    }
}

void ModelCtrl::Update() {
    model.UpdateCentersPosition();
    UpdateChildren();
}

void ModelCtrl::UpdateChildren() {
    for (ModelCtrl *child : children_) {
        if (nullptr != child) {
            child->Update();
        }
    }
}

Bone ModelCtrl::GetBone(const int &boneid) const {
    const Bones &bones = this->GetBones();
    if (boneid >= 0 && boneid < bones.size())
        return bones[boneid];
    return Bone();
}

Bone ModelCtrl::GetBone(const std::string &name) const {
    for (const Bone &bone : this->GetBones()) {
        if (bone.name == name)
            return bone;
    }
    return Bone();
}

Bone ModelCtrl::GetRootBone() const {
    return GetBone(ROOT_NAME);
}

void ModelCtrl::AddChild(ModelCtrl &child, const int to_boneid) {
    if (to_boneid >= 0 && to_boneid < model.bones.size()) {
        this->children_.push_back(&child);
        child.parent_ = this;
        child.parent_bone_id_ = to_boneid;
        child.GetModel().SetHasParent(true);

        const Bone &parent_bone = this->GetBone(to_boneid);
        child.Move(parent_bone);
        child.Update();
    }
}
