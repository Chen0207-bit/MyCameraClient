#include "paramwidget.h"
#include "ui_paramwidget.h"

#include "CameraInterface/CMCameraMetaInfo.h"
#include "CameraInterface/CameraContext.h"
#include "ControlWidget/controlwidget.h"

#include <QListWidgetItem>

namespace {
constexpr int kTipsRole = Qt::UserRole;
}

ParamWidget::ParamWidget(ControlWidget *controlWidget, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParamWidget)
    , m_controlWidget(controlWidget)
{
    ui->setupUi(this);

    connect(m_controlWidget, &ControlWidget::controlStateChanged,
            this, &ParamWidget::refreshInfoPanel);
    connect(ui->paramListWidget, &QListWidget::currentItemChanged,
            this, [this](QListWidgetItem*, QListWidgetItem*) {
                onCurrentParamChanged();
            });

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
        ui->paramListWidget->clear();
        ui->paramDescriptionBrowser->setPlainText("Enumerate a camera to view its parameter descriptions.");
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

    if (!isConnected)
    {
        ui->paramListWidget->clear();
        ui->paramDescriptionBrowser->setPlainText("Connect the camera before loading virtual parameters.");
        ui->summaryLabel->setText("Camera info is visible now. Connect the camera to load virtual parameters.");
        return;
    }

    QVector<CameraParam> paramList;
    const auto paramRet = CameraContext::Instance()->getParamList(info.Serial, paramList);
    if (paramRet != CHONGMING_OK)
    {
        ui->paramListWidget->clear();
        ui->paramDescriptionBrowser->setPlainText("Failed to read parameter list from CameraContext.");
        ui->summaryLabel->setText("Camera state is available, but the parameter list could not be loaded.");
        return;
    }

    refreshParamList(paramList);
    ui->summaryLabel->setText(
        QString("Loaded %1 virtual parameter(s) for the current camera.").arg(paramList.size()));
}

void ParamWidget::onCurrentParamChanged()
{
    auto* currentItem = ui->paramListWidget->currentItem();
    if (currentItem == nullptr)
    {
        ui->paramDescriptionBrowser->clear();
        return;
    }

    ui->paramDescriptionBrowser->setPlainText(currentItem->data(kTipsRole).toString());
}

void ParamWidget::refreshParamList(const QVector<CameraParam>& paramList)
{
    ui->paramListWidget->clear();

    for (const auto& param : paramList)
    {
        auto* item = new QListWidgetItem(
            QString("%1 = %2").arg(param.Name, param.Value),
            ui->paramListWidget);
        item->setData(kTipsRole, param.Tips);
    }

    if (!paramList.isEmpty())
    {
        ui->paramListWidget->setCurrentRow(0);
        return;
    }

    ui->paramDescriptionBrowser->setPlainText("No parameters are available for the current camera.");
}
