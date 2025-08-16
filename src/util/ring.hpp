#pragma once
#include "external.hpp"

template <typename T, size_t S>
class Ring {

	private:

		std::array<T, S> buffer;
		size_t head;
		size_t count;

	public:

		Ring()
			: head(0), count(0) {
		}

		/**
		 * Add another element to the ring,
		 * if no space is left the oldest element will be replaced
		 */
		void push(const T& value) {
			buffer[head] = value;
			head = (head + 1) % S;

			if (count != S) {
				count ++;
			}
		}

		/**
		 * Get reference to the element held in the ring,
		 * where 0 is the youngest element and S is the oldest (if present)
		 */
		T& get(size_t index) {
			if (index > count) {
				throw std::out_of_range("index is out of range");
			}

			return buffer[(head - index) % S];
		}

		/**
		 * Check if adding another element will override the oldest one,
		 * this is equal to size() == S
		 */
		bool full() {
			return count == S;
		}

		/**
		 * Clear the buffer, making it so it effectively holds nothing
		 * this is a O(1) operation
		 */
		void clear() {
			count = 0;
		}

		/**
		 * Return the number of elements held in the ring,
		 * will always be in range [0, S]
		 */
		int size() {
			return count;
		}

};
