#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <assert.h>

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QKeySequence>

#include "QUIConst.h"
#include "GLWidget.h"

namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QWidget *central_widget;
    QGridLayout *central_layout;
    GLWidget *gl_widget_;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Init();
    void SetGLWidget(GLWidget *gl_widget);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:
    void onOpenQuick();
    void onOpenBody();
    void onOpenRightHand();
    void onOpenLeftHand();
    void onOpenAnimation();


}; // class MainWindow

} // namespace ui

#endif // MAIN_WINDOW_H
