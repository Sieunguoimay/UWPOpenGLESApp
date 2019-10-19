#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
namespace GameEngine {
	class Application{
	public:

		void Init(int width, int height);
		void Update();
		void Draw();
		void Cleanup();

		void OnWindowSizeChanged(int width, int height);
		void HandleInputEvent();
	};
}