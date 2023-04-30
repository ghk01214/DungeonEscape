#include "pch.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Player.h"

ImplementSingletone(MessageHandler);

MessageHandler::MessageHandler()
{
}

MessageHandler::~MessageHandler()
{
    Release();
}

void MessageHandler::Init()
{
}

void MessageHandler::Release()
{
	while (!m_messageQueue.empty())
    {
        m_messageQueue.pop();
	}
}

void MessageHandler::InsertMessage(uint32_t playerID, MessageType msgType)
{
    Message msg = Message(playerID, msgType);
    m_messageQueue.push(msg);
}

void MessageHandler::ExecuteMessage()
{
    if (m_messageQueue.empty())
        return;

    auto objMgr = ObjectManager::GetInstance();

    while (!m_messageQueue.empty())
    {
        Message msg = m_messageQueue.front();

        switch (msg.msgType)
        {
            case LOGIN:
            objMgr->AddGameObjectToLayer<Player>(L"Layer_Player", msg.id, Vec3(5, 10, -10), Quat(0, 0, 0, 1), Vec3(0.5, 0.5, 0.5));
            //피직스 시뮬레이션에서 오브젝트를 추가하는거랑 sesssion의 명령 두개가 아주 약간의 시간차는 둬도 되지않나?
            //상관없으면 저 두 라인은 server.cpp에 그대로 냅두는것도 괜찮을듯. 일단 냄겨둠
            //session->SetObject(player);           
            //session->SendLoginPacket(player);   
            break;                                
           

            case LOGOUT:
            // actual orders
            break;
        }


        m_messageQueue.pop();
    }
}
