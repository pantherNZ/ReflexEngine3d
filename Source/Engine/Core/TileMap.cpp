#include "Precompiled.h"
#include "TileMap.h"
#include "Objects/Object.h"
#include "Components/2D/TransformComponent.h"
#include "Components/2D/SFMLObjectComponent.h"

#define TileMapLogging

namespace Reflex::Core
{
	TileMap::TileMap( const unsigned cellSize, const unsigned chunkSizeInCells )
		: m_cellSize( cellSize )
		, m_chunkSizeInCells( chunkSizeInCells )
	{
		Reset();
	}

	void TileMap::Reset( const unsigned cellSize, const unsigned chunkSizeInCells )
	{
		m_cellSize = cellSize;
		m_chunkSizeInCells = chunkSizeInCells;
		Reset();
	}

	void TileMap::Reset()
	{
		m_chunkSize = m_cellSize * m_chunkSizeInCells;
		m_spacialChunks.clear();

		Chunk newChunk;
		newChunk.chunk = sf::Vector2i( 0, 0 );
		newChunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );
		m_spacialChunks.push_back( newChunk );
	}

	void TileMap::Repopulate( World& world, const unsigned cellSize, const unsigned chunkSizeInCells )
	{
		m_cellSize = cellSize;
		m_chunkSizeInCells = chunkSizeInCells;
		Repopulate( world );
	}

	void TileMap::Repopulate( World& world )
	{
		Reset();
		
		for( const auto object : world.GetObjects() )
			Insert( object );
	}

	void TileMap::Insert( const Object& object )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto position = object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition();
			const auto chunkIdx = ChunkHash( position );

			auto chunk_iter = FindChunk( chunkIdx );
			if( chunk_iter == m_spacialChunks.end() )
			{
				m_spacialChunks.emplace_back();
				chunk_iter = std::prev( m_spacialChunks.end() );
			}

			auto& chunk = *chunk_iter;

			if( chunk.buckets.empty() )
				chunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );

			const auto cellId = GetCellId( position );
			chunk.buckets[cellId].push_back( object );
			chunk.totalObjects++;
			chunk.chunk = chunkIdx;

#ifdef TileMapLogging
			//LOG_INFO( "Insert Position: " << position << ", Chunk: " << chunkIdx << ", cell: " << cellId );
			//if( auto sfmlObj = object.GetComponent< Reflex::Components::SFMLObject >() )
			//	sfmlObj->GetCircleShape().setFillColor( Reflex::ColourFromScalar( ( std::hash< unsigned >()( cellId * 23234 ^ 345 ) % 1000 ) / 1000.0f ) );
#endif
		}
	}

	void TileMap::Insert( const Object& object, const sf::FloatRect& boundary )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );

					auto chunk_iter = FindChunk( chunkIdx );
					if( chunk_iter == m_spacialChunks.end() )
					{
						m_spacialChunks.emplace_back();
						chunk_iter = std::prev( m_spacialChunks.end() );
					}

					auto& chunk = *chunk_iter;

					if( chunk.buckets.empty() )
						chunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );

					const auto cellId = y * m_chunkSizeInCells + x;
					chunk.buckets[cellId].push_back( object );
					chunk.totalObjects++;
					chunk.chunk = chunkIdx;
#ifdef TileMapLogging
					//LOG_INFO( "Insert Boundary: Chunk: " << chunkIdx << ", cell: " << cellId );
					//if( auto sfmlObj = object.GetComponent< Reflex::Components::SFMLObject >() )
					//	sfmlObj->GetCircleShape().setFillColor( Reflex::ColourFromScalar( ( std::hash< unsigned >()( cellId * 23234 ^ 345 ) % 1000 ) / 1000.0f ) );
#endif
				}
			}
		}
	}

	void TileMap::Remove( const Object& object )
	{
		assert( object );
		if( object )
			Remove( object, object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() );
	}

	void TileMap::Remove( const Object& object, const sf::Vector2f& position )
	{
		assert( object );
		if( object )
			Remove( object, ChunkHash( position ), GetCellId( position ) );
	}

	void TileMap::Remove( const Object& object, const sf::Vector2i& chunkIdx, const unsigned cellId )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			auto chunk_iter = FindChunk( chunkIdx );
			auto& bucket = chunk_iter->buckets[cellId];

			const auto found = std::find( bucket.begin(), bucket.end(), object );
			assert( found != bucket.end() );
			if( found != bucket.end() )
			{
#ifdef TileMapLogging
				//LOG_INFO( "Remove Position: " << object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() << ", Chunk: " << chunkIdx << ", cell: " << cellId );
#endif
				bucket.erase( found );
				chunk_iter->totalObjects--;

				if( chunk_iter->totalObjects == 0 )
					m_spacialChunks.erase( chunk_iter );
			}
		}
	}

	void TileMap::Remove( const Object& object, const sf::FloatRect& boundary )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );
					auto chunk_iter = FindChunk( chunkIdx );

					const auto cellId = y * m_chunkSizeInCells + x;
					auto& container = chunk_iter->buckets[cellId];

					const auto found = std::find( container.begin(), container.end(), object );
					assert( found != container.end() );
					if( found != container.end() )
					{
#ifdef TileMapLogging
						//LOG_INFO( "Remove Boundary: , Chunk: " << chunkIdx << ", cell: " << cellId );
#endif

						container.erase( found );
						chunk_iter->totalObjects--;

						if( chunk_iter->totalObjects == 0 )
							m_spacialChunks.erase( chunk_iter );
					}
				}
			}
		}
	}

	void TileMap::GetNearby( const Object& object, const float distance, std::vector< Object >& out ) const
	{
		ForEachInRange( object, distance, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const sf::Vector2f& position, const float distance, std::vector< Object >& out ) const
	{
		ForEachInRange( position, distance, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const sf::FloatRect& boundary, std::vector< Object >& out ) const
	{
		ForEachInBounds( boundary, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	unsigned TileMap::GetCellId( const Object& object ) const
	{
		assert( object );
		return GetCellId( object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() );
	}

	unsigned TileMap::GetCellId( const sf::Vector2f& position ) const
	{
		const auto chunk = ChunkHash( position );
		const auto startPosition = Reflex::Vector2iToVector2f( chunk ) * ( float )m_chunkSize;
		const auto loc = CellHash( position - startPosition );
		return loc.y * m_chunkSizeInCells + loc.x;
	}

	sf::Vector2i TileMap::CellHash( const Object& object ) const
	{
		return CellHash( GetObjectPosition( object ) );
	}

	sf::Vector2i TileMap::ChunkHash( const Object& object ) const
	{
		return ChunkHash( GetObjectPosition( object ) );
	}

	sf::Vector2i TileMap::CellHash( const sf::Vector2f& position ) const
	{
		return sf::Vector2i( int( std::floor( position.x / ( float )m_cellSize ) ), int( std::floor( position.y / ( float )m_cellSize ) ) );
	}

	sf::Vector2i TileMap::ChunkHash( const sf::Vector2f& position ) const
	{
		return sf::Vector2i( int( std::floor( position.x / ( float )m_chunkSize ) ), int( std::floor( position.y / ( float )m_chunkSize ) ) );
	}

	bool TileMap::IsValid() const
	{
		return m_cellSize && m_chunkSize;
	}

	bool TileMap::IsValid( const BaseObject& object ) const
	{
		return Object( object ).IsValid();
	}

	sf::Vector2f TileMap::GetObjectPosition( const BaseObject& object ) const
	{
		assert( IsValid( object ) );
		return Object( object ).GetTransform()->getPosition();
	}

	std::vector< TileMap::Chunk >::iterator TileMap::FindChunk( const sf::Vector2i& chunkIdx )
	{
		return std::find_if( m_spacialChunks.begin(), m_spacialChunks.end(), [&]( const Chunk& chunk )
		{
			return chunk.chunk == chunkIdx;
		} );
	}
}