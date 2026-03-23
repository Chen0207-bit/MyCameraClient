#include "paramwidget.h"
#include "ui_paramwidget.h"

ParamWidget::ParamWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParamWidget)
{
    ui->setupUi(this);
}

ParamWidget::~ParamWidget()
{
    delete ui;
}
