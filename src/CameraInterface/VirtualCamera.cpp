#include "VirtualCamera.h"

VirtualCamera::VirtualCamera(const CameraMetaInfo& info)
    : CameraInterface(info)
    , m_isConnected(false)
    , m_isGrabbing(false)
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
