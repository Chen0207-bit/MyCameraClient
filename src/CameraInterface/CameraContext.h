#ifndef CAMERACONTEXT_H
#define CAMERACONTEXT_H

#include <QMap>
#include <QString>
#include <QVector>

#include "CMCameraMetaInfo.h"
#include "CameraError.h"

class CameraInterface;

class CameraContext
{
public:
    static CameraContext* Instance();
    static void Release();

    uint32_t EnumerationCamera(QVector<CameraMetaInfo>& cameraInfos);
    uint32_t isConnect(const QString& serial, bool& state);
    uint32_t connect(const QString& serial);
    uint32_t disconnect(const QString& serial);

private:
    CameraContext();
    ~CameraContext();

private:
    static CameraContext* m_pContext;
    QMap<QString, CameraInterface*> m_serialCamMap;//序列号A -> 某个相机对象
};

#endif // CAMERACONTEXT_H
