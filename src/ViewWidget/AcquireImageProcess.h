#ifndef ACQUIREIMAGEPROCESS_H
#define ACQUIREIMAGEPROCESS_H

#include <QImage>
#include <QMutex>
#include <QString>
#include <QThread>

#include <atomic>

class AcquireImageProcess : public QThread
{
    Q_OBJECT

public:
    explicit AcquireImageProcess(QObject *parent = nullptr);

    void setSerial(const QString& serial);
    void stop();

signals:
    void imageReady(const QString& serial, const QImage& image);

protected:
    void run() override;

private:
    QString snapshotSerial();

private:
    QString m_serial;
    mutable QMutex m_mutex;
    std::atomic_bool m_running;
};

#endif // ACQUIREIMAGEPROCESS_H
