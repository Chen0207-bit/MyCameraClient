#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QImage>
#include <QWidget>

class ControlWidget;
class AcquireImageProcess;

namespace Ui {
class ViewWidget;
}

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(ControlWidget *controlWidget, QWidget *parent = nullptr);
    ~ViewWidget();

signals:
    void viewStateChanged();

private:
    void refreshGrabbingButtonState();
    void stopAcquireProcess();

private slots:
    void onGrabbingClicked();
    void onImageReady(const QImage& image);

private:
    Ui::ViewWidget *ui;
    ControlWidget *m_controlWidget;
    AcquireImageProcess *m_acquireImageProcess;
};

#endif // VIEWWIDGET_H
