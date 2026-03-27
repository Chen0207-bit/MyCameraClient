#include "viewwidget.h"
#include "ui_viewwidget.h"

#include "CameraInterface/CameraContext.h"
#include "ControlWidget/controlwidget.h"

ViewWidget::ViewWidget(ControlWidget *controlWidget, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewWidget)
    , m_controlWidget(controlWidget)
{
    ui->setupUi(this);

    connect(ui->grabbingButton, &QPushButton::clicked,
            this, &ViewWidget::onGrabbingClicked);
    connect(m_controlWidget, &ControlWidget::controlStateChanged,
            this, &ViewWidget::refreshGrabbingButtonState);

    refreshGrabbingButtonState();
}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::refreshGrabbingButtonState()
{
    const QString serial = m_controlWidget ? m_controlWidget->currentCameraSerial() : QString{};
    if (serial.isEmpty())
    {
        ui->grabbingButton->setText("Start Grabbing");
        ui->grabbingButton->setEnabled(false);
        ui->cameraLabel->setText("Current Camera: None");
        return;
    }

    ui->grabbingButton->setEnabled(true);
    ui->cameraLabel->setText(QString("Current Camera: %1").arg(serial));

    bool isGrabbing = false;
    const auto ret = CameraContext::Instance()->isGrabbing(serial, isGrabbing);
    if (ret != CHONGMING_OK)
    {
        ui->grabbingButton->setText("Start Grabbing");
        return;
    }

    ui->grabbingButton->setText(isGrabbing ? "Stop Grabbing" : "Start Grabbing");
}

void ViewWidget::onGrabbingClicked()
{
    const QString serial = m_controlWidget ? m_controlWidget->currentCameraSerial() : QString{};
    //如果没选中相机，直接报错并刷新
    if (serial.isEmpty())
    {
        ui->statusLabel->setText("Please enumerate and connect a camera first");
        refreshGrabbingButtonState();
        return;
    }
    //先检查这台相机有没有连接
    bool isConnected = false;
    auto ret = CameraContext::Instance()->isConnect(serial, isConnected);
    if (ret != CHONGMING_OK || !isConnected)
    {
        ui->statusLabel->setText("Please connect the camera before grabbing");
        refreshGrabbingButtonState();
        return;
    }
    //再检查当前是否已经在采集
    bool isGrabbing = false;
    ret = CameraContext::Instance()->isGrabbing(serial, isGrabbing);
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Camera not found");
        refreshGrabbingButtonState();
        return;
    }

    if (isGrabbing)
    {
        ret = CameraContext::Instance()->stopGrabbing(serial);
        if (ret != CHONGMING_OK)
        {
            ui->statusLabel->setText("Stop grabbing failed");
            return;
        }

        ui->statusLabel->setText(QString("Stopped grabbing: %1").arg(serial));
        refreshGrabbingButtonState();
        return;
    }

    ret = CameraContext::Instance()->startGrabbing(serial);
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Start grabbing failed");
        return;
    }

    ui->statusLabel->setText(QString("Grabbing started: %1").arg(serial));
    refreshGrabbingButtonState();
}
