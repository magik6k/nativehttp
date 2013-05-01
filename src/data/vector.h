/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED


namespace data
{

	template<class T>class vector
	{
	private:

		size_t elems;
		size_t msize;
		T **eptr;

	public:

		vector(size_t maxsize);

		void push_back(T tp);
		T &operator[](size_t id);

		size_t size();
		int empty();

		void destroy();

	};

	template<class T>vector<T>::vector(size_t maxsize)
	{
		eptr = new T*[maxsize];
		elems = 0;
	}

	template<class T>void vector<T>::push_back(T tp)
	{
		eptr[elems] = new T;
		*(eptr[elems]) = tp;
		elems++;
	}

	template<class T>T &vector<T>::operator[](size_t id)
	{
		return *(eptr[id]);
	}


	template<class T>size_t vector<T>::size()
	{
		return elems;
	}


	template<class T>int vector<T>::empty()
	{
		return (elems == 0);
	}

	template<class T>void vector<T>::destroy()
	{
		for(int i = 0; i < elems; i++)
		{
			delete eptr[i];
		}
		delete[] eptr;
	}

}

#endif // VECTOR_H_INCLUDED
