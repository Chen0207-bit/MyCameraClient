#include "controlwidget.h"
#include "ui_controlwidget.h"

#include "CameraInterface/CMCameraMetaInfo.h"
#include "CameraInterface/CameraContext.h"

ControlWidget::ControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlWidget)
{
    ui->setupUi(this);

    connect(ui->enumerateButton, &QPushButton::clicked,
            this, &ControlWidget::onEnumerateClicked);
    connect(ui->connectButton, &QPushButton::clicked,
            this, &ControlWidget::onConnectClicked);
    connect(ui->cameraListWidget, &QListWidget::currentRowChanged,
            this, [this](int) { refreshConnectButtonState(); });

    refreshConnectButtonState();
}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::refreshCameraList()
{
    ui->cameraListWidget->clear();

    for (const auto& cameraInfo : m_cameraMetaInfos)
    {
        // 堆：
        // [ VirtualCamera ]
        // map：
        // key: "VIR-0001"
        // value: 指针 -> VirtualCamera
        // 拿到info
        //    CameraMetaInfo ui数据
        //    ├─ UserDefineID -> QString("VirtualCamera")
        //    ├─ Serial       -> QString("VIR-0001")
        //    └─ VenderName   -> QString("MyCameraClient")
        const QString displayText =
            QString("%1 (%2)")//这两个是第一个、第二个参数（字符串模板）
                .arg(cameraInfo.UserDefineID)
                .arg(cameraInfo.Serial);
        // cameraInfo.UserDefineID -> "VirtualCamera"
        // cameraInfo.Serial       -> "VIR-0001"
        // ->
        // "VirtualCamera (VIR-0001)"
        ui->cameraListWidget->addItem(displayText);
    }

    if (!m_cameraMetaInfos.isEmpty())
    {
        ui->cameraListWidget->setCurrentRow(0);
    }

    refreshConnectButtonState();
}

void ControlWidget::refreshConnectButtonState()
{
    const QString serial = currentCameraSerial();
    if (serial.isEmpty())
    {
        ui->connectButton->setText("Connect");
        ui->connectButton->setEnabled(false);
        return;
    }

    ui->connectButton->setEnabled(true);

    bool state = false;
    const auto ret = CameraContext::Instance()->isConnect(serial, state);
    if (ret != CHONGMING_OK)
    {
        ui->connectButton->setText("Connect");
        return;
    }

    ui->connectButton->setText(state ? "Disconnect" : "Connect");
}

QString ControlWidget::currentCameraSerial() const
{
    const int currentRow = ui->cameraListWidget->currentRow();
    //0: VirtualCamera (VIR-0001)
    //1: Camera2 (...)
    //currentRow=0
    if (currentRow < 0 || currentRow >= m_cameraMetaInfos.size())
    {
        return {};
    }
    return m_cameraMetaInfos.at(currentRow).Serial;
    //等价m_cameraMetaInfos[row] 得到CameraMetaInfo 对象 有三个参数，get到serial
}

void ControlWidget::onEnumerateClicked()
{
    m_cameraMetaInfos.clear();
    const auto ret = CameraContext::Instance()->EnumerationCamera(m_cameraMetaInfos);
    // CameraContext::Instance()是static 不需要通过类对象调用 属于类本身
    // 返回一个CameraContext*  m_pContext -> cameracontext类对象可以调用cameracontext类成员函数
    // 堆：
    // [ VirtualCamera ]
    // map：
    // key: "VIR-0001"
    // value: 指针 -> VirtualCamera
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Enumeration failed");
        return;
    }

    refreshCameraList();
    ui->statusLabel->setText(
        QString("Found %1 camera(s)").arg(m_cameraMetaInfos.size()));
    //.arg() = 把字符串里的占位符（%1 %2 ...）替换成你传进去的值
}

void ControlWidget::onConnectClicked()
{
    const QString serial = currentCameraSerial();//serial是virtualcamera的serial
    if (serial.isEmpty())
    {
        ui->statusLabel->setText("Please enumerate and select a camera first");
        return;
    }

    bool isConnected = false;
    auto ret = CameraContext::Instance()->isConnect(serial, isConnected);
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Camera not found");
        refreshConnectButtonState();
        return;
    }

    if (isConnected)
    {
        ret = CameraContext::Instance()->disconnect(serial);
        if (ret != CHONGMING_OK)
        {
            ui->statusLabel->setText("Disconnect failed");
            return;
        }

        ui->statusLabel->setText(QString("Disconnected: %1").arg(serial));
        refreshConnectButtonState();
        return;
    }

    ret = CameraContext::Instance()->connect(serial);
    //这里先调用CameraContext::connect(serial)
    //ret= return的 camera->connect() 也就是virtualcamera::connect()

    //VirtualCamera::connect()
    // vptr -> vtable（VirtualCamera）
    // vtable:
    // connect -> VirtualCamera::connect
    // disconnect -> VirtualCamera::disconnect
    //堆:
    // [ VirtualCamera对象 ]
    // ├──────────────────────
    // │ vptr -> vtable       │
    // ├──────────────────────
    // │ CameraInterface成员   │
    // ├──────────────────────
    // │ VirtualCamera成员     │
    // └──────────────────────
    if (ret != CHONGMING_OK)
    {
        ui->statusLabel->setText("Connect failed");
        return;
    }

    ui->statusLabel->setText(QString("Connected: %1").arg(serial));
    refreshConnectButtonState();
}
