#include "paramwidget.h"
#include "ui_paramwidget.h"

#include "CameraInterface/CMCameraMetaInfo.h"
#include "CameraInterface/CameraContext.h"
#include "ControlWidget/controlwidget.h"
#include "ParamWidget/CameraParamItem.h"
#include "ParamWidget/CameraParamModel.h"
#include "ParamWidget/cameraparamdelegate.h"

#include <QHeaderView>
#include <QItemSelectionModel>

ParamWidget::ParamWidget(ControlWidget *controlWidget, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParamWidget)
    , m_controlWidget(controlWidget)
    , m_paramModel(new CameraParamModel({"Param", "Value"}, this))
    , m_paramDelegate(new CameraParamDelegate(this))
{
    ui->setupUi(this);

    ui->paramTreeView->setModel(m_paramModel);
    ui->paramTreeView->setItemDelegate(m_paramDelegate);
    ui->paramTreeView->setAlternatingRowColors(true);
    ui->paramTreeView->setRootIsDecorated(true);
    ui->paramTreeView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    ListenerManager::Instance()->registerMessage(
        MESSAGE::CAMERA_ENUMERATION |
        MESSAGE::CAMERA_SWITCH |
        MESSAGE::CAMERA_CONNECT |
        MESSAGE::CAMERA_DISCONNECT |
        MESSAGE::CAMERA_STARTGRAB |
        MESSAGE::CAMERA_STOPGRAB,
        this);

    connect(ui->refreshButton, &QPushButton::clicked,
            this, &ParamWidget::onRefreshClicked);
    connect(ui->paramTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ParamWidget::onSelectionChanged);
    connect(m_paramModel, &CameraParamModel::SigValueChanged,
            this, &ParamWidget::writeCameraParam);

    refreshInfoPanel();
}

ParamWidget::~ParamWidget()
{
    ListenerManager::Instance()->unregisterListener(this);
    delete ui;
}

void ParamWidget::RespondMessage(int message)
{
    if ((message & MESSAGE::CAMERA_STARTGRAB) == MESSAGE::CAMERA_STARTGRAB)
    {
        ui->paramTreeView->setEnabled(false);
        ui->refreshButton->setEnabled(false);
    }

    if ((message & MESSAGE::CAMERA_STOPGRAB) == MESSAGE::CAMERA_STOPGRAB)
    {
        ui->paramTreeView->setEnabled(true);
        ui->refreshButton->setEnabled(true);
    }

    refreshInfoPanel();
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
        clearParamWidget();
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

    ui->paramTreeView->setEnabled(isConnected && !isGrabbing);
    ui->refreshButton->setEnabled(isConnected && !isGrabbing);

    if (!isConnected)
    {
        clearParamWidget();
        ui->paramDescriptionBrowser->setPlainText("Connect the camera before loading virtual parameters.");
        ui->summaryLabel->setText("Camera info is visible now. Connect the camera to load virtual parameters.");
        return;
    }

    QVector<CameraParam> paramList;
    const auto paramRet = CameraContext::Instance()->getParamList(info.Serial, paramList);
    if (paramRet != CHONGMING_OK)
    {
        clearParamWidget();
        ui->paramDescriptionBrowser->setPlainText("Failed to read parameter list from CameraContext.");
        ui->summaryLabel->setText("Camera state is available, but the parameter tree could not be loaded.");
        return;
    }

    refreshParamTree(paramList);
    ui->summaryLabel->setText(
        QString("Loaded %1 virtual parameter(s) across %2 group(s).")
            .arg(paramList.size())
            .arg(ui->paramTreeView->model()->rowCount()));
}

void ParamWidget::onSelectionChanged(const QItemSelection& selected, const QItemSelection&)
{
    ui->paramDescriptionBrowser->clear();

    const QModelIndexList indexes = selected.indexes();
    for (const auto& index : indexes)
    {
        const CameraParam param = index.data(Qt::UserRole).value<CameraParam>();
        if (!param.Name.isEmpty())
        {
            ui->paramDescriptionBrowser->setPlainText(param.Tips);
            return;
        }
    }
}

void ParamWidget::onRefreshClicked()
{
    refreshInfoPanel();
    ui->paramTreeView->expandAll();
}

void ParamWidget::writeCameraParam(const QModelIndex& index)
{
    const CameraParam param = index.data(Qt::UserRole).value<CameraParam>();
    if (param.Name.isEmpty())
    {
        return;
    }

    const QString serial = m_controlWidget ? m_controlWidget->currentCameraSerial() : QString{};
    if (serial.isEmpty())
    {
        return;
    }

    const auto ret = CameraContext::Instance()->writeParam(serial, param);
    if (ret != CHONGMING_OK)
    {
        ui->summaryLabel->setText(QString("Failed to write parameter: %1").arg(param.Name));
        return;
    }

    ui->summaryLabel->setText(QString("Updated parameter %1 to %2").arg(param.Name, param.Value));
}

void ParamWidget::refreshParamTree(const QVector<CameraParam>& paramList)
{
    m_paramModel->clear();

    for (const auto& param : paramList)
    {
        m_paramModel->addCameraParam(param);
    }

    ui->paramTreeView->expandAll();
    ui->paramTreeView->header()->setStretchLastSection(true);
    ui->paramTreeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    if (m_paramModel->rowCount() > 0)
    {
        const QModelIndex firstGroup = m_paramModel->index(0, 0);
        if (m_paramModel->rowCount(firstGroup) > 0)
        {
            ui->paramTreeView->setCurrentIndex(m_paramModel->index(0, 0, firstGroup));
        }
    }
}

void ParamWidget::clearParamWidget()
{
    m_paramModel->clear();
    ui->paramDescriptionBrowser->clear();
}
