#pragma once

// Includes
#include "VectorMap.h"

#define PROFILING
#define LOGGING

namespace Reflex
{
	// Logging system
	inline void LOG( const std::string& log ) { std::cout << log << "\n"; }

#ifdef LOGGING
	#define LOG_CRIT( x ) { Reflex::SetConsoleTextAttributes textColour( FOREGROUND_RED ); Reflex::LOG( Stream( "CRIT: (" << __FUNCTION__ << ", " << __LINE__ << ") " << x ) ); assert( false ); }
	#define LOG_WARN( x ) { Reflex::SetConsoleTextAttributes textColour( FOREGROUND_YELLOW ); Reflex::LOG( Stream( "Warning: (" << __FUNCTION__ << ", " << __LINE__ << ") " << x ) ); }
	#define LOG_INFO( x ) Reflex::LOG( Stream( "Info: " << x ) );
#else
	#define LOG_CRIT( x ) ((void)0);
	#define LOG_WARN( x ) ((void)0);
	#define LOG_INFO( x ) ((void)0);
#endif

	#define THROW( x ) throw std::runtime_error( Stream( "EXCEPTION: (" << __FUNCTION__ << ") " << x ) );

	// Profiling code
	namespace Core
	{
		class Profiler : sf::NonCopyable
		{
		public:
			static Profiler& GetProfiler();
			void StartProfile( const std::string& name );
			void EndProfile( const std::string& name );
			void FrameTick( const sf::Int64 frameTimeMS );
			void OutputResults( const std::string& file );

		protected:
			Profiler() { }

		private:
			struct ProfileData
			{
				sf::Clock timer;
				sf::Int64 currentFrame = 0;
				sf::Int64 shortestFrame = std::numeric_limits< int >::max();
				sf::Int64 longestFrame = 0;
				sf::Int64 totalDuration = 0;
				unsigned minHitCount = 1U;
				unsigned maxHitCount = 1U;
				unsigned currentHitCount = 1U;
				unsigned totalSamples = 0U;
			};

			sf::Int64 m_totalDuration = 0;

			Reflex::VectorMap< std::string, ProfileData > m_profileData;
			static std::unique_ptr< Profiler > s_profiler;
			static bool s_profilerEnabled;
		};

		class ScopedProfiler : sf::NonCopyable
		{
		public:
			ScopedProfiler( const std::string& name );
			~ScopedProfiler();

		private:
			const std::string m_profileName;
		};
	}

#ifdef PROFILING
	#define PROFILE Reflex::Core::ScopedProfiler profile( __FUNCTION__ );
	#define PROFILE_NAME( x ) Reflex::Core::ScopedProfiler profile##x( #x );
#else
	#define PROFILE ((void)0);
	#define PROFILE_NAME( x ) ((void)0);
#endif
}