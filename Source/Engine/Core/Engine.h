#pragma once

#ifdef RF_BUILD_DLL
	#define REFLEX_API __declspec(dllexport)
#else
	#define REFLEX_API __declspec(dllimport)
#endif

namespace Reflex
{
	class REFLEX_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Run();
		int GetExitCode() const;

	protected:

	private:
	};

	Application* CreateApplication();
}