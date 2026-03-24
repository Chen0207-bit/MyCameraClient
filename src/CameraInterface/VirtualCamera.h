#ifndef VIRTUALCAMERA_H
#define VIRTUALCAMERA_H

#include "CameraInterface.h"

class VirtualCamera : public CameraInterface
{
public:
    explicit VirtualCamera(const CameraMetaInfo& info);

    bool isConnect() override;
    bool isGrabbing() override;

    uint32_t acquire() override;
    uint32_t release() override;
    uint32_t connect() override;
    uint32_t disconnect() override;
    uint32_t startGrabbing() override;
    uint32_t stopGrabbing() override;

private:
    bool m_isConnected;
    bool m_isGrabbing;
};

#endif // VIRTUALCAMERA_H
