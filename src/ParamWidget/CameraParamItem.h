#ifndef CAMERAPARAMITEM_H
#define CAMERAPARAMITEM_H

#include "CameraInterface/CameraParam.h"

#include <QVariant>
#include <QVector>

class CameraParamItem
{
public:
    explicit CameraParamItem(const QVector<QVariant>& data, CameraParamItem* parentItem = nullptr);
    ~CameraParamItem();

    void appendChild(CameraParamItem* child);
    CameraParamItem* child(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);
    int row() const;
    CameraParamItem* parentItem() const;

    void setCameraParam(const CameraParam& param);
    CameraParam cameraParam() const;
    bool hasCameraParam() const;

private:
    QVector<CameraParamItem*> m_childItems;
    QVector<QVariant> m_itemData;
    CameraParamItem* m_parentItem;
    CameraParam m_cameraParam;
    bool m_hasCameraParam;
};

#endif // CAMERAPARAMITEM_H
