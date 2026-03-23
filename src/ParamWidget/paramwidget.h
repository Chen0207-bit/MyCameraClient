#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QWidget>

namespace Ui {
class ParamWidget;
}

class ParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParamWidget(QWidget *parent = nullptr);
    ~ParamWidget();

private:
    Ui::ParamWidget *ui;
};

#endif // PARAMWIDGET_H
