#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "Listener.h"

#include <QImage>
#include <QWidget>

class ControlWidget;
class AcquireImageProcess;

namespace Ui {
class ViewWidget;
}

class ViewWidget : public QWidget, public Listener
{
    Q_OBJECT

public:
    explicit ViewWidget(ControlWidget *controlWidget, QWidget *parent = nullptr);
    ~ViewWidget();
    void RespondMessage(int message) override;

private:
    void refreshGrabbingButtonState();
    void stopAcquireProcess();

private slots:
    void onGrabbingClicked();
    void onImageReady(const QString& serial, const QImage& image);

private:
    Ui::ViewWidget *ui;
    ControlWidget *m_controlWidget;
    AcquireImageProcess *m_acquireImageProcess;
    QString m_previewSerial;
};

#endif // VIEWWIDGET_H
