#ifndef SIGN_DATA_H
#define SIGN_DATA_H

#include "LexicalItem.h"
#include "utils/csv.h"
#include "utils/csv_convert.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace signer {

typedef int SignId;
typedef unsigned int Frame;
typedef std::vector<float> Params;
typedef std::vector<Params> FrameParams;

class SignData {
private:
    LexicalItem lexical_item_;
    SignId id;
    FrameParams frame_params;
public:
    SignData();
    SignData(SignData &sign_data);
    SignData(const SignId &sign_id, const LexicalItem &lexical_item);
    explicit SignData(const FrameParams &frame_params);

    ~SignData();

    void LoadParams(const std::string &data_path);
    void Optimize();

    void AddFrame(const Params &params);
    void InsertFrame(const Frame &frame, const Params &params);
    void DelFrame(const Frame &frame);
    void ModifyFrame(const Frame &frame, const Params &params);
    Params GetFrame(const Frame &frame);
    inline Frame FrameCount() const { return (Frame)frame_params.size(); }
};

} // namespace singer

#endif // SIGN_DATA_H
