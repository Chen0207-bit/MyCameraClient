#include "paramwidget.h"
#include "ui_paramwidget.h"

#include "CameraInterface/CMCameraMetaInfo.h"
#include "CameraInterface/CameraContext.h"
#include "ControlWidget/controlwidget.h"

ParamWidget::ParamWidget(ControlWidget *controlWidget, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParamWidget)
    , m_controlWidget(controlWidget)
{
    ui->setupUi(this);

    connect(m_controlWidget, &ControlWidget::controlStateChanged,
            this, &ParamWidget::refreshInfoPanel);

    refreshInfoPanel();
}

ParamWidget::~ParamWidget()
{
    delete ui;
}

void ParamWidget::refreshInfoPanel()
{
    const CameraMetaInfo info = m_controlWidget ? m_controlWidget->currentCameraInfo() : CameraMetaInfo{};

    if (info.Serial.isEmpty())
    {
        ui->userNameValueLabel->setText("None");
        ui->serialValueLabel->setText("None");
        ui->vendorValueLabel->setText("None");
        ui->connectStateValueLabel->setText("Not connected");
        ui->grabbingStateValueLabel->setText("Not grabbing");
        ui->summaryLabel->setText("Enumerate a camera to view its basic information.");
        return;
    }

    ui->userNameValueLabel->setText(info.UserDefineID);
    ui->serialValueLabel->setText(info.Serial);
    ui->vendorValueLabel->setText(info.VenderName);

    bool isConnected = false;
    const auto connectRet = CameraContext::Instance()->isConnect(info.Serial, isConnected);
    if (connectRet == CHONGMING_OK)
    {
        ui->connectStateValueLabel->setText(isConnected ? "Connected" : "Disconnected");
    }
    else
    {
        ui->connectStateValueLabel->setText("Unknown");
    }

    bool isGrabbing = false;
    const auto grabbingRet = CameraContext::Instance()->isGrabbing(info.Serial, isGrabbing);
    if (grabbingRet == CHONGMING_OK)
    {
        ui->grabbingStateValueLabel->setText(isGrabbing ? "Grabbing" : "Idle");
    }
    else
    {
        ui->grabbingStateValueLabel->setText("Unknown");
    }

    ui->summaryLabel->setText("Round 6: ParamWidget now displays current camera info and status.");
}
