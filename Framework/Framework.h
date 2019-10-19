#pragma once
#include"pch.h"
#include"OnlineElement.h"
#include"OfflineElement.h"
#include"DrawableElement.h"

#include"Managers/Timer.h"
#include"Utils/Utils.h"

namespace GameEngine {
	class Framework{
		std::vector<OnlineElement*>m_onlineElements;
		std::vector<OfflineElement*>m_offlineElements;
		std::vector<DrawableElement*>m_drawableElements;

		Framework() { Utils::GetInstance()->Log("FRAMEWORK","Hello Framework"); }
		~Framework() { Utils::GetInstance()->Log("FRAMEWORK", "Goodbye Framework"); }

		void Init();
		void Update(float deltaTime);
		void Draw();
		void Cleanup();
		friend class Application;

		static Framework* s_instance;



		Timer m_timer;
	public:
		static Framework* GetInstance();

		Timer& GetTimer() { return m_timer; }
	};
}
