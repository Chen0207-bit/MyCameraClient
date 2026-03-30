#include "CameraContext.h"

#include "CameraError.h"
#include "CameraInterface.h"
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
    qDeleteAll(m_serialCamMap);//面向操作逻辑的map
    // [对象成员区 / 栈上宿主对象里]
    //     m_serialCamMap
    // │
    // ├── 节点1: key="A001", value=0x1000   ← 还在
    // └── 节点2: key="B002", value=0x2000   ← 还在

    // [堆区]
    // 0x1000 ── 原来的Camera对象A 已经被释放
    // 0x2000 ── 原来的Camera对象B 已经被释放
    m_serialCamMap.clear();
    //把map中的键值对清空
    cameraInfos.clear();
    //面向gui的vector
    CameraMetaInfo info;
    info.UserDefineID = "VirtualCamera";
    info.Serial = "VIR-0001";
    info.VenderName = "MyCameraClient";
    CameraMetaInfo info2;
    info2.UserDefineID = "VirtualCamera2";
    info2.Serial = "VIR-0002";
    info2.VenderName = "MyCameraClient";
    cameraInfos.push_back(info);//给外部看
    cameraInfos.push_back(info2);
    //cameraInfos[0]会得到camerametainfo对象
    // struct CameraMetaInfo
    // {
    //     QString UserDefineID;
    //     QString Serial;
    //     QString VenderName;
    // };
    m_serialCamMap.insert(info.Serial, new VirtualCamera(info));//给内部看
    m_serialCamMap.insert(info2.Serial, new VirtualCamera(info2));
    //变量类型：CameraInterface*
    //实际对象：VirtualCamera
    // 堆：
    // 对象[ VirtualCamera ]
    // map：
    // key: "VIR-0001"
    // value: 指针 → VirtualCamera

    //父类指针能指向子类对象
    //= 子类对象“包含”一个完整的父类部分

    //     [ VirtualCamera对象 ]

    // ┌──────────────────────┐
    // │ CameraInterface部分   │  ← 父类子对象（完整存在）
    // ├──────────────────────┤
    // │ VirtualCamera自己成员 │
    // └──────────────────────┘
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

uint32_t CameraContext::getImage(const QString& serial, QImage& image)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);
    return camera->getImage(image);
}

uint32_t CameraContext::connect(const QString& serial)
{
    if (!m_serialCamMap.contains(serial))
    {
        return NOCAMERA_ERROR;
    }

    auto* camera = m_serialCamMap.value(serial);//camera → VirtualCamera*
    auto ret = camera->acquire();//执行camera对象的acquire
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
    auto ret = camera->disconnect();
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
