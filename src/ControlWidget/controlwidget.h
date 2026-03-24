#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include "CameraInterface/CMCameraMetaInfo.h"

#include <QString>
#include <QVector>
#include <QWidget>

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = nullptr);
    ~ControlWidget();

private:
    void refreshCameraList();
    void refreshConnectButtonState();
    QString currentCameraSerial() const;

private slots:
    void onEnumerateClicked();
    void onConnectClicked();

private:
    Ui::ControlWidget *ui;
    QVector<CameraMetaInfo> m_cameraMetaInfos;
};

#endif // CONTROLWIDGET_H
