#ifndef _DS_RESOURCE_CONTAINER_H
#define _DS_RESOURCE_CONTAINER_H

#include "DemoTypes.h"

#define _CONTAINER_INITIAL_SIZE 4
#define _CONTAINER_EXPAND 2

#define DEFINE_RESOURCE_HANDLE(_name)		\
		struct _name { uint32 index; };	\

template <typename T, typename H>
class ResourceContainer
{
public:
	
	ResourceContainer(uint32 initialSize = containerInitialiSize)
	{
		resources = new T[initialSize];
		capacity = initialSize;
	}

	~ResourceContainer()
	{
		delete[] resources;
	}

	H CreateResource()
	{
		H a;
		return a;
	}

	void ReleaseResource(H handle)
	{

	}

	T& GetResource(H handle)
	{
		T a;
		return a;
	}

	T* GetResourcePointer(H handle)
	{

	}

private:

	void Expand()
	{

	}

	void Resize() 
	{

	}

	T* resources = nullptr;
	uint32 capacity = 0;

	static const uint32 containerInitialiSize = 4;
	static const uint32 containerExpand = 2;
};

#endif // _DS_RESOURCE_CONTAINER_H