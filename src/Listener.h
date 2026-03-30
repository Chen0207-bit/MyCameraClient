#ifndef LISTENER_H
#define LISTENER_H

#include <QVector>

enum MESSAGE
{
    CAMERA_ENUMERATION = 1 << 0,
    CAMERA_SWITCH = 1 << 1,
    CAMERA_CONNECT = 1 << 2,
    CAMERA_DISCONNECT = 1 << 3,
    CAMERA_STARTGRAB = 1 << 4,
    CAMERA_STOPGRAB = 1 << 5
};

class Listener
{
public:
    virtual ~Listener() = default;
    virtual void RespondMessage(int message) = 0;
};

class ListenerManager
{
public:
    static ListenerManager* Instance();

    void registerMessage(int messageMask, Listener* listener);
    void unregisterListener(Listener* listener);
    void notify(int message);

private:
    ListenerManager() = default;

private:
    struct ListenerNode
    {
        int messageMask;
        Listener* listener;
    };

    static ListenerManager* m_instance;
    QVector<ListenerNode> m_listenerNodes;
};

#endif // LISTENER_H
