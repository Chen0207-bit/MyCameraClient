#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

class ControlWidget;
class ViewWidget;
class ParamWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ControlWidget *m_controlWidget;
    ViewWidget *m_viewWidget;
    ParamWidget *m_paramWidget;
    QLabel *m_statusLabel;
};
#endif // MAINWINDOW_H
