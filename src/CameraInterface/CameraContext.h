#ifndef CAMERACONTEXT_H
#define CAMERACONTEXT_H

#include <QMap>
#include <QImage>
#include <QString>
#include <QVector>

#include "CMCameraMetaInfo.h"
#include "CameraError.h"
#include "CameraParam.h"

class CameraInterface;

class CameraContext
{
public:
    static CameraContext* Instance();
    static void Release();

    uint32_t EnumerationCamera(QVector<CameraMetaInfo>& cameraInfos);
    uint32_t isConnect(const QString& serial, bool& state);
    uint32_t isGrabbing(const QString& serial, bool& state);
    uint32_t getParamList(const QString& serial, QVector<CameraParam>& paramList);
    uint32_t writeParam(const QString& serial, const CameraParam& param);
    uint32_t getImage(const QString& serial, QImage& image);
    uint32_t connect(const QString& serial);
    uint32_t disconnect(const QString& serial);
    uint32_t startGrabbing(const QString& serial);
    uint32_t stopGrabbing(const QString& serial);

private:
    CameraContext();
    ~CameraContext();
    CameraInterface* createCamera(const CameraMetaInfo& info);
    void appendVirtualCameras(QVector<CameraMetaInfo>& cameraInfos);

private:
    static CameraContext* m_pContext;
    QMap<QString, CameraInterface*> m_serialCamMap;//序列号A -> 某个相机对象
};

#endif // CAMERACONTEXT_H
