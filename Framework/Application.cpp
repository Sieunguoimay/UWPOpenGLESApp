#include "pch.h"
#include "Application.h"
#include<stdio.h>
#include"Framework.h"
#include"Utils/Utils.h"
void GameEngine::Application::Init(int width, int height)
{
	Framework::GetInstance()->Init();
}

void GameEngine::Application::Update()
{
	Framework::GetInstance()->Update(Framework::GetInstance()->GetTimer().GetDeltaTime());
}

void GameEngine::Application::Draw()
{
	Framework::GetInstance()->Draw();
}

void GameEngine::Application::Cleanup()
{
	Framework::GetInstance()->Cleanup();
	Utils::GetInstance()->Cleanup();
}



void GameEngine::Application::OnWindowSizeChanged(int width, int height)
{
}

void GameEngine::Application::HandleInputEvent()
{
	printf("Some input event has occured\n");
}
