#include"pch.h"
#include"Framework.h"

void GameEngine::Framework::Init()
{
	//this is where all the systems are setup

	m_onlineElements.push_back(&m_timer);

	for (auto& a : m_onlineElements) a->Init();
	for (auto& a : m_offlineElements) a->Init();
	for (auto& a : m_drawableElements) a->Init();
}

void GameEngine::Framework::Update(float deltaTime)
{
	for (auto& a : m_onlineElements) a->Update(deltaTime);
	for (auto& a : m_drawableElements) a->Update(deltaTime);
}

void GameEngine::Framework::Draw()
{
	for (auto& a : m_drawableElements) a->Draw();
}

void GameEngine::Framework::Cleanup()
{
	for (auto& a : m_onlineElements) a->Cleanup();
	for (auto& a : m_offlineElements) a->Cleanup();
	for (auto& a : m_drawableElements) a->Cleanup();
	delete s_instance;
}

GameEngine::Framework* GameEngine::Framework::s_instance = 0;
GameEngine::Framework* GameEngine::Framework::GetInstance()
{
	if (s_instance == 0)
		s_instance = new GameEngine::Framework();
	return s_instance;
}
