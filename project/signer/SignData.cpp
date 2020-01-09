#include "SignData.h"
#include <cstdlib>

using namespace signs;

SignData::SignData() {
    id = -1;
}

SignData::SignData(const SignId &sign_id, const Params &all_params)
  : id(sign_id),
    params(all_params)
{

}

SignData::~SignData() {
    params.clear();
}

void SignData::AddFrame(const FrameParams &frame_params) {
    this->params.push_back(frame_params);
}

void SignData::InsertFrame(const Frame &frame, const FrameParams &frame_params) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        this->params.insert(this->params.begin() + frame, frame_params);
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
void SignData::ModifyFrame(const Frame &frame, const FrameParams &frame_params) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        params[frame] = frame_params;
    }
    else {
        std::cout << "!!! Error: modifying frame of [" << frame << "] is illegal." << std::endl;
        return;
    }
}

FrameParams SignData::GetFrame(const Frame &frame) {
    const Frame frame_count = this->FrameCount();
    if (frame < frame_count) {
        return this->params[frame];
    }
    else {
        std::cout << "!!! Error: trying to read nonexistent frame of [" << frame << "]." << std::endl;
        return FrameParams();
    }
}

bool SignData::IsValid() {
    return this->FrameCount() > 0 && id >= 0;
}

// TO DO
FrameParams SignData::GetParams(const std::string &model_name, const Frame &frame) {
    if (model_name == "body") {
        return GetBodyParams(frame);
    }
    else if (model_name == "right-hand") {
        return GetRightHandParams(frame);
    }
    else if (model_name == "left-hand") {
        return GetLeftHandParams(frame);
    }
    else {
        return FrameParams();
    }
}

FrameParams SignData::GetBodyParams(const Frame &frame) {
    FrameParams res;
    res.insert(res.end(), params[frame].begin(), params[frame].begin() + 23);
    return res;
}

FrameParams SignData::GetRightHandParams(const Frame &frame) {
    FrameParams res;
    res.insert(res.end(), params[frame].begin() + 23, params[frame].begin() + 23 + 26);
    return res;
}

FrameParams SignData::GetLeftHandParams(const Frame &frame) {
    FrameParams res;
    res.insert(res.end(), params[frame].begin() + 23 + 26, params[frame].end());
    return res;
}
