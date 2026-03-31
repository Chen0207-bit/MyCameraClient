#ifndef CMCAMERAMETAINFO_H
#define CMCAMERAMETAINFO_H

#include <QString>

enum class CameraBackend
{
    Virtual,
    Hik
};

struct CameraMetaInfo
{
    QString UserDefineID;
    QString Serial;
    QString VenderName;
    CameraBackend Backend = CameraBackend::Virtual;
};

#endif // CMCAMERAMETAINFO_H
