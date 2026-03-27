#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

class ControlWidget;

namespace Ui {
class ViewWidget;
}

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(ControlWidget *controlWidget, QWidget *parent = nullptr);
    ~ViewWidget();

private:
    void refreshGrabbingButtonState();

private slots:
    void onGrabbingClicked();

private:
    Ui::ViewWidget *ui;
    ControlWidget *m_controlWidget;
};

#endif // VIEWWIDGET_H
