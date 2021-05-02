#pragma once

namespace Reflex::Core
{
	class ComponentAllocatorBase
	{
	public:
		ComponentAllocatorBase( const std::size_t count, const std::size_t elementSize, const std::size_t chunkSize = 8192 )
			: chunkSize( chunkSize )
			, elementSize( elementSize )
		{
			ExpandToFit( count );
		}

		~ComponentAllocatorBase()
		{
			for( auto& chunk : data )
				delete[] chunk;
			data.clear();
		}

		std::size_t GetCapacity() const { return capacity; }
		std::size_t GetCount() const { return count; }
		std::size_t GetChunkCount() const { return data.size(); }
		std::size_t GetElementSize() const { return elementSize; }
		std::size_t GetChunkSize() const { return chunkSize; }

		void Reserve( const std::size_t num )
		{
			while( num >= capacity )
				Append();
		}

		void Resize( const std::size_t num )
		{
			count = num;
		}

		void ExpandToFit( const std::size_t num )
		{
			if( num > count ) 
			{
				if( num >= capacity ) 
					Reserve( num );
				count = num;
			}
		}

		void Append()
		{
			data.emplace_back( new char[elementSize * chunkSize] );
			capacity += chunkSize;
		}

		void* Get( const std::size_t index ) 
		{
			assert( index < count );
			return static_cast< void* >( data[index / chunkSize] + ( index % chunkSize ) * elementSize );
		}

		const void* Get( const std::size_t index ) const
		{
			assert( index < count );
			return static_cast< const void* >( data[index / chunkSize] + ( index % chunkSize ) * elementSize );
		}

		virtual void* ConstructEmpty( const size_t index, const Object& object ) = 0;
		virtual void Destroy( const std::size_t index ) = 0;

	protected:
		std::vector< char* > data;
		const std::size_t elementSize = 0;
		const std::size_t chunkSize = 0;
		std::size_t count = 0;
		std::size_t capacity = 0;
	};

	template< typename T >
	class ComponentAllocator : public ComponentAllocatorBase
	{
	public:
		ComponentAllocator( const std::size_t count, const std::size_t chunkSize = 8192 )
			: ComponentAllocatorBase( count, sizeof( T ), chunkSize )
		{
		}

		T* Get( const std::size_t index )
		{
			return static_cast< T* >( ComponentAllocatorBase::Get( index ) );
		}

		const T* Get( const std::size_t index ) const
		{
			return static_cast< const T* >( ComponentAllocatorBase::Get( index ) );
		}

		void* ConstructEmpty( const size_t index, const Object& object )
		{
			return ( void* )Construct( index, object );
		}

		template< typename... Args >
		T* Construct( const std::size_t index, Args&& ... args )
		{
			ExpandToFit( index );
			assert( index < count );
			new( Get( index ) ) T( std::forward<Args>( args )... );
			return Get( index );
		}

		void Destroy( const std::size_t index )
		{
			assert( index < count );
			Get( index )->~T();
		}
	};
}