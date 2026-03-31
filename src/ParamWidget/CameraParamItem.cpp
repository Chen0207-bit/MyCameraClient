#include "CameraParamItem.h"

CameraParamItem::CameraParamItem(const QVector<QVariant>& data, CameraParamItem* parentItem)
    : m_itemData(data)
    , m_parentItem(parentItem)
    , m_hasCameraParam(false)
{
}

CameraParamItem::~CameraParamItem()
{
    qDeleteAll(m_childItems);
}

void CameraParamItem::appendChild(CameraParamItem* child)
{
    m_childItems.push_back(child);
}

CameraParamItem* CameraParamItem::child(int row) const
{
    if (row < 0 || row >= m_childItems.size())
    {
        return nullptr;
    }

    return m_childItems.at(row);
}

int CameraParamItem::childCount() const
{
    return m_childItems.size();
}

int CameraParamItem::columnCount() const
{
    return m_itemData.size();
}

QVariant CameraParamItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
    {
        return {};
    }

    return m_itemData.at(column);
}

bool CameraParamItem::setData(int column, const QVariant& value)
{
    if (column < 0 || column >= m_itemData.size())
    {
        return false;
    }

    m_itemData[column] = value;
    return true;
}

int CameraParamItem::row() const
{
    if (m_parentItem == nullptr)
    {
        return 0;
    }

    for (int index = 0; index < m_parentItem->m_childItems.size(); ++index)
    {
        if (m_parentItem->m_childItems.at(index) == this)
        {
            return index;
        }
    }

    return 0;
}

CameraParamItem* CameraParamItem::parentItem() const
{
    return m_parentItem;
}

void CameraParamItem::setCameraParam(const CameraParam& param)
{
    m_cameraParam = param;
    m_hasCameraParam = true;
}

CameraParam CameraParamItem::cameraParam() const
{
    return m_cameraParam;
}

bool CameraParamItem::hasCameraParam() const
{
    return m_hasCameraParam;
}
