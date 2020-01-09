#include "ModelCtrl.h"

using namespace smodel;

//ModelCtrl::ModelCtrl() : name_("model"), mask_val_(0) {
//    //config_loader = new ModelConfigLoader();
//    //semantics_ = new ModelSemantics(model);
//    parent_bone_id_ = -1;
//    selected_centerid = -1;
//}

ModelCtrl::ModelCtrl(const int& id, const std::string &name, const std::string &data_dir, const int &parent_id, const int &to_boneid)
  : id_(id),
    name_(name),
    data_dir_(data_dir),
    parent_id_(parent_id),
    parent_bone_id_(to_boneid),
    model(nullptr),
    parent_(nullptr),
    selected_centerid(-1),
    mask_val_(-1)
{

}

//ModelCtrl::ModelCtrl(const std::string &project_path): name_("model"), mask_val_(0) {
//    //config_loader = new ModelConfigLoader();
//    //semantics_ = new ModelSemantics(model);
//    parent_bone_id_ = -1;
//    selected_centerid = -1;
//}

//ModelCtrl::ModelCtrl(const std::string &name, const std::string &project_path) : name_(name), mask_val_(0) {
//    //config_loader = new ModelConfigLoader();
//    //semantics_ = new ModelSemantics(model);
//    parent_bone_id_ = -1;
//    selected_centerid = -1;
//}

ModelCtrl::~ModelCtrl() {
    model->Clear();
    if (nullptr != model) delete model;
    model = nullptr;

    //if (nullptr != config_loader)
    //    delete config_loader;
    //config_loader = nullptr;

    //if (nullptr != semantics_)
    //    delete semantics_;
    //semantics_ = nullptr;

    children_.clear();
}

void ModelCtrl::Init() {
    semantics->Init();
}

void ModelCtrl::MoveToInit() {
    model->MoveToInit();
    MoveChildren();
}

void ModelCtrl::Move(const smodel::Thetas &thetas) {
    model->Move(thetas);
    MoveChildren();
}

void ModelCtrl::Move(const Bone &parent_bone) {
    model->Move(parent_bone);
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
    model->UpdateCentersPosition();
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

void ModelCtrl::AddChild(ModelCtrl *child, const int to_boneid) {
    if (to_boneid >= 0 && to_boneid < model->bones.size()) {
        this->children_.push_back(child);
        child->parent_ = this;
        child->GetModel()->SetHasParent(true);

        const Bone &parent_bone = this->GetBone(to_boneid);
        child->Move(parent_bone);
        child->Update();
    }
    else {
        std::cout << "### [error]: the id [" << to_boneid << "] of the bone is illegal." << std::endl;
    }
}

void ModelCtrl::SetMaskValue(const int &mask_val) {
    this->mask_val_ = mask_val;
}

const int ModelCtrl::GetMaskValue() const {
    return this->mask_val_;
}
