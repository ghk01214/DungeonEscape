#include "pch.h"
#include "EventHandler.h"
#include "Event.h"

ImplementSingletone(EventHandler);

EventHandler::EventHandler()
{
}

void EventHandler::Init()
{
}

void EventHandler::Update(double timeDelta)
{
	//for (auto& event : m_events)
	//{
	//	event->Tick(timeDelta);
	//}

    for (int i = 0; i < m_events.size(); ++i)
    {
        m_events[i]->Tick(timeDelta);
    }
}

void EventHandler::LateUpdate(double timeDelta)
{
    for (auto it = m_events.begin(); it != m_events.end();)
    {
        // 객체 삭제 플래그값 확인
        if ((*it)->executed)
        {
            // 오브젝트 삭제
            delete* it;

            // 메인 컨테이너에 삭제된 오브젝트들을 제외시킨다.
            it = m_events.erase(it);
        }
        else
        {
            // 삭제가 진행되지 않았다면 다음 오브젝트로 진행
            ++it;
        }
    }
}

void EventHandler::Release()
{
    for (auto& event : m_events)
    {
        delete event;
    }
    m_events.clear();
}

void EventHandler::AddEvent(std::string context, float remainTime, GameObject* subject)
{
	Event* event = new Event(context, remainTime, subject);
	m_events.emplace_back(event);
}

Event* EventHandler::GetEvent(std::string context)
{
    for (auto& event : m_events)
    {
        if (event->msg == context)
        {
            return event;
        }
    }

    return nullptr;
}

void EventHandler::DeleteEvent(std::string context)
{
    for (auto& event : m_events)
    {
        if (event->msg == context)
        {
            event->executed = true;
            event->time = -1.f;
        }
    }
}

