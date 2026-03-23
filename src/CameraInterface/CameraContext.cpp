#include "CameraContext.h"

#include "CameraError.h"
#include "CameraInterface.h"

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
    Q_UNUSED(cameraInfos);
    return CHONGMING_OK;
}

uint32_t CameraContext::connect(const QString& serial)
{
    Q_UNUSED(serial);
    return CHONGMING_OK;
}

uint32_t CameraContext::disconnect(const QString& serial)
{
    Q_UNUSED(serial);
    return CHONGMING_OK;
}
