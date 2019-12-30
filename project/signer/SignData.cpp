#include "SignData.h"
#include <cstdlib>

using namespace signer;

SignData::SignData() {
    this->id = -1;
}

SignData::SignData(SignData &sign_data) {
    this->frame_params.clear();

    this->lexical_item_ = sign_data.lexical_item_;
    this->id = sign_data.id;
    this->frame_params = sign_data.frame_params;
}

SignData::SignData(const SignId &sign_id, const LexicalItem &lexical_item)
  : id(sign_id),
    lexical_item_(lexical_item)
{

}

SignData::SignData(const FrameParams &frame_params) {
    this->frame_params.clear();
    this->frame_params = frame_params;
}

SignData::~SignData() {
    frame_params.clear();
}

void SignData::AddFrame(const Params &params) {
    frame_params.push_back(params);
}

void SignData::InsertFrame(const Frame &frame, const Params &params) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        frame_params.insert(frame_params.begin() + frame, params);
    }
    else {
        std::cout << "!!! Error: inserting frame of [" << frame << "] is illegal." << std::endl;
        return;
    }
}

// TO DO
void SignData::DelFrame(const Frame &frame) {
    //if ((decltype(signs.size()))frame <= signs.size()) {
    //    Signs::iterator it = signs.find(frame);
    //    if (it != signs.end()) {
    //        signs.erase(it);
    //    }
    //}
    //else {
    //    std::cout << "!!! Error: frame of [" << frame << "] is illegal." << std::endl;
    //    return;
    //}
}

// TO DO
void SignData::ModifyFrame(const Frame &frame, const Params &params) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        frame_params[frame] = params;
    }
    else {
        std::cout << "!!! Error: modifying frame of [" << frame << "] is illegal." << std::endl;
        return;
    }
}

Params SignData::GetFrame(const Frame &frame) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        return this->frame_params[frame];
    }
    else {
        std::cout << "!!! Error: trying to read nonexistent frame of [" << frame << "]." << std::endl;
        return Params();
    }
}

// 根据LexicalItem的类型加载数据库中的手势参数
// TO DO
// 先做简单句
void SignData::LoadParams(const std::string &data_path) {
    if (data_path.empty()) {
        std::cout << "!!! Error: data file's path is not specified." << std::endl;
        return;
    }
    if (id < 0) {
        std::cout << "!!! Error: id is illegal." << std::endl;
        return;
    }

    this->frame_params.clear();
    std::string file_path = data_path + "/";
    file_path += id;
    try {
        csv::CSVReader<4> reader(file_path);
        reader.read_header(csv::ignore_extra_column, "frame", "body_thetas", "rhand_thetas", "lhand_thetas");
        Frame frame;
        std::string body_thetas, rhand_thetas, lhand_thetas;
        while (reader.read_row(frame, body_thetas, rhand_thetas, lhand_thetas)) {
            Params params = utils::ConvertToVectorFloat(body_thetas);
            Params rhand_params = utils::ConvertToVectorFloat(rhand_thetas);
            params.insert(params.end(), rhand_params.begin(), rhand_params.end());
            Params lhand_params = utils::ConvertToVectorFloat(lhand_thetas);
            params.insert(params.end(), lhand_params.begin(), lhand_params.end());
            this->AddFrame(params);
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!! Error: Can't open file.\nDetail: " << err.what() << std::endl;
    }
    catch (...) {
        std::cout << "!!! Unknow Error while reading " << file_path << "." << std::endl;
    }
}

void SignData::Optimize() {

}
