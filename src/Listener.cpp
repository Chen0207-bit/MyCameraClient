#include "Listener.h"

ListenerManager* ListenerManager::m_instance = nullptr;

ListenerManager* ListenerManager::Instance()
{
    if (m_instance == nullptr)
    {
        m_instance = new ListenerManager();
    }

    return m_instance;
}

void ListenerManager::registerMessage(int messageMask, Listener* listener)
{
    if (listener == nullptr)
    {
        return;
    }

    for (const auto& node : m_listenerNodes)
    {
        if (node.listener == listener && node.messageMask == messageMask)
        {
            return;
        }
    }

    m_listenerNodes.push_back({messageMask, listener});
}

void ListenerManager::unregisterListener(Listener* listener)
{
    if (listener == nullptr)
    {
        return;
    }

    for (int index = m_listenerNodes.size() - 1; index >= 0; --index)
    {
        if (m_listenerNodes[index].listener == listener)
        {
            m_listenerNodes.removeAt(index);
        }
    }
}

void ListenerManager::notify(int message)
{
    for (const auto& node : m_listenerNodes)
    {
        if (node.listener == nullptr)
        {
            continue;
        }

        if ((node.messageMask & message) != 0)
        {
            node.listener->RespondMessage(message);
        }
    }
}
