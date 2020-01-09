#ifndef MODEL_SEMANTICS_H
#define MODEL_SEMANTICS_H

#include "Model.h"

namespace smodel {

class ModelSemantics {
protected:
    Model *model_;
    Centers &centers_;
    Bones &bones_;
    Dofs &dofs_;
public:
    ModelSemantics(Model *model);
    ~ModelSemantics();

    void Init();
    void InitTopology();

    void InitCenters();
    void InitBones();
    void InitDofs();

    void InitBonesTransformMatrix();
    void InitAttachmentCentersOffset();
};

} // namespace smodel

#endif // MODEL_SEMANTICS_H
