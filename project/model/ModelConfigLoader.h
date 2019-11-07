#ifndef MODEL_CONFIG_LOADER_H
#define MODEL_CONFIG_LOADER_H

#include "ModelType.h"
#include "utils/csv.h"
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

namespace smodel {

class ModelConfigLoader {
public:
    ModelConfigLoader();
    Centers LoadCenters(const std::string &path);
    Bones LoadBones(const std::string &path);
    Dofs LoadDofs(const std::string &path);
    Blocks LoadBlocks(const std::string &path);

private:
    smodel::vec3 convertToVec3(const std::string &str);
    smodel::mat4 convertToMat4(const std::string &str);
    std::vector<int> convertToIntArray(const std::string &str);

    SphereType convertToSphereType(const int &type);
    DofType convertToDofType(const int &type);
    DofFreeType convertToDofFreeType(const int &free_type);
    AxisType convertToAxisType(const int &type);
};

} // namespace smodel

#endif // MODEL_CONFIG_LOADER_H