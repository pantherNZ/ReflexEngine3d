#pragma once

#include <typeindex>
#include <array>
#include <vector>

// Common Utility
namespace Reflex
{
	typedef std::type_index Type;
	typedef unsigned char ComponentFamily;
	constexpr ComponentFamily MaxComponents = 32;
	typedef std::bitset< MaxComponents > ComponentsMask;
	extern std::optional< float > box2DUnitToPixelScale;

	// Math common
	#define PI					3.141592654f
	#define PI2					6.283185307f
	#define PIDIV2				1.570796327f
	#define PIDIV4				0.785398163f
	#define SQRT2				1.41421356237f

	// String common
	#define STRINGIFY( x ) #x
	#define STRINGIFY2( x ) STRINGIFY( x )
	#define TODO( Msg ) __pragma( message( __FILE__ "(" STRINGIFY2( __LINE__ ) ") : TODO [ " Msg " ]" ) )
	#define Stream( message ) [&](){ std::stringstream s; s << message; return s.str(); }()

	// Helper functions
	constexpr inline int Mod( int a, int b )
	{
		return ( ( a %= b ) < 0 ) ? a + b : a;
	}

	template< typename T >
	T ToRadians( T deg )
	{
		return T( deg * PI / T( 180.0 ) );
	}

	template< typename T >
	T ToDegrees( T rad )
	{
		return T( rad / PI * T( 180.0 ) );
	}

	inline float Modf( float a, float b )
	{
		a = std::fmod( a, b );
		return ( a < 0 ) ? a + b : a;
	}

	inline int RandomInt( const int max )
	{
		return rand() % max;
	}

	inline int RandomInt( const int min, const int max )
	{
		if( min == max )
			return min;
		return min + ( RandomInt( max - min ) );
	}

	inline int RandomUnsigned( const unsigned max )
	{
		return rand() % max;
	}

	inline int RandomUnsigned( const unsigned min, const unsigned max )
	{
		return min + ( RandomUnsigned( max - min ) );
	}

	inline bool RandomBool()
	{
		return rand() > ( RAND_MAX / 2 );
	}

	inline float RandomFloat()
	{
		return rand() / ( RAND_MAX + 1.0f );
	}

	inline float RandomFloat( const float min, const float max )
	{
		return min + RandomFloat() * ( max - min );
	}

	inline float RandomFloat( const float max )
	{
		return RandomFloat( 0.0f, max );
	}

	inline float RandomAngle()
	{
		return RandomFloat( 0.0f, PI2 );
	}

	inline float Round( float value )
	{
		return std::round( value );
	}

	inline float Round( float value, int accuracy )
	{
		value *= accuracy;
		return std::round( value ) / ( float )accuracy;
	}

	inline int RoundToInt( float value )
	{
		return ( int )std::round( value );
	}

	template< typename T >
	constexpr inline T Clamp( const T& x, const T& min, const T& max )
	{
		return std::min( max, std::max( min, x ) );
	}

	template< typename T >
	constexpr inline T Clamp( const T& x )
	{
		return std::min( static_cast< T >( 1.0 ), std::max( static_cast< T >( 0.0 ), x ) );
	}

	template< typename T >
	constexpr inline T Sign( const T& x )
	{
		return T( ( double )x > 0.0f ? 1.0 : ( double )x < 0.0 ? -1.0f : 0.0f );
	}

	template< typename T >
	constexpr inline T Lerp( const T& a, const T& b, const float t )
	{
		return static_cast< T >( a + ( b - a ) * Clamp( t ) );
	}

	template< typename T >
	constexpr inline T InverseLerp( const T& a, const T& b, const float t )
	{
		return static_cast< T >( ( t - a ) / ( b - a ) );
	}

	// String functions
	std::vector< std::string > Split( const std::string& _strInput, const char _cLetter );

	constexpr inline bool IsSpace( const char c )
	{
		return c == ' ' || c == '\t';
	}

	// Trim from start (in place)
	void TrimLeft( std::string& str );

	// trim from end (in place)
	void TrimRight( std::string& str );

	// trim from both ends (in place)
	inline void Trim( std::string &str )
	{
		TrimLeft( str );
		TrimRight( str );
	}

	template< typename T >
	T FromString( const std::string& str )
	{
		T out;
		std::stringstream stream( str );
		stream >> out;
		return out;
	}

	template< typename T >
	void FromStringV( const std::string& str, T& t )
	{
		std::stringstream stream( str );
		stream >> t;
	}

	template< typename T, typename... Args >
	void FromStringV( const std::string& str, T& t, Args&... args )
	{
		FromStringV( str, t );
		FromStringV( str, args... );
	}

	template< typename T >
	std::string ToString( const T& t )
	{
		return Stream( t );
	}

	template< typename T, typename... Args >
	std::string ToString( const T& t, const Args&... args )
	{
		return ToString( t ) + ToString( args... );
	}

	template< typename T >
	bool IsDefault( const T& vec )
	{
		return vec == T{};
	}

	//inline sf::Color RandomColour( const bool randomAlpha = false, const int minMagnitude = 0, const int maxMagnitude = 255 )
	//{
	//	return sf::Color( 
	//		RandomInt( minMagnitude, maxMagnitude ),
	//		RandomInt( minMagnitude, maxMagnitude ),
	//		RandomInt( minMagnitude, maxMagnitude ),
	//		randomAlpha ? RandomInt( minMagnitude, maxMagnitude ) : 255 );
	//}
	//
	//inline sf::Color BlendColourLinear( const sf::Color& colourA, const sf::Color& colourB, const float percent = 0.5f )
	//{
	//	return sf::Color(
	//		Lerp( colourA.r, colourB.r, 0.5f ),
	//		Lerp( colourA.g, colourB.g, 0.5f ),
	//		Lerp( colourA.b, colourB.b, 0.5f ),
	//		Lerp( colourA.a, colourB.a, 0.5f ) );
	//}
	//
	//inline float BlendGammaCorrection( const float a, const float b, const float percent = 0.5f )
	//{
	//	const auto t = Clamp( percent );
	//	return sqrt( ( 1.0f - t ) * std::pow( a, 2.2f ) + t * std::pow( b, 2.2f ) );
	//}
	//
	//inline sf::Color BlendColourGammaCorrection( const sf::Color& colourA, const sf::Color& colourB, const float percent = 0.5f )
	//{
	//	return sf::Color(
	//		sf::Uint8( BlendGammaCorrection( colourA.r / 255.0f, colourB.r / 255.0f, 0.5f ) * 255 ),
	//		sf::Uint8( BlendGammaCorrection( colourA.g / 255.0f, colourB.g / 255.0f, 0.5f ) * 255 ),
	//		sf::Uint8( BlendGammaCorrection( colourA.b / 255.0f, colourB.b / 255.0f, 0.5f ) * 255 ),
	//		sf::Uint8( Lerp( colourA.a / 255.0f, colourB.a / 255.0f, 0.5f ) * 255 ) );
	//}
	//
	//inline sf::Color BlendColourDivideAlpha( const sf::Color& colourA, const sf::Color& colourB, const float percent = 0.5f )
	//{
	//	sf::Color r;
	//	const float Aa = colourA.a / 255.0f;
	//	const float Ba = colourB.a / 255.0f;
	//	const float newA = 1.0f - ( 1.0f - Ba ) * ( 1.0f - Aa );
	//	if( r.a < 1.0e-6 ) 
	//		return r;
	//	r.r = sf::Uint8( ( ( colourB.r / 255.0f ) * Ba / newA + ( colourA.r / 255.0f ) * Aa * ( 1 - Ba ) / newA ) * 255 );
	//	r.g = sf::Uint8( ( ( colourB.g / 255.0f ) * Ba / newA + ( colourA.g / 255.0f ) * Aa * ( 1 - Ba ) / newA ) * 255 );
	//	r.b = sf::Uint8( ( ( colourB.b / 255.0f ) * Ba / newA + ( colourA.b / 255.0f ) * Aa * ( 1 - Ba ) / newA ) * 255 );
	//	r.a = sf::Uint8( newA * 255 );
	//	return r;
	//}
	//
	//inline sf::Color ColourFromScalar( float t_val )
	//{
	//	sf::Color colour( 0, 0, 0, 255 );
	//	t_val *= 4.0f;
	//	if( ( 0 <= t_val && t_val <= 1 ) || ( 5 <= t_val && t_val <= 6 ) ) colour.r = 255;
	//	else if( 4 <= t_val && t_val <= 5 ) colour.r = sf::Uint8( 255 * ( t_val - 4 ) );
	//	else if( 1 <= t_val && t_val <= 2 ) colour.r = sf::Uint8( 255 * ( 1.0f - ( t_val - 1 ) ) );
	//	if( 1 <= t_val && t_val <= 3 ) colour.g = 255;
	//	else if( 0 <= t_val && t_val <= 1 ) colour.g = sf::Uint8( 255 * t_val );
	//	else if( 3 <= t_val && t_val <= 4 ) colour.g = sf::Uint8( 255 * ( 1.0f - ( t_val - 3 ) ) );
	//	if( 3 <= t_val && t_val <= 5 ) colour.b = 255;
	//	else if( 2 <= t_val && t_val <= 3 ) colour.b = sf::Uint8( 255 * ( t_val - 2 ) );
	//	else if( 5 <= t_val && t_val <= 6 ) colour.b = sf::Uint8( 255 * ( 1.0f - ( t_val - 5 ) ) );
	//	return colour;
	//}

	// Useful math functions
	//inline float Dot( const sf::Vector2f& a, const sf::Vector2f& b )
	//{
	//	return a.x * b.x + a.y * b.y;
	//}
	//
	//inline float GetMagnitudeSq( const sf::Vector2f& a )
	//{
	//	return float( a.x * a.x + a.y * a.y );
	//}
	//
	//inline float GetMagnitude( const sf::Vector2f& a )
	//{
	//	return sqrtf( GetMagnitudeSq( a ) );
	//}
	//
	//inline float GetDistanceSq( const sf::Vector2f& a, const sf::Vector2f& b )
	//{
	//	return float( GetMagnitudeSq( a - b ) );
	//}
	//
	//inline float GetDistance( const sf::Vector2f& a, const sf::Vector2f& b )
	//{
	//	return GetMagnitude( a - b );
	//}
	//
	//inline sf::Vector2f Normalise( sf::Vector2f& v )
	//{
	//	const auto magnitude = GetMagnitude( v );
	//	v = ( magnitude <= 0.0001f ? sf::Vector2f() : v / magnitude );
	//	return v;
	//}
	//
	//inline sf::Vector2f Normalise( const sf::Vector2f& v )
	//{
	//	const auto magnitude = GetMagnitude( v );
	//	return magnitude <= 0.0001f ? sf::Vector2f() : v / magnitude;
	//}
	//
	//inline sf::Vector2f Truncate( sf::Vector2f& v, const float maxMagnitude )
	//{
	//	const auto magnitude = GetMagnitude( v );
	//	v = magnitude > maxMagnitude ? Normalise( v ) * maxMagnitude : v;
	//	return v;
	//}
	//
	//inline sf::Vector2f Truncate( const sf::Vector2f& v, const float maxMagnitude )
	//{
	//	const auto magnitude = GetMagnitude( v );
	//	return magnitude > maxMagnitude ? Normalise( v ) * maxMagnitude : v;
	//}
	//
	//inline sf::Vector2f ScaleTo( sf::Vector2f& v, const float magnitude )
	//{
	//	Normalise( v );
	//	v *= magnitude;
	//	return v;
	//}
	//
	//inline sf::Vector2f ScaleTo( const sf::Vector2f& v, const float magnitude )
	//{
	//	return Normalise( v ) * magnitude;
	//}
	//
	//inline sf::Vector2f VectorFromAngle( const float angleRadians, const float distance )
	//{
	//	return sf::Vector2f( distance * sin( angleRadians ), distance * -cos( angleRadians ) );
	//}
	//
	//inline sf::Vector2f RotateVector( const sf::Vector2f& vector, const float angleRadians )
	//{
	//	const float sinR = sin( angleRadians );
	//	const float cosR = cos( angleRadians );
	//	return sf::Vector2f( vector.x * cosR - vector.y * sinR, vector.y * cosR + vector.x * sinR );
	//}
	//
	//inline sf::Vector2f RotateAroundPoint( const sf::Vector2f& position, const sf::Vector2f& rotateAround, const float angleDegrees )
	//{
	//	const auto angleRadians = Reflex::ToRadians( angleDegrees );
	//	const float sinR = sin( angleRadians );
	//	const float cosR = cos( angleRadians );
	//	const float diffX = position.x - rotateAround.x;
	//	const float diffY = position.y - rotateAround.y;
	//	return sf::Vector2f( cosR * diffX - sinR * diffY + rotateAround.x, sinR * diffX + cosR * diffY + rotateAround.y );
	//}
	//
	//inline sf::Vector2i RotateAroundPoint( const sf::Vector2i& position, const sf::Vector2i& rotateAround, const float angleDegrees )
	//{
	//	return Vector2fToVector2i( RotateAroundPoint( Vector2iToVector2f( position ), Vector2iToVector2f( rotateAround ), angleDegrees ) );
	//}
	//
	//// Returns a rotation from a vector in radians
	//inline float RotationFromVector( const sf::Vector2f& v )
	//{
	//	const auto n = Normalise( v );
	//	return atan2f( v.y, v.x );
	//}
	//
	//inline void ScaleTo( sf::Sprite& sprite, const sf::Vector2f& targetScale )
	//{
	//	sprite.setScale( sf::Vector2f( targetScale.x / ( float )sprite.getTextureRect().width, targetScale.y / ( float )sprite.getTextureRect().height ) );
	//}
	//
	//sf::Vector2f WrapAround( const sf::Vector2f& pos, const sf::FloatRect& bounds );
	//
	//bool IntersectPolygonCircle( const std::vector< sf::Vector2f >& polygon, const sf::Vector2f& circlePosition, const float radius );
	//bool IntersectPolygonCircle( const std::vector< sf::Vector2f >& polygon, const Circle& circle );
	//
	//bool IntersectLineLine( const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4 );
	//
	//bool IntersectPolygonLine( const std::vector< sf::Vector2f >& polygon, const sf::Vector2f& line_begin, const sf::Vector2f& line_end );
	//
	//bool IntersectLineCircle( const sf::Vector2f& line_begin, const sf::Vector2f& line_end, const sf::Vector2f& circle_position, const float circle_radius );
	//bool IntersectLineCircle( const sf::Vector2f& line_begin, const sf::Vector2f& line_end, const Circle& circle );
	//
	//bool IntersectCircleCircle( const sf::Vector2f& position1, const float size1, const sf::Vector2f& position2, const float size2 );
	//bool IntersectCircleCircle( const Circle& circle1, const Circle& circle2 );
	//
	//bool IntersectPolygonSquare( const std::vector< sf::Vector2f >& polygon, const sf::Vector2f& square_position, const float half_width );
	//
	//bool IntersectCircleSquare( const sf::Vector2f& circle_position, const float circle_radius, const sf::Vector2f& square_position, const float half_width );

	template< typename T >
	const typename T::value_type& RandomElement( const T& container )
	{
		if( container.empty() )
			throw std::runtime_error( "RandomElement called on empty container" );
		return container[RandomUnsigned( ( unsigned )container.size() )];
	}

	template< typename T >
	typename T::value_type& RandomElement( const T& container )
	{
		if( container.empty() )
			throw std::runtime_error( "RandomElement called on empty container" );
		return container[RandomUnsigned( ( unsigned )container.size() )];
	}

	template< typename T >
	typename std::vector<T>::const_iterator Erase( std::vector<T>& container, const T& value )
	{
		return container.erase( container.begin(), container.end(), value );
	}

	template< typename T, typename Pred >
	typename std::vector<T>::const_iterator EraseIf( std::vector<T>& container, const Pred& pred )
	{
		return container.erase( std::remove_if( container.begin(), container.end(), pred ), container.end() );
	}

	template< typename T >
	typename std::vector<T>::const_iterator Find( const std::vector<T>& container, const T& value )
	{
		return std::find( container.begin(), container.end(), value );
	}

	template< typename T, typename Pred >
	typename std::vector<T>::const_iterator FindIf( const std::vector<T>& container, const Pred& pred )
	{
		return std::find_if( container.begin(), container.end(), pred );
	}

	template< typename T >
	bool Contains( const std::vector< T >& container, const T& value )
	{
		return Find( container, value ) != container.end();
	}

	template< typename T, typename Pred >
	bool ContainsIf( const std::vector< T >& container, const Pred& pred )
	{
		return FindIf( container, pred ) != container.end();
	}

	template< typename T, typename... Args >
	bool PushBack( const std::vector< T >& container, Args&&... args )
	{
		return ( container.push_back( args ), ... );
	}

	#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY

	//struct SetConsoleTextAttributes
	//{
	//	SetConsoleTextAttributes( const WORD attributes )
	//	{
	//		CONSOLE_SCREEN_BUFFER_INFO Info;
	//		HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
	//		GetConsoleScreenBufferInfo( hStdout, &Info );
	//		savedAttributes = Info.wAttributes;
	//		SetConsoleTextAttribute( hStdout, attributes );
	//	}
	//
	//	~SetConsoleTextAttributes()
	//	{
	//		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), savedAttributes );
	//	}
	//
	//private:
	//	WORD savedAttributes = 0;
	//};

	// Hash stuff
	template <typename T, typename... Rest>
	void HashCombine( std::size_t& seed, const T& v, const Rest& ... rest )
	{
		seed ^= std::hash<T>{}(v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		( HashCombine( seed, rest ), ... );
	}

#define MAKE_HASHABLE( type, ... ) \
    namespace std \
	{ \
        template<> struct hash<type> \
		{ \
            std::size_t operator()( const type& t) const \
			{ \
                std::size_t ret = 0; \
                Reflex::HashCombine( ret, __VA_ARGS__ ); \
                return ret; \
            } \
        }; \
    }
}

//MAKE_HASHABLE( sf::Vector2f, t.x, t.y )
//MAKE_HASHABLE( sf::Vector2i, t.x, t.y )
//MAKE_HASHABLE( sf::Vector2u, t.x, t.y )