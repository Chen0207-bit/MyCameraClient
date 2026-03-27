#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ControlWidget/controlwidget.h"
#include "ViewWidget/viewwidget.h"
#include "ParamWidget/paramwidget.h"

#include <QLabel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controlWidget(nullptr)
    , m_viewWidget(nullptr)
    , m_paramWidget(nullptr)
    , m_statusLabel(nullptr)
{
    ui->setupUi(this);

    m_controlWidget = new ControlWidget(ui->controlWidgetContainer);
    m_viewWidget = new ViewWidget(m_controlWidget, ui->viewWidgetContainer);
    m_paramWidget = new ParamWidget(m_controlWidget, ui->paramWidgetContainer);
    m_statusLabel = new QLabel("Round 1 Ready", this);

    ui->controlWidgetContainer->layout()->addWidget(m_controlWidget);
    ui->viewWidgetContainer->layout()->addWidget(m_viewWidget);
    ui->paramWidgetContainer->layout()->addWidget(m_paramWidget);
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 5);
    ui->horizontalLayout->setStretch(2, 3);

    statusBar()->addWidget(m_statusLabel);

    resize(1200, 700);
    setWindowTitle("MyCameraClient");
}

MainWindow::~MainWindow()
{
    delete ui;
}
