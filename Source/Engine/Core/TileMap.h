#pragma once

#include "Precompiled.h"
#include "BaseObject.h"

namespace Reflex { class Object; }

namespace Reflex::Core
{
	class TileMap : sf::NonCopyable
	{
		friend class Reflex::Components::Transform;

	public:
		explicit TileMap( const unsigned cellSize, const unsigned chunkSizeInCells );

		void Reset( const unsigned cellSize, const unsigned chunkSizeInCells );
		void Reset();

		void Repopulate( World& world, const unsigned cellSize, const unsigned chunkSizeInCells );
		void Repopulate( World& world );

		void Insert( const Object& object );
		void Insert( const Object& object, const sf::FloatRect& boundary );

		void Remove( const Object& object );
		void Remove( const Object& object, const sf::Vector2f& position );
		void Remove( const Object& object, const sf::Vector2i& chunkIdx, const unsigned cellId );
		void Remove( const Object& object, const sf::FloatRect& boundary );

		void GetNearby( const Object& object, const float distance, std::vector< Object >& out ) const;
		void GetNearby( const sf::Vector2f& position, const float distance, std::vector< Object >& out ) const;
		void GetNearby( const sf::FloatRect& boundary, std::vector< Object >& out ) const;

		template< typename Func >
		void ForEachInRange( const BaseObject& object, const float distance, Func f ) const;

		template< typename Func >
		void ForEachInRange( const sf::Vector2f& position, const float distance, Func f ) const;

		template< typename Func >
		void ForEachInBounds( const sf::FloatRect& boundary, Func f ) const;

	protected:
		unsigned GetCellId( const Object& obj ) const;
		unsigned GetCellId( const sf::Vector2f& position ) const;

		sf::Vector2i CellHash( const Object& object ) const;
		sf::Vector2i CellHash( const sf::Vector2f& position ) const;

		sf::Vector2i ChunkHash( const Object& object ) const;
		sf::Vector2i ChunkHash( const sf::Vector2f& position ) const;

		sf::Vector2f GetObjectPosition( const BaseObject& object ) const;

		bool IsValid() const;
		bool IsValid( const BaseObject& obj ) const;

		template< typename Func >
		void ForEachInBoundsInternal( const sf::FloatRect& boundary, Func f ) const;

	private:
		struct Chunk;
		std::vector< TileMap::Chunk >::iterator FindChunk( const sf::Vector2i& chunkIdx );

	private:
		unsigned m_cellSize = 0U;
		unsigned m_chunkSizeInCells = 0U;
		unsigned m_chunkSize = 0U;

		struct Chunk
		{
			sf::Vector2i chunk;
			std::vector< std::vector< BaseObject > > buckets;
			unsigned totalObjects = 0;
		};
		std::vector< Chunk > m_spacialChunks;
	};

	// Template function definitions
	template< typename Func >
	void TileMap::ForEachInRange( const BaseObject& object, const float distance, Func f ) const
	{
		ForEachInRange( GetObjectPosition( object ), distance, f );
	}

	template< typename Func >
	void TileMap::ForEachInRange( const sf::Vector2f& position, const float distance, Func f ) const
	{
		sf::FloatRect bounds( position - sf::Vector2f( distance, distance ), sf::Vector2f( distance, distance ) * 2.0f );
		ForEachInBoundsInternal( bounds, [&]( const Reflex::Object& object )
		{
			if( Reflex::GetDistanceSq( position, object.GetTransform()->getPosition() ) <= distance * distance )
				f( object );
		} );
	}

	template< typename Func >
	void TileMap::ForEachInBounds( const sf::FloatRect& boundary, Func f ) const
	{
		ForEachInBoundsInternal( boundary, [&]( const Reflex::Object& object )
		{
			if( boundary.intersects( object.GetTransform()->GetGlobalBounds() ) )
				f( object );
		} );
	}

	template< typename Func >
	void TileMap::ForEachInBoundsInternal( const sf::FloatRect& boundary, Func f ) const
	{
		if( IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );
					const auto chunk_iter = std::find_if( m_spacialChunks.begin(), m_spacialChunks.end(), [&]( const Chunk& chunk )
					{
						return chunk.chunk == chunkIdx;
					} );
					
					if( chunk_iter == m_spacialChunks.end() )
						continue;

					const auto cellId = ( y % m_chunkSizeInCells ) * m_chunkSizeInCells + x % m_chunkSizeInCells;
					//const auto& bucket = m_spacialChunks[0].buckets[cellId];
					const auto& bucket = chunk_iter->buckets[cellId];

					for( const auto& obj : bucket )
						f( Object( obj ) );
				}
			}
		}
	}
}