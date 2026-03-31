#ifndef HIKCAMERA_H
#define HIKCAMERA_H

#include "CameraInterface.h"

class HikCamera : public CameraInterface
{
public:
    explicit HikCamera(const CameraMetaInfo& info);

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
};

#endif // HIKCAMERA_H
