#include "cameraparamdelegate.h"

#include "CameraInterface/CameraParam.h"

#include <QComboBox>
#include <QLineEdit>

CameraParamDelegate::CameraParamDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

CameraParamDelegate::~CameraParamDelegate() = default;

QWidget* CameraParamDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&,
                                           const QModelIndex& index) const
{
    if (index.column() != 1)
    {
        return nullptr;
    }

    const CameraParam param = index.data(Qt::UserRole).value<CameraParam>();
    if (!param.Options.isEmpty())
    {
        auto* comboBox = new QComboBox(parent);
        comboBox->addItems(param.Options);
        return comboBox;
    }

    return new QLineEdit(parent);
}

void CameraParamDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    const QString currentValue = index.data(Qt::EditRole).toString();

    if (auto* comboBox = qobject_cast<QComboBox*>(editor))
    {
        const int comboIndex = comboBox->findText(currentValue);
        comboBox->setCurrentIndex(comboIndex >= 0 ? comboIndex : 0);
        return;
    }

    if (auto* lineEdit = qobject_cast<QLineEdit*>(editor))
    {
        lineEdit->setText(currentValue);
    }
}

void CameraParamDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                       const QModelIndex& index) const
{
    if (auto* comboBox = qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, comboBox->currentText(), Qt::EditRole);
        return;
    }

    if (auto* lineEdit = qobject_cast<QLineEdit*>(editor))
    {
        model->setData(index, lineEdit->text(), Qt::EditRole);
    }
}

void CameraParamDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                               const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}
