#ifndef ENGINE_FUNCTION_H__
#define ENGINE_FUNCTION_H__

namespace Engine
{
	template <typename T>
	void Safe_Delete(T& pointer)
	{
		if (NULL != pointer)
		{
			delete pointer;
		}

		pointer = NULL;
	}

	template <typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (NULL != pointer)
		{
			delete[] pointer;
		}

		pointer = NULL;
	}

	template <typename T>
	unsigned long Safe_Release(T& pointer)
	{
		unsigned long lRefCnt = 0;

		if (NULL != pointer)
		{
			lRefCnt = pointer->Release();

			if (0 == lRefCnt)
			{
				pointer = NULL;
			}
		}

		return lRefCnt;
	}
}

#endif // ENGINE_FUNCTION_H__