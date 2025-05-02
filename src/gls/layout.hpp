#pragma once

#include <external.hpp>

class Layout {

	private:

		/// Get size of the given OpenGL type
		int glTypeSize(GLenum type);

		friend class VertexBuffer;

		struct Attribute {
			int index;
			int count;
			GLenum type;
			bool normalize;
			int size;
			int offset;

			Attribute(int index, int count, GLenum type, bool normalize, int size, int offset);
		};

		std::vector<Attribute> attributes;

		uint32_t size() const;

		uint32_t apply() const;

	public:

		void attribute(int index, int count, GLenum type, bool normalize = false);

};