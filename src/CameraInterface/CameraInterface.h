#ifndef CAMERAINTERFACE_H
#define CAMERAINTERFACE_H

#include "CMCameraMetaInfo.h"
#include "CameraError.h"

class CameraInterface
{
public:
    explicit CameraInterface(const CameraMetaInfo& info)
        : m_cameraInfo(info)
    {
    }

    virtual ~CameraInterface() = default;

    virtual bool isConnect() = 0;
    virtual bool isGrabbing() = 0;

    virtual uint32_t acquire() = 0;
    virtual uint32_t release() = 0;
    virtual uint32_t connect() = 0;
    virtual uint32_t disconnect() = 0;
    virtual uint32_t startGrabbing() = 0;
    virtual uint32_t stopGrabbing() = 0;

protected:
    CameraMetaInfo m_cameraInfo;
};

#endif // CAMERAINTERFACE_H
