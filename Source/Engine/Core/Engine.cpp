// Includes
#include "Engine.h"

// Implementation
namespace Reflex::Core
{
	Engine::Engine( const std::string& windowName, const bool fullscreen )
		: m_world( Context( m_window, m_textureManager, m_fontManager ), m_params.worldBounds, m_params.gravity )
		, m_stateManager( m_world )
	{
		Setup();
	}

	Engine::Engine( const std::string& windowName, const int screenWidth, const int screenHeight )
		: m_world( Context( m_window, m_textureManager, m_fontManager ), m_params.worldBounds, m_params.gravity )
		, m_stateManager( m_world )
	{
		m_params.videoMode.width = screenWidth;
		m_params.videoMode.height = screenHeight;
		Setup();
	}

	Engine::Engine( const Engine::EngineParams& params )
		: m_params( params )
		, m_world( Context( m_window, m_textureManager, m_fontManager ), m_params.worldBounds, m_params.gravity )
		, m_stateManager( m_world )
	{
		Setup();
	}

	Engine::Engine( const bool createWindow, const int fixedUpdatesPerSecond, const bool enableProfiling )
		: m_world( Context( m_window, m_textureManager, m_fontManager ), m_params.worldBounds, m_params.gravity )
		, m_stateManager( m_world )
	{
		m_params.cmdMode = !createWindow;
		m_params.fixedUpdatesPerSecond = fixedUpdatesPerSecond;
		m_params.enableProfiling = enableProfiling;
		Setup();
	}


	void Engine::Setup()
	{
		srand( (unsigned )time( 0 ) );

		if( !m_params.cmdMode )
		{
			m_window.create( m_params.videoMode, m_params.windowName, m_params.windowStyle );
			m_window.setPosition( sf::Vector2i( -6, 0 ) );
			m_window.setVerticalSyncEnabled( false );

			ImGui::SFML::Init( m_window );
		}

		if( m_params.enableProfiling )
			Profiler::GetProfiler();
	}

	Engine::~Engine()
	{
		ImGui::SFML::Shutdown();
	}

	void Engine::Run()
	{
		m_totalTime.restart();

		try
		{
			sf::Clock clock;
			sf::Time accumlatedTime = sf::Time::Zero;

			while( m_params.cmdMode || m_window.isOpen() )
			{
				sf::Time deltaTime = std::min( clock.restart(), sf::seconds( 1.0f / 30.f ) );

#ifdef PROFILING
				if( m_params.enableProfiling )
					Profiler::GetProfiler().FrameTick( deltaTime.asMicroseconds() );
#endif

				accumlatedTime += deltaTime;
				unsigned counter = 0;
				const auto interval = sf::seconds( 1.0f / m_params.fixedUpdatesPerSecond );

				while( accumlatedTime > interval && ++counter < 10 )
				{
					accumlatedTime -= interval;
					ProcessEvents();
					Update( interval.asSeconds() );
				}

				if( !m_params.cmdMode )
				{
					ImGui::SFML::Update( m_window, deltaTime );
					Render();
					UpdateStatistics( ( int )deltaTime.asMicroseconds(), ( int )clock.getElapsedTime().asMicroseconds() );
				}

				const auto targetFPS = sf::seconds( 1.0f / m_params.fpsLimit );
				if( m_params.fpsLimit > 0 && clock.getElapsedTime() < targetFPS )
					sf::sleep( targetFPS - clock.getElapsedTime() );
			}

#ifdef PROFILING
			if( m_params.enableProfiling )
				Profiler::GetProfiler().OutputResults( "Performance_Results.txt" );
#endif
		}
		catch( std::exception& e )
		{
			LOG_CRIT( "*EXCEPTION: " << *e.what() );
			throw;
		}
	}

	void Engine::Exit()
	{
		m_params.cmdMode = false;

		if( m_window.isOpen() )
			m_window.close();
	}

	void Engine::ProcessEvents()
	{
		sf::Event processEvent;

		while( m_window.pollEvent( processEvent ) )
		{
			ImGui::SFML::ProcessEvent( processEvent );
			m_stateManager.ProcessEvent( processEvent );

			switch( processEvent.type )
			{
			case sf::Event::KeyPressed:
				KeyboardInput( processEvent.key.code, true );
				break;

			case sf::Event::KeyReleased:
				KeyboardInput( processEvent.key.code, false );
				break;

			case sf::Event::Closed:
				m_window.close();
				break;
			default: break;
			}
		}
	}

	void Engine::KeyboardInput( const sf::Keyboard::Key key, const bool isPressed )
	{
		if( key == sf::Keyboard::Escape )
			m_window.close();
	}

	void Engine::Update( const float deltaTime )
	{
		m_stateManager.Update( deltaTime );
	}

	void Engine::Render()
	{
		m_window.clear( sf::Color::Black );
		m_stateManager.Render();

		ImGui::SetNextWindowPos( sf::Vector2( 5.0f, 5.0f ), ImGuiCond_::ImGuiCond_Once );
		ImGui::SetNextWindowSize( sf::Vector2( 200.0f, 200.0f ), ImGuiCond_::ImGuiCond_Once );
		ImGui::Begin( "Engine Info" );
		ImGui::Text( m_statisticsText.toAnsiString().c_str() );
		ImGui::InputInt( "FPS Limit", &m_params.fpsLimit, 1, 10 );
		m_params.fpsLimit = std::max( m_params.fpsLimit, 0 );
		ImGui::InputInt( "Fixed Updates Per Second", &m_params.fixedUpdatesPerSecond, 1, 10 );
		m_params.fixedUpdatesPerSecond = Reflex::Clamp( m_params.fixedUpdatesPerSecond, 0, 240 );

		ImGui::Text( Stream( "Mouse Pos: " << ImGui::GetMousePos().x << ", " << ImGui::GetMousePos().y ).c_str() );

		ImGui::NewLine();

		ImGui::Checkbox( "Show ImGui Metrics", &m_showMetrics );
		ImGui::Checkbox( "Show ImGui Style Editor", &m_showStyleEditor );

		if( m_showMetrics )
			ImGui::ShowMetricsWindow();

		if( m_showStyleEditor ) 
		{ 
			ImGui::Begin( "Style Editor", &m_showStyleEditor );
			ImGui::ShowStyleEditor(); 
			ImGui::End(); 
		}

		ImGui::End();

		ImGui::SFML::Render( m_window );
		m_window.display();
	}

	void Engine::UpdateStatistics( const int deltaTimeUS, const int frameTimeUS )
	{
		if( idx == -1 )
		{
			idx = 0;
			frames[idx].deltaTimeUS = deltaTimeUS;
			frames[idx].frameTimeUS = frameTimeUS;
			totalDeltaUS += deltaTimeUS;
			totalTimeUS += frameTimeUS;
		}
		else
		{
			idx = ( idx + 1 ) % NumSamples;
			totalDeltaUS -= frames[idx].deltaTimeUS;
			totalTimeUS -= frames[idx].frameTimeUS;
			frames[idx].deltaTimeUS = deltaTimeUS;
			frames[idx].frameTimeUS = frameTimeUS;
			totalDeltaUS += deltaTimeUS;
			totalTimeUS += frameTimeUS;
		}

		m_statisticsUpdateTime += sf::microseconds( deltaTimeUS );
		const auto interval = sf::seconds( 1.0f );

		if( m_statisticsUpdateTime >= interval )
		{
			std::stringstream ss;
			ss << "FPS: " << std::to_string( int( 1.0f / ( totalDeltaUS / ( float )NumSamples / 1000.0f / 1000.0f ) ) ) << "\nFrame Time: ";
			
			const auto ms_per_frame = ( totalTimeUS / ( float )NumSamples ) / 1000.0f;
			if( ms_per_frame > 0 )
				ss << std::fixed << std::setprecision( 2 ) << ms_per_frame << "ms";
			else
				ss << int( totalTimeUS / ( float )NumSamples ) << "us";

			ss << "\nDuration: " << ( int )m_totalTime.getElapsedTime().asSeconds() << "s";

			m_statisticsText = ss.str();
			m_statisticsUpdateTime -= interval;
		}
	}
}