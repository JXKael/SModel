#ifndef SIGN_DATA_H
#define SIGN_DATA_H

#include "utils/csv.h"
#include "utils/csv_convert.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace signs {

typedef int SignId;
typedef std::vector<SignId> SignIds;

typedef unsigned int Frame;
typedef std::vector<float> FrameParams;
typedef std::vector<FrameParams> Params;

class SignData {
private:
    SignId id;
    Params params;
public:
    SignData();
    SignData(const SignId &sign_id, const Params &all_params);

    ~SignData();

    SignData &operator=(const SignData &) = delete;

    void AddFrame(const FrameParams &frame_params);
    void InsertFrame(const Frame &frame, const FrameParams &frame_params);
    void DelFrame(const Frame &frame);
    void ModifyFrame(const Frame &frame, const FrameParams &params);
    FrameParams GetFrame(const Frame &frame);
    inline Frame FrameCount() const { return (Frame)params.size(); }

    bool IsValid();

    FrameParams GetParams(const std::string &model_name, const Frame &frame);
    FrameParams GetBodyParams(const Frame &frame);
    FrameParams GetRightHandParams(const Frame &frame);
    FrameParams GetLeftHandParams(const Frame &frame);
};

typedef std::vector<SignData> SignDatas;

} // namespace singer

#endif // SIGN_DATA_H
