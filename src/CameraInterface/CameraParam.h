#ifndef CAMERAPARAM_H
#define CAMERAPARAM_H

#include <QString>
#include <QStringList>
#include <QMetaType>

struct CameraParam
{
    QString Group;
    QString Name;
    QString Value;
    QString Tips;
    QStringList Options;
};

Q_DECLARE_METATYPE(CameraParam)

#endif // CAMERAPARAM_H
