#include "AcquireImageProcess.h"

#include "CameraInterface/CameraContext.h"

#include <QMutexLocker>

AcquireImageProcess::AcquireImageProcess(QObject *parent)
    : QThread(parent)
    , m_running(false)
{
}

void AcquireImageProcess::setSerial(const QString& serial)
{
    QMutexLocker locker(&m_mutex);
    m_serial = serial;
}

void AcquireImageProcess::stop()
{
    m_running.store(false);
}

void AcquireImageProcess::run()
{
    m_running.store(true);

    while (m_running.load())
    {
        const QString serial = snapshotSerial();
        if (serial.isEmpty())
        {
            msleep(20);
            continue;
        }

        QImage image;
        const auto ret = CameraContext::Instance()->getImage(serial, image);
        if (ret == CHONGMING_OK)
        {
            emit imageReady(serial, image);
            msleep(120);
            continue;
        }

        msleep(40);
    }

    m_running.store(false);
}

QString AcquireImageProcess::snapshotSerial()
{
    QMutexLocker locker(&m_mutex);
    return m_serial;
}
