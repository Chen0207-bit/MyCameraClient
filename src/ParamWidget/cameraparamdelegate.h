#ifndef CAMERAPARAMDELEGATE_H
#define CAMERAPARAMDELEGATE_H

#include <QStyledItemDelegate>

class CameraParamDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CameraParamDelegate(QObject* parent = nullptr);
    ~CameraParamDelegate() override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;
};

#endif // CAMERAPARAMDELEGATE_H
