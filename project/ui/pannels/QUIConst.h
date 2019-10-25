#ifndef QUI_CONST_H
#define QUI_CONST_H

#define SINGLE_LINE_HEIGHT 24 // 一行的高度
#define EDIT_TO_SLIDER_VLAUE(y, k, b) ((y - b) / k)
#define SLIDER_TO_EDIT_VLAUE(x, k, b) (k * x + b)

#define ADJUST_SLIDER_MIN 0
#define ADJUST_SLIDER_MAX INT_MAX
#define ADJUST_SLIDER_MAXHALF (ADJUST_SLIDER_MAX * 0.5f)
#define ADJUST_OPERATIONG_BTN_WIDTH 400

#define ADJUST_SAVE_BATCH_X 32
#define ADJUST_SAVE_BATCH_Y 32 // 最终数量 y * (x + 1)

#define CALIBRATION_RANGE 16.0
#define CALIBRATION_SLIDER_MIN 0
#define CALIBRATION_SLIDER_MAX INT_MAX
#define CALIBRATION_SLIDER_MAXHALF (CALIBRATION_SLIDER_MAX * 0.5f)

#define OPERATION_LABEL_WIDTH 110
#define OPERATION_SLIDER_WIDTH 200
#define OPERATION_EDIT_WIDTH 100

#define RADII_MIN 0.01
#define RADII_MAX 32.00

#define OFFSET_MIN -100.0f
#define OFFSET_MAX 100.0f

#define ATTACHMENT "Attachment"

#define PARAM_FIX 1000
#define PARAM_ENCODER(ID, IDX) (ID * PARAM_FIX + IDX)

#define SINGLE_BTN_HEIGHT 40
#define SINGLE_BTN_WIDTH 80
#define PANNEL_WIDTH 350
#define BTN_COLUMN_NUM 4

enum Hand {
    kRight = 0,
    kLeft = 1
};

enum ParamType {
    kSlope = 0, // 斜率
    kIntercept  // 截距
};

enum MINMAX {
    kMin = 0,
    kMax
};

enum TreeItemType { 
    kNone = 0,
    kBone,
    kAttachment
};

#endif // QUI_CONST_H

