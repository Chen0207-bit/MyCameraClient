#ifndef CAMERAPARAMMODEL_H
#define CAMERAPARAMMODEL_H

#include <QAbstractItemModel>
#include <QMap>
#include <QStringList>

class CameraParamItem;
struct CameraParam;

class CameraParamModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CameraParamModel(const QStringList& headers, QObject* parent = nullptr);
    ~CameraParamModel() override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    CameraParamItem* getItem(const QModelIndex& index) const;
    void addCameraParam(const CameraParam& param);
    void clear();

signals:
    void SigValueChanged(const QModelIndex& index);

private:
    CameraParamItem* m_rootItem;
    QMap<QString, CameraParamItem*> m_groups;
};

#endif // CAMERAPARAMMODEL_H
