#include "HikCamera.h"

HikCamera::HikCamera(const CameraMetaInfo& info)
    : CameraInterface(info)
    , m_isConnected(false)
    , m_isGrabbing(false)
{
}

bool HikCamera::isConnect()
{
    return m_isConnected;
}

bool HikCamera::isGrabbing()
{
    return m_isGrabbing;
}

uint32_t HikCamera::acquire()
{
    return SDK_NOT_AVAILABLE;
}

uint32_t HikCamera::release()
{
    m_isConnected = false;
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t HikCamera::connect()
{
    return SDK_NOT_AVAILABLE;
}

uint32_t HikCamera::disconnect()
{
    m_isConnected = false;
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t HikCamera::startGrabbing()
{
    return SDK_NOT_AVAILABLE;
}

uint32_t HikCamera::stopGrabbing()
{
    m_isGrabbing = false;
    return CHONGMING_OK;
}

uint32_t HikCamera::getParamList(QVector<CameraParam>& paramList)
{
    paramList.clear();
    return SDK_NOT_AVAILABLE;
}

uint32_t HikCamera::writeParam(const CameraParam&)
{
    return SDK_NOT_AVAILABLE;
}

uint32_t HikCamera::getImage(QImage& image)
{
    image = QImage();
    return SDK_NOT_AVAILABLE;
}
