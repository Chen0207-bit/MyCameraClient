#ifndef ACQUIREIMAGEPROCESS_H
#define ACQUIREIMAGEPROCESS_H

#include <QImage>
#include <QString>
#include <QThread>

class AcquireImageProcess : public QThread
{
    Q_OBJECT

public:
    explicit AcquireImageProcess(QObject *parent = nullptr);

    void setSerial(const QString& serial);
    void stop();

signals:
    void imageReady(const QImage& image);

protected:
    void run() override;

private:
    QString m_serial;
    bool m_running;
};

#endif // ACQUIREIMAGEPROCESS_H
