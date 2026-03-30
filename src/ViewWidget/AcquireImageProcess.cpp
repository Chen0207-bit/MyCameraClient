#include "AcquireImageProcess.h"

#include "CameraInterface/CameraContext.h"

AcquireImageProcess::AcquireImageProcess(QObject *parent)
    : QThread(parent)
    , m_running(false)
{
}

void AcquireImageProcess::setSerial(const QString& serial)
{
    m_serial = serial;
}

void AcquireImageProcess::stop()
{
    m_running = false;
}

void AcquireImageProcess::run()
{
    m_running = true;

    while (m_running)
    {
        QImage image;
        const auto ret = CameraContext::Instance()->getImage(m_serial, image);
        if (ret == CHONGMING_OK)
        {
            emit imageReady(image);
            msleep(120);
            continue;
        }

        msleep(40);
    }
}
