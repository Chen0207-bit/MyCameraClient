#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include "CameraInterface/CameraParam.h"
#include "Listener.h"

#include <QItemSelection>
#include <QVector>
#include <QWidget>

class CameraParamDelegate;
class CameraParamModel;
class ControlWidget;

namespace Ui {
class ParamWidget;
}

class ParamWidget : public QWidget, public Listener
{
    Q_OBJECT

public:
    explicit ParamWidget(ControlWidget *controlWidget, QWidget *parent = nullptr);
    ~ParamWidget();
    void RespondMessage(int message) override;

public slots:
    void refreshInfoPanel();
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
    void onRefreshClicked();
    void writeCameraParam(const QModelIndex& index);

private:
    void refreshParamTree(const QVector<CameraParam>& paramList);
    void clearParamWidget();

private:
    Ui::ParamWidget *ui;
    ControlWidget *m_controlWidget;
    CameraParamModel *m_paramModel;
    CameraParamDelegate *m_paramDelegate;
};

#endif // PARAMWIDGET_H
