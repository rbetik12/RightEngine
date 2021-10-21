#pragma once

#include <Window.hpp>
#include <Application.hpp>
#include <gtest/gtest.h>
#include "Core.h"

extern bool GShouldStop;

class GameApplication
{
public:
    static void OnStart();
    static void OnUpdate();
    static void OnDestroy();
};

int main()
{
    ::testing::InitGoogleTest();
//    RUN_ALL_TESTS();
    RightEngine::LaunchEngine::Init();
    GameApplication::OnStart();
    RightEngine::Application* application = RightEngine::Application::Get();

    while (!GShouldStop)
    {
        application->OnUpdate();
        GameApplication::OnUpdate();
        application->OnUpdateEnd();
    }
    GameApplication::OnDestroy();
    RightEngine::LaunchEngine::Exit();
    return 0;
}