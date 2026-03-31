#include "CameraContext.h"

#include "CameraError.h"
#include "CameraInterface.h"
#include "HikCamera.h"
#include "VirtualCamera.h"

CameraContext* CameraContext::m_pContext = nullptr;

CameraContext* CameraContext::Instance()
{
    if (m_pContext == nullptr)
    {
        m_pContext = new CameraContext();
    }

    return m_pContext;
}

void CameraContext::Release()
{
    delete m_pContext;
    m_pContext = nullptr;
}

CameraContext::CameraContext()
{
}

CameraContext::~CameraContext()
{
    qDeleteAll(m_serialCamMap);
    m_serialCamMap.clear();
}

uint32_t CameraContext::EnumerationCamera(QVector<CameraMetaInfo>& cameraInfos)
{
    qDeleteAll(m_serialCamMap);
    m_serialCamMap.clear();
    cameraInfos.clear();

    appendVirtualCameras(cameraInfos);
    return CHONGMING_OK;
}

uint32_t CameraContext::isConnect(const QString& serial, bool& state)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    state = camera->isConnect();
    return CHONGMING_OK;
}

uint32_t CameraContext::isGrabbing(const QString& serial, bool& state)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    state = camera->isGrabbing();
    return CHONGMING_OK;
}

uint32_t CameraContext::getParamList(const QString& serial, QVector<CameraParam>& paramList)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    if (!camera->isConnect())
    {
        return CAMERA_NOT_CONNECTED;
    }

    return camera->getParamList(paramList);
}

uint32_t CameraContext::writeParam(const QString& serial, const CameraParam& param)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    if (!camera->isConnect())
    {
        return CAMERA_NOT_CONNECTED;
    }

    return camera->writeParam(param);
}

uint32_t CameraContext::getImage(const QString& serial, QImage& image)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    if (!camera->isConnect())
    {
        return CAMERA_NOT_CONNECTED;
    }

    return camera->getImage(image);
}

uint32_t CameraContext::connect(const QString& serial)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    const auto ret = camera->acquire();
    if (ret != CHONGMING_OK)
    {
        return ret;
    }

    return camera->connect();
}

uint32_t CameraContext::disconnect(const QString& serial)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    if (camera->isGrabbing())
    {
        const auto stopRet = camera->stopGrabbing();
        if (stopRet != CHONGMING_OK)
        {
            return stopRet;
        }
    }

    const auto ret = camera->disconnect();
    if (ret != CHONGMING_OK)
    {
        return ret;
    }

    return camera->release();
}

uint32_t CameraContext::startGrabbing(const QString& serial)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    return camera->startGrabbing();
}

uint32_t CameraContext::stopGrabbing(const QString& serial)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    return camera->stopGrabbing();
}

CameraInterface* CameraContext::createCamera(const CameraMetaInfo& info)
{
    switch (info.Backend)
    {
    case CameraBackend::Virtual:
        return new VirtualCamera(info);
    case CameraBackend::Hik:
        return new HikCamera(info);
    default:
        return nullptr;
    }
}

void CameraContext::appendVirtualCameras(QVector<CameraMetaInfo>& cameraInfos)
{
    CameraMetaInfo info;
    info.UserDefineID = "VirtualCamera";
    info.Serial = "VIR-0001";
    info.VenderName = "MyCameraClient";
    info.Backend = CameraBackend::Virtual;

    CameraMetaInfo info2;
    info2.UserDefineID = "VirtualCamera2";
    info2.Serial = "VIR-0002";
    info2.VenderName = "MyCameraClient";
    info2.Backend = CameraBackend::Virtual;

    cameraInfos.push_back(info);
    cameraInfos.push_back(info2);

    if (auto* camera = createCamera(info))
    {
        m_serialCamMap.insert(info.Serial, camera);
    }

    if (auto* camera = createCamera(info2))
    {
        m_serialCamMap.insert(info2.Serial, camera);
    }
}
