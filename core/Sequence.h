#pragma once

#include <memory>
#include <limits>
#include <assert.h>
#include <stdexcept>
#include "bad_concepts.h"
#include "bad_utility.h"

//TODO: proper insert and all pushes/emplaces hierarchy
//TODO: range constructors

namespace badCore
{
	template<typename T> requires IS_SEQUENCE_COMPATIBLE<T>
	class Sequence final
	{
		using type = Sequence<T>;
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		//allocator/deallocator functions
		pointer alloc_memory(size_type count)const
		{
			assert(count < max_size());
			return static_cast<pointer>(::operator new(count * sizeof(value_type)));
		}

		constexpr void free_memory(pointer& mem)noexcept
		{
			::operator delete(mem);
			mem = nullptr;
		}

		constexpr void destroy(pointer obj)noexcept
		{
			obj->~T();
		}

		constexpr void destroy_objects(pointer begin, pointer end)noexcept
		{
			while (begin != end)
				begin++->~T();
		}

		//NOTE:: does not take size
		//NOTE2:: forever trust me bro function
		template<typename LambdaFunc>
		void reConstructAllocate(size_type newSize, LambdaFunc constructor)
		{
			//allocate new chunck of memory of size newSize and move [from -> to] data into it (always move) 
			iterator thisBegin = begin();
			iterator thisEnd = end();

			iterator destination = nullptr;
			iterator initialized = nullptr;

			try {
				destination = alloc_memory(newSize);
				initialized = constructor(destination, newSize);
			}
			catch (...) {
				destroy_objects(destination, initialized);
				free_memory(destination);
				throw;
			}

			if (mArray) {
				destroy_objects(thisBegin, thisEnd);
				free_memory(mArray);
			}
			mArray = destination;
			mCapacity = newSize;
		}

		//growth math
		constexpr size_type growthFactor(size_type seed)const noexcept
		{
			return size_type(seed + (seed * 0.5f) + mAdditive);
		}

	public:

		constexpr Sequence()noexcept = default;

		explicit Sequence(size_type count)
			requires std::default_initializable<value_type>
		:Sequence(count, value_type())
		{

		}

		explicit Sequence(size_type count, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			if (count > CORE_ZERO) {
				reConstructAllocate(count, [&value](pointer dest, size_type n) {
					return std::uninitialized_fill_n(dest, n, value);
					});
				mSize = count;
			}
		}

		Sequence(std::initializer_list<value_type> init)
			requires std::constructible_from<value_type, const_reference>
		{
			const size_type size = init.size();
			if (size > CORE_ZERO) {
				reConstructAllocate(size, [init](pointer dest, size_type n) {
					return std::uninitialized_copy(init.begin(), init.end(), dest);
					});
				mSize = size;
			}
		}

		Sequence(const Sequence& rhs)
			requires std::constructible_from<value_type, const_reference>
		{
			size_type size = rhs.size();
			if (size > CORE_ZERO) {
				reConstructAllocate(size, [&rhs](pointer dest, size_type n) {
					return std::uninitialized_copy(rhs.begin(), rhs.end(), dest);
					});
				mSize = size;
			}
		}

		Sequence(Sequence&& rhs)noexcept
		{
			mArray = rhs.mArray;
			rhs.mArray = nullptr;

			mSize = rhs.mSize;
			rhs.mSize = 0;

			mCapacity = rhs.mCapacity;
			rhs.mCapacity = 0;

			mAdditive = rhs.mAdditive;
			rhs.mAdditive = 1;
		}

		Sequence& operator=(Sequence rhs)noexcept
		{
			rhs.swap(*this);
			return *this;
		}

		Sequence& operator=(std::initializer_list<value_type> ilist)
		{
			Sequence temp(ilist);
			temp.swap(*this);
			return *this;
		}

		~Sequence()noexcept
		{
			if (mArray) {
				destroy_objects(begin(), end());
				free_memory(mArray);
				mSize = 0;
				mCapacity = 0;
			}
		}

		constexpr void swap(Sequence& rhs)noexcept
		{
			std::swap(mArray, rhs.mArray);
			std::swap(mSize, rhs.mSize);
			std::swap(mCapacity, rhs.mCapacity);
			std::swap(mAdditive, rhs.mAdditive);
		}

		using iterator = pointer;
		using const_iterator = const_pointer;

		constexpr iterator begin()noexcept
		{
			return  mArray;
		}

		constexpr iterator end()noexcept
		{
			return  mArray + mSize;
		}

		constexpr const_iterator begin()const noexcept
		{
			return mArray;
		}

		constexpr const_iterator end()const noexcept
		{
			return mArray + mSize;
		}

		constexpr const_iterator cbegin()const noexcept
		{
			return mArray;
		}

		constexpr const_iterator cend()const noexcept
		{
			return mArray + mSize;
		}

		constexpr pointer data() noexcept
		{
			return mArray;
		}

		constexpr const_pointer data() const noexcept
		{
			return mArray;
		}


		//UB if container is empty, otherwise returns reference to the first element
		constexpr reference front() noexcept
		{
			assert(!isEmpty());
			return *begin();
		}

		//UB if container is empty, otherwise returns reference to the first element
		constexpr const_reference front()const noexcept
		{
			assert(!isEmpty());
			return *begin();
		}

		//UB if container is empty, otherwise returns reference to the last element
		constexpr reference back() noexcept
		{
			assert(!isEmpty());
			return mArray[mSize - 1];
		}

		//UB if container is empty, otherwise returns reference to the last element
		constexpr const_reference back()const noexcept
		{
			assert(!isEmpty());
			return mArray[mSize - 1];
		}

		//UB if index is not in the range of [begin -> end), otherwise returns reference to the element
		constexpr reference operator[](size_type index) noexcept
		{
			assert(!isEmpty());
			return mArray[index];
		}

		//UB if index is not in the range of [begin -> end), otherwise returns reference to the element
		constexpr const_reference operator[](size_type index)const noexcept
		{
			assert(!isEmpty());
			return mArray[index];
		}

		//throws BadException if indeex is not in the range of [begin -> end), otherwise returns reference to the element
		constexpr reference at(size_type index)
		{
			if (index >= mSize)
				throw std::out_of_range("out of range index");
			return mArray[index];
		}

		//throws BadException if indeex is not in the range of [begin -> end), otherwise returns reference to the element
		constexpr const_reference at(size_type index)const
		{
			if (index >= mSize)
				throw std::out_of_range("out of range index");
			return mArray[index];
		}

		//number of constructed elements
		constexpr size_type size()const noexcept
		{
			return mSize;
		}

		//maximum number elements depending on sizeof(T)
		constexpr size_type max_size()const noexcept
		{
			return std::numeric_limits<size_type>::max() / sizeof(value_type);
		}

		//current size of allocated memory
		constexpr size_type capacity() const noexcept
		{
			return mCapacity;
		}

		//if any constructed elements
		constexpr bool isEmpty()const noexcept
		{
			return mSize == CORE_ZERO;
		}

		//set addative for growth calculation, can't go lower than 1 in debug, otherwise all types of memes will happen
		constexpr void set_additive(size_type additive)noexcept
		{
			assert(additive >= 1);
			mAdditive = additive;
		}

		//destroy all constructed elements
		constexpr void clear()noexcept
		{
			destroy_objects(begin(), end());
			mSize = 0;
		}

		//clears and sets to nullptr
		void wipe()noexcept
		{
			if (!mArray || mCapacity == 0)
				return;

			destroy_objects(begin(), end());
			secure_zero_bytes(mArray, mCapacity * sizeof(value_type));

			::operator delete(mArray);

			mArray = nullptr;
			mSize = 0;
			mCapacity = 0;;
		}

		//copies elements
		void push_back(const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			emplace_back(value);
		}

		//moves elements
		void push_back(value_type&& value)
			requires std::constructible_from<value_type, value_type&&>
		{
			emplace_back(std::move(value));
		}

		//creates elements in place
		template<typename... Args>
		void emplace_back(Args&&... args)
			requires std::constructible_from<value_type, Args...>
		{
			//if at capacity, reallocate with extra memory
			if (mSize == mCapacity) {
				set_capacity(growthFactor(mCapacity));
			}
			new(mArray + mSize)value_type(std::forward<Args>(args)...);
			++mSize;
		}

		template<std::input_iterator It>
		void insert_back(It first, It last)
			requires std::constructible_from<value_type, std::iter_reference_t<It>>
		{
			for (; first != last; ++first)
				emplace_back(*first);
		}


		//UB if the container is empty, otherwise erases the last element
		constexpr void pop_back()noexcept
		{
			assert(!isEmpty());
			erase(end() - 1, end());
		}

		//UB if pos is not in the range of [begin -> end) and if container is empty (probably terminate)
		constexpr void erase(const_iterator pos) noexcept
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			assert(!isEmpty() && pos >= begin() && pos < end());
			erase(pos, pos + 1);
		}

		//UB if given range is not in the range of [begin -> end) and if container is empty (probably terminate)
		constexpr void erase(const_iterator first, const_iterator last) noexcept
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			if (first == last)return;

			iterator targetBegin = const_cast<iterator>(first);
			iterator targetEnd = const_cast<iterator>(last);
			iterator thisBegin = begin();
			iterator thisEnd = end();
			assert(!isEmpty());
			assert(thisBegin <= targetBegin && thisEnd >= targetEnd && targetBegin <= targetEnd);
			//get distance
			size_type destroy_size = static_cast<size_type>(targetEnd - targetBegin);

			//move objects
			for (; targetEnd != thisEnd; ++targetBegin, ++targetEnd) {
				*targetBegin = std::move(*targetEnd);
			}

			mSize -= destroy_size;
			destroy_objects(end(), thisEnd);
		}

		//swaps given element with last and pops back. UB if pos is not in the range of [begin -> end)
		constexpr void swap_with_last_erase(const_iterator pos) noexcept
		{
			assert(!isEmpty() && pos >= begin() && pos < end());
			swap_with_last_erase(pos, pos + 1);
		}

		//swaps given range with last one by one and pops range. UB if given range is not in the range of [begin -> end)
		constexpr void swap_with_last_erase(const_iterator first, const_iterator last) noexcept
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			if (first == last)return;
			iterator destination = const_cast<iterator>(first);
			iterator targetEnd = const_cast<iterator>(last);
			iterator thisEnd = end();
			assert(!isEmpty());
			assert(begin() <= destination && thisEnd >= targetEnd && destination <= targetEnd);
			//get distance
			size_type destroy_size = static_cast<size_type>(targetEnd - destination);

			iterator src = thisEnd - 1;

			for (size_type i = 0; i < destroy_size; ++i) {
				*destination++ = std::move(*src--);
			}
			mSize -= destroy_size;
			destroy_objects(end(), thisEnd);
		}

		//acts as both a reserver or trimmer. if n is more than current cappacity, sets new capacity to given, if less then destroys the difference amount
		void set_capacity(size_type n)
			requires std::is_nothrow_move_assignable_v<value_type>
		{
			pointer oldBegin = begin();
			size_type moveCount = core_min(mSize, n);
			reConstructAllocate(n, [oldBegin, moveCount](pointer dest, size_type size) {
				return std::uninitialized_move(oldBegin, oldBegin + moveCount, dest);
				}
			);
			mSize = moveCount;
		}
		//default constructs or erases elements by the difference amount of current size and given count
		void resize(size_type count)
			requires std::default_initializable<value_type>
		{
			value_type def = value_type{};
			resize(count, def);
		}

		//default constructs or erases elements by the difference amount of current size and given count
		void resize(size_type count, const_reference value)
			requires std::constructible_from<value_type, const_reference>
		{
			if (count < mSize) {
				erase(begin() + count, end());
			}
			else {

				if (count > mCapacity)
					set_capacity(count);

				size_type amount = count - mSize;
				while (amount--) {
					emplace_back(value);
				}
			}
		}

		//shrinks to current size
		void shrink_to_fit()
		{
			set_capacity(mSize);
		}

	private:
		//member variables
		pointer mArray = nullptr;
		size_type mSize = 0;
		size_type mCapacity = 0;

		size_type mAdditive = 1;
	};
}
