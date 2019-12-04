#include "QUIRenderImg.h"
#include "QUIManager.h"

using namespace ui;

QUIRenderImg::QUIRenderImg(const std::string &project_path) :project_path_(project_path) {

}

QUIRenderImg::~QUIRenderImg() {}

void QUIRenderImg::Init() {
    QGridLayout *layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // 文件路径
    file_path = new QLineEdit();
    file_path->setFocusPolicy(Qt::ClickFocus);
    file_path->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    file_path->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_path->setText(QString("%1/data/images/").arg(project_path_.c_str()));

    // 文件名称
    file_name = new QLineEdit();
    file_name->setFocusPolicy(Qt::ClickFocus);
    file_name->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 4);
    file_name->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_name->setPlaceholderText("file name");

    // 文件后缀
    file_suffix = new QLabel();
    file_suffix->setFocusPolicy(Qt::NoFocus);
    file_suffix->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_suffix->setText(".jpg");

    // 设置屏幕为渲染手部
    QPushButton *btn_render_hand = new QPushButton();
    btn_render_hand->setText("Normlize Screen");
    btn_render_hand->setFocusPolicy(Qt::ClickFocus);
    btn_render_hand->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_hand, SIGNAL(pressed()), this, SLOT(onClickBtnRenderHand()));

    // 单张渲染
    QPushButton *btn_render = new QPushButton();
    btn_render->setText("Render One");
    btn_render->setFocusPolicy(Qt::ClickFocus);
    btn_render->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render, SIGNAL(pressed()), this, SLOT(onClickBtnRender()));

    // 批量渲染
    QPushButton *btn_render_batch = new QPushButton();
    btn_render_batch->setText("Render Batch");
    btn_render_batch->setFocusPolicy(Qt::ClickFocus);
    btn_render_batch->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_batch, SIGNAL(pressed()), this, SLOT(onClickBtnRenderBatch()));

    layout->addWidget(file_path, 0, 0, 1, 4);
    layout->addWidget(file_name, 0, 4, 1, 2);
    layout->addWidget(file_suffix, 0, 7, 1, 1);
    layout->addWidget(btn_render_hand, 1, 0, 1, 2);
    layout->addWidget(btn_render, 2, 0, 1, 2);
    layout->addWidget(btn_render_batch, 2, 2, 1, 2);

    // 总的弹板设置
    this->setLayout(layout);
    this->resize(600, 100);
    this->move(80, 20);
    this->setWindowTitle("Render Image");
}

void QUIRenderImg::UpdateModel(const std::string &name, const smodel::Thetas &thetas) {
    //models_[name]->Move(thetas);
    //models_[name]->Update();
}

void QUIRenderImg::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

void QUIRenderImg::onClickBtnRenderHand() {
    QUIManager::Instance().GetGLWindow()->SetRenderHandScreen();
}

void QUIRenderImg::onClickBtnRender() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text() + ".jpg").toStdString();
        QUIManager::Instance().GetGLWindow()->SaveScreenImg(file);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}

void QUIRenderImg::onClickBtnRenderBatch() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text() + ".jpg").toStdString();
        QUIManager::Instance().GetGLWindow()->SaveScreenImgBatch(file_path->text().toStdString(), file_name->text().toStdString(), "jpg");
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}
