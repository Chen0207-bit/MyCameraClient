#include "VirtualCamera.h"

VirtualCamera::VirtualCamera(const CameraMetaInfo& info)
    : CameraInterface(info)
    , m_isConnected(false)
    , m_isGrabbing(false)
{
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
