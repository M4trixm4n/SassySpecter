#include "DummyBot.h"

DummyBot::DummyBot()
{
    
}

void DummyBot::OnGameStart() 
{

}

void DummyBot::OnStep()
{
    /*for (auto &mess : Observation()->GetChatMessages()) {
        std::cout << "dummy Message : " << mess.message << std::endl;
        Actions()->SendChat(mess.message);
        if (mess.message == "gg") std::cout << "message détecté\n";
    }*/
}