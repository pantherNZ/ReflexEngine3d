#pragma once

// Includes
#include "ResourceManager.h"
#include "StateManager.h"

#pragma once

#ifdef RF_BUILD_DLL
#define REFLEX_API __declspec(dllexport)
#else
#define REFLEX_API __declspec(dllimport)
#endif

//namespace Reflex
//{
//	class REFLEX_API Application
//	{
//	public:
//		Application();
//		virtual ~Application();
//
//		virtual void Run();
//		int GetExitCode() const;
//
//	protected:
//
//	private:
//	};
//
//	Application* CreateApplication();
//}

namespace Reflex::Core
{
	// Engine class
	class Engine : private sf::NonCopyable
	{
	public:
		struct EngineParams
		{
			std::string windowName = "Reflex Engine";

			// By default it will use window size as the resolution
			sf::VideoMode videoMode = sf::VideoMode::getFullscreenModes()[0];
			unsigned windowStyle = sf::Style::Default;
			bool fullscreen = false; 
			int fixedUpdatesPerSecond = 30;
			int fpsLimit = 60;
			bool enableProfiling = false;

			// Default will be window bounds
			sf::FloatRect worldBounds = sf::FloatRect( 0.0f, 0.0f, ( float )videoMode.width, ( float )videoMode.height );
			sf::Vector2f gravity = sf::Vector2f( 0.0f, 9.8f );

			// Command Line Mode: Don't create a window - this is used for the unit tests project
			bool cmdMode = false;
		};

		// Construct an engine instance
		explicit Engine( const std::string& windowName = "Reflex Engine", const bool fullscreen = false );
		explicit Engine( const std::string& windowName, const int screenWidth, const int screenHeight );
		explicit Engine( const EngineParams& params );

		// This constructor is intended for creating an engine without a window / UI (by passing false)
		explicit Engine( const bool createWindow, const int fixedUpdatesPerSecond = 30, const bool enableProfiling = false );

		~Engine();

		void Run();
		void Exit();

		template< typename T >
		void RegisterState( const bool isStartingState = false );

		sf::RenderWindow& GetWindow() { return m_window; }

	protected:
		void Setup();
		void KeyboardInput( const sf::Keyboard::Key key, const bool isPressed );
		void ProcessEvents();
		void Update( const float deltaTime );
		void Render();

		void UpdateStatistics( const int deltaTimeUS, const int frameTimeUS );

	protected:
		// Only used on construction (having this as a member provides access to the same defaults as the structure)
		EngineParams m_params;

		// Core window
		sf::RenderWindow m_window;

		// Resource managers
		TextureManager m_textureManager;
		FontManager m_fontManager;

		// Game world
		World m_world;

		// State manager (handles different scenes & transitions, contains worlds which hold objects etc.)
		StateManager m_stateManager;

		// Stats
		sf::String m_statisticsText;
		sf::Time m_statisticsUpdateTime;
		sf::Clock m_totalTime;

		struct Frame
		{
			int deltaTimeUS = 0;
			int frameTimeUS = 0;
		};

		int totalDeltaUS = 0;
		int totalTimeUS = 0;
		enum { NumSamples = 20 };
		Frame frames[NumSamples];
		int idx = -1;

		// ImGui
		bool m_showMetrics = false;
		bool m_showStyleEditor = false;
	};

	template< typename T >
	void Engine::RegisterState( const bool isStartingState )
	{
		m_stateManager.RegisterState< T >();

		if( isStartingState )
			m_stateManager.PushState< T >();
	}
}