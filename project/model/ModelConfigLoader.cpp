﻿#include "ModelConfigLoader.h"
//#include "../utils/SMLog.h"

using namespace smodel;

ModelConfigLoader::ModelConfigLoader() {}

ModelCtrlMap ModelConfigLoader::LoadComposedModel(const std::string &path) {
    ModelCtrlMap model_ctrls;
    try {
        csv::CSVReader<5> reader(path);
        reader.read_header(csv::ignore_extra_column, "id", "name", "data_dir", "parent_id", "to_boneid");
        int id, parent_id, to_boneid;
        std::string name, data_dir;
        while (reader.read_row(id, name, data_dir, parent_id, to_boneid)) {
            // create a model
            Model *model = new Model(); // clear in ModelCtrl
            // create a semantics;
            ModelSemantics *model_semantics = new ModelSemantics(model);
            // create a model ctrl
            ModelCtrl *model_ctrl = new ModelCtrl(id, name, data_dir, parent_id, to_boneid); // clear in ModelsManager

            model_ctrl->SetModel(model);
            model_ctrl->SetModelSemantics(model_semantics);

            model_ctrls[id] = model_ctrl;
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
        //utils::SMLog::logger().print(utils::kError, "无法打开文件");
        //utils::SMLog::logger().print(err.what());
    }
    catch (...) {
        //utils::SMLog::logger().print(utils::kError, "未知错误");
    }
    return model_ctrls;
}

Centers ModelConfigLoader::LoadCenters(const std::string &path) {
    Centers centers;
    try {
        csv::CSVReader<5> centers_csv(path);
        centers_csv.read_header(csv::ignore_extra_column, "id", "name", "position", "radius", "type");
        int id, type;
        float radius;
        std::string name, position;
        while (centers_csv.read_row(id, name, position, radius, type)) {
            centers.push_back(Sphere(id, name, radius,
                convertToVec3(position),
                convertToSphereType(type))
            );
        }
    } catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
    }

    return centers;
}

Bones ModelConfigLoader::LoadBones(const std::string &path) {
    Bones bones;
    try {
        csv::CSVReader<6> centers_csv(path);
        centers_csv.read_header(csv::ignore_extra_column, "id", "name", "center_id", "parent_id", "attachment_ids", "init_local");
        int id, center_id, parent_id;
        std::string name, attachment_ids, init_local;
        while (centers_csv.read_row(id, name, center_id, parent_id, attachment_ids, init_local)) {
            bones.push_back(Bone(id, name, center_id, parent_id,
                convertToIntArray(attachment_ids),
                convertToMat4(init_local))
            );
        }
    } catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
    }

    return bones;
}

Dofs ModelConfigLoader::LoadDofs(const std::string &path) {
    Dofs dofs;
    try {
        csv::CSVReader<9> centers_csv(path);
        centers_csv.read_header(csv::ignore_extra_column, "id", "name", "type", "free_type", "axis_type", "min", "max", "init_val", "bone_id");
        int id, type, axis_type, free_type, bone_id;
        std::string name;
        float min, max, init_val;
        while (centers_csv.read_row(id, name, type, free_type, axis_type, min, max, init_val, bone_id)) {
            dofs.push_back(Dof(id, name,
                convertToDofType(type),
                convertToDofFreeType(free_type),
                convertToAxisType(axis_type),
                min, max, init_val,
                bone_id));
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
    }

    return dofs;
}

Blocks ModelConfigLoader::LoadBlocks(const std::string &path) {
    Blocks blocks;
    try {
        csv::CSVReader<3> centers_csv(path);
        centers_csv.read_header(csv::ignore_extra_column, "a", "b", "c");
        int a, b, c;
        while (centers_csv.read_row(a, b, c)) {
            blocks.push_back(Block(a, b, c));
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
    }

    return blocks;
}

BlockColors ModelConfigLoader::LoadBlockColors(const std::string &path) {
    BlockColors block_colors;
    try {
        csv::CSVReader<1> centers_csv(path);
        centers_csv.read_header(csv::ignore_extra_column, "mask_color");
        int color;
        while (centers_csv.read_row(color)) {
            block_colors.push_back(BlockColor(color));
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!!无法打开文件: " << err.what() << std::endl;
    }

    return block_colors;
}

/// >>> 转换函数

smodel::vec3 ModelConfigLoader::convertToVec3(const std::string &str) {
    smodel::vec3 res = smodel::vec3::Zero();
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        std::vector<float> vec;
        while (getline(ss, val, '#')) {
            vec.push_back((float)atof(val.c_str()));
        }
        if (vec.size() > 0 && vec.size() == VEC3_DIM) {
            res << vec[0], vec[1], vec[2];
        }
    }
    return res;
}

smodel::mat4 ModelConfigLoader::convertToMat4(const std::string &str) {
    smodel::mat4 res = smodel::mat4::Identity();
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        std::vector<float> vec;
        while (getline(ss, val, '#')) {
            vec.push_back((float)atof(val.c_str()));
        }
        if (vec.size() > 0 && vec.size() == MAT4_DIM * MAT4_DIM) {
            for (int u = 0; u < MAT4_DIM; ++u) {
                for (int v = 0; v < MAT4_DIM; ++v) {
                    res(v, u) = vec[MAT4_DIM * u + v];
                }
            }
        }
    }
    return res;
}

std::vector<int> ModelConfigLoader::convertToIntArray(const std::string &str) {
    std::vector<int> res;
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        while (getline(ss, val, '#')) {
            res.push_back(atoi(val.c_str()));
        }
    }
    return res;
}

SphereType ModelConfigLoader::convertToSphereType(const int &type) {
    if (type == 0 || type == 1) return (SphereType)type;
    return SphereType::kCenter;
}

DofType ModelConfigLoader::convertToDofType(const int &type) {
    if (type == 0 || type == 1) return (DofType)type;
    return DofType::kRotation;
}

DofFreeType ModelConfigLoader::convertToDofFreeType(const int &free_type) {
    if (free_type == 0 || free_type == 1) return (DofFreeType)free_type;
    return DofFreeType::kFree;
}

AxisType ModelConfigLoader::convertToAxisType(const int &type) {
    if (type == 0 || type == 1 || type == 2) return (AxisType)type;
    return AxisType::kX;
}
