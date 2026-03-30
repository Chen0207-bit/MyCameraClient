#include "VirtualCamera.h"

#include <QDateTime>
#include <QPainter>

VirtualCamera::VirtualCamera(const CameraMetaInfo& info)
    : CameraInterface(info)
    , m_isConnected(false)
    , m_isGrabbing(false)
    , m_frameIndex(0)
{
    m_cameraParams.push_back({"ExposureTime", "12000 us", "Controls how long the virtual sensor accumulates light for each frame."});
    m_cameraParams.push_back({"Gain", "6 dB", "Amplifies the image signal. Higher gain brightens the image but can increase noise."});
    m_cameraParams.push_back({"FrameRate", "24 fps", "The target number of frames produced every second while grabbing is active."});
    m_cameraParams.push_back({"TriggerMode", "Off", "When Off, the camera grabs continuously. When On, it waits for an external trigger."});
}

bool VirtualCamera::isConnect()
{
    return m_isConnected;
}

bool VirtualCamera::isGrabbing()
{
    return m_isGrabbing;
}

uint32_t VirtualCamera::acquire()
{
    return CHONGMING_OK;
}

uint32_t VirtualCamera::release()
{
    m_isConnected = false;
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::connect()
{
    m_isConnected = true;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::disconnect()
{
    m_isConnected = false;
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::startGrabbing()
{
    if (!m_isConnected)
    {
        return CAMERA_NOT_CONNECTED;
    }

    m_isGrabbing = true;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::stopGrabbing()
{
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::getParamList(QVector<CameraParam>& paramList)
{
    paramList = m_cameraParams;
    return CHONGMING_OK;
}

uint32_t VirtualCamera::getImage(QImage& image)
{
    if (!m_isConnected)
    {
        return CAMERA_NOT_CONNECTED;
    }

    if (!m_isGrabbing)
    {
        return GETIMAGE_TIMEOUT;
    }

    image = QImage(640, 480, QImage::Format_RGB32);
    const int colorOffset = (m_frameIndex * 15) % 120;
    image.fill(QColor(30 + colorOffset, 60, 120 + colorOffset));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(20, 20, 600, 440);
    painter.drawLine(320, 20, 320, 460);
    painter.drawLine(20, 240, 620, 240);

    QFont titleFont = painter.font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(40, 40, 560, 40), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Virtual Camera Preview"));

    QFont bodyFont = painter.font();
    bodyFont.setPointSize(12);
    bodyFont.setBold(false);
    painter.setFont(bodyFont);
    painter.drawText(QRect(40, 100, 560, 28), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Camera: %1").arg(m_cameraInfo.UserDefineID));
    painter.drawText(QRect(40, 130, 560, 28), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Serial: %1").arg(m_cameraInfo.Serial));
    painter.drawText(QRect(40, 160, 560, 28), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Frame Index: %1").arg(m_frameIndex));
    painter.drawText(QRect(40, 190, 560, 28), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Timestamp: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));

    painter.setBrush(QColor(255, 220, 80));
    painter.drawEllipse(QPointF(320 + ((m_frameIndex % 40) - 20) * 6, 240), 24, 24);
    painter.end();

    ++m_frameIndex;
    return CHONGMING_OK;
}
