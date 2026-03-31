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
    uint32_t getParamList(QVector<CameraParam>& paramList) override;
    uint32_t writeParam(const CameraParam& param) override;
    uint32_t getImage(QImage& image) override;

private:
    bool m_isConnected;
    bool m_isGrabbing;
    int m_frameIndex;
    QVector<CameraParam> m_cameraParams;
};

#endif // VIRTUALCAMERA_H
