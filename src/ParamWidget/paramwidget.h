#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QWidget>

class ControlWidget;

namespace Ui {
class ParamWidget;
}

class ParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParamWidget(ControlWidget *controlWidget, QWidget *parent = nullptr);
    ~ParamWidget();

private:
    void refreshInfoPanel();

private:
    Ui::ParamWidget *ui;
    ControlWidget *m_controlWidget;
};

#endif // PARAMWIDGET_H
