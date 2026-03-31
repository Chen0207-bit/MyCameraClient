#include "CameraParamModel.h"

#include "CameraParamItem.h"
#include "CameraInterface/CameraParam.h"

CameraParamModel::CameraParamModel(const QStringList& headers, QObject* parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    for (const auto& header : headers)
    {
        rootData.push_back(header);
    }

    m_rootItem = new CameraParamItem(rootData);
}

CameraParamModel::~CameraParamModel()
{
    delete m_rootItem;
}

QModelIndex CameraParamModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

    CameraParamItem* parentItem = getItem(parent);
    CameraParamItem* childItem = parentItem->child(row);
    if (childItem == nullptr)
    {
        return {};
    }

    return createIndex(row, column, childItem);
}

QModelIndex CameraParamModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return {};
    }

    CameraParamItem* childItem = getItem(index);
    CameraParamItem* parentItem = childItem == nullptr ? nullptr : childItem->parentItem();
    if (parentItem == nullptr || parentItem == m_rootItem)
    {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int CameraParamModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    return getItem(parent)->childCount();
}

int CameraParamModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_rootItem->columnCount();
}

QVariant CameraParamModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    CameraParamItem* item = getItem(index);
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return item->data(index.column());
    }
    //widgetui把item显示
    if (role == Qt::UserRole && item->hasCameraParam())
    {
        return QVariant::fromValue(item->cameraParam());
    }

    return {};
}

QVariant CameraParamModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_rootItem->data(section);
    }

    return {};
}

Qt::ItemFlags CameraParamModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    CameraParamItem* item = getItem(index);
    Qt::ItemFlags defaultFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (item->hasCameraParam() && index.column() == 1)
    {
        defaultFlags |= Qt::ItemIsEditable;
    }

    return defaultFlags;
}

bool CameraParamModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
    {
        return false;
    }

    CameraParamItem* item = getItem(index);
    if (item == nullptr || !item->hasCameraParam() || index.column() != 1)
    {
        return false;
    }

    if (!item->setData(index.column(), value))
    {
        return false;
    }

    CameraParam param = item->cameraParam();
    param.Value = value.toString();
    item->setCameraParam(param);

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole, Qt::UserRole});
    emit SigValueChanged(index);
    return true;
}

CameraParamItem* CameraParamModel::getItem(const QModelIndex& index) const
{
    if (index.isValid())
    {
        auto* item = static_cast<CameraParamItem*>(index.internalPointer());
        if (item != nullptr)
        {
            return item;
        }
    }

    return m_rootItem;
}

void CameraParamModel::addCameraParam(const CameraParam& param)
{
    CameraParamItem* groupItem = m_groups.value(param.Group, nullptr);
    if (groupItem == nullptr)
    {
        QVector<QVariant> groupData;
        groupData << param.Group << QString{};

        beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount());
        groupItem = new CameraParamItem(groupData, m_rootItem);
        m_rootItem->appendChild(groupItem);
        endInsertRows();

        m_groups.insert(param.Group, groupItem);
    }

    const QModelIndex groupIndex = createIndex(groupItem->row(), 0, groupItem);
    const int newRow = groupItem->childCount();
    beginInsertRows(groupIndex, newRow, newRow);

    QVector<QVariant> itemData;
    itemData << param.Name << param.Value;
    auto* childItem = new CameraParamItem(itemData, groupItem);
    childItem->setCameraParam(param);
    groupItem->appendChild(childItem);
    //挂节点 创建树结构
    endInsertRows();
}

void CameraParamModel::clear()
{
    beginResetModel();
    delete m_rootItem;
    QVector<QVariant> rootData;
    rootData << "Param" << "Value";
    m_rootItem = new CameraParamItem(rootData);
    m_groups.clear();
    endResetModel();
}
