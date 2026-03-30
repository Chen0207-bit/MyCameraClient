#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include "CameraInterface/CameraParam.h"
#include "Listener.h"

#include <QVector>
#include <QWidget>

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

private slots:
    void onCurrentParamChanged();

private:
    void refreshParamList(const QVector<CameraParam>& paramList);

private:
    Ui::ParamWidget *ui;
    ControlWidget *m_controlWidget;
};

#endif // PARAMWIDGET_H
