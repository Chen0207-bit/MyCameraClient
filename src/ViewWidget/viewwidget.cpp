#include "viewwidget.h"
#include "ui_viewwidget.h"

#include "CameraInterface/CameraContext.h"
#include "ControlWidget/controlwidget.h"
#include "AcquireImageProcess.h"

#include <QPixmap>

ViewWidget::ViewWidget(ControlWidget *controlWidget, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViewWidget)
    , m_controlWidget(controlWidget)
    , m_acquireImageProcess(new AcquireImageProcess(this))
{
    ui->setupUi(this);

    ListenerManager::Instance()->registerMessage(
        MESSAGE::CAMERA_ENUMERATION |
        MESSAGE::CAMERA_SWITCH |
        MESSAGE::CAMERA_CONNECT |
        MESSAGE::CAMERA_DISCONNECT |
        MESSAGE::CAMERA_STARTGRAB |
        MESSAGE::CAMERA_STOPGRAB,
        this);
    connect(ui->grabbingButton, &QPushButton::clicked,
            this, &ViewWidget::onGrabbingClicked);
    connect(m_acquireImageProcess, &AcquireImageProcess::imageReady,
            this, &ViewWidget::onImageReady);

    refreshGrabbingButtonState();
}

ViewWidget::~ViewWidget()
{
    ListenerManager::Instance()->unregisterListener(this);
    stopAcquireProcess();
    delete ui;
}

void ViewWidget::RespondMessage(int)
{
    refreshGrabbingButtonState();

    bool isGrabbing = false;
    const QString serial = m_controlWidget ? m_controlWidget->currentCameraSerial() : QString{};
    const auto ret = CameraContext::Instance()->isGrabbing(serial, isGrabbing);
    if (serial.isEmpty() || ret != CHONGMING_OK || !isGrabbing)
    {
        ui->imageLabel->setPixmap(QPixmap());
        ui->imageLabel->setText("Round 2 Preview Area\n\nClick Start Grabbing to preview the current camera.");
    }
}

void ViewWidget::refreshGrabbingButtonState()
{
    const QString serial = m_controlWidget ? m_controlWidget->currentCameraSerial() : QString{};
    //如果 m_controlWidget 存在，就用m_controlWidget->currentCameraSerial()作为 serial否则就给 serial 一个空的 QString
    if (serial.isEmpty())
    {
        stopAcquireProcess();
        ui->grabbingButton->setText("Start Grabbing");
        ui->grabbingButton->setEnabled(false);
        ui->cameraLabel->setText("Current Camera: None");
        ui->imageLabel->setText("Round 2 Preview Area\n\nEnumerate and connect a camera first.");
        ui->imageLabel->setPixmap(QPixmap());
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
    if (!isGrabbing)
    {
        stopAcquireProcess();
    }
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
        stopAcquireProcess();
        refreshGrabbingButtonState();
        ListenerManager::Instance()->notify(MESSAGE::CAMERA_STOPGRAB);
        return;
    }
    //停止完会立刻刷新按钮，把文字变回 Start Grabbing。
    ret = CameraContext::Instance()->startGrabbing(serial);
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Start grabbing failed");
        return;
    }

    ui->statusLabel->setText(QString("Grabbing started: %1").arg(serial));
    m_acquireImageProcess->setSerial(serial);
    if (!m_acquireImageProcess->isRunning())
    {
        m_acquireImageProcess->start();
    }
    refreshGrabbingButtonState();
    ListenerManager::Instance()->notify(MESSAGE::CAMERA_STARTGRAB);
}

void ViewWidget::onImageReady(const QImage& image)
{
    if (image.isNull())
    {
        return;
    }

    const QPixmap pixmap = QPixmap::fromImage(image);
    ui->imageLabel->setPixmap(
        pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ViewWidget::stopAcquireProcess()
{
    if (m_acquireImageProcess == nullptr || !m_acquireImageProcess->isRunning())
    {
        return;
    }

    m_acquireImageProcess->stop();
    m_acquireImageProcess->wait();
}
