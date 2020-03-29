#pragma once

#include <vector>
#include <functional>

template<typename ... ARGS>
class Callback
{
public:
	void operator()(ARGS&&... args) const
	{
		for (auto callback : myCallbacks)
			callback(std::forward<ARGS>(args)...);
	}

	template<typename T>
	void Add(T& anObject, void (T::*aCallback)(ARGS...))
	{
		myCallbacks.push_back([&anObject, aCallback](ARGS&&... someArgs)
		{
			(anObject.*aCallback)(std::forward<ARGS>(someArgs)...);
		});
	}

private:
	std::vector<std::function<void(ARGS...)>> myCallbacks;
};
