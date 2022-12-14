#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include <functional>
#include <vector>

namespace core
{
/**
 * \brief Action is an utility class loosely based on the observer pattern and close to C# Action class
 * \tparam Ts arguments types of the callback function
 */
template <class ... Ts>
class Action
{
public:
	/**
	 * \brief RegisterCallback is a method that registers a function that will be called when the Execute method is called.
	 * \param callback is a function to be called when calling Execute
	 */
	void RegisterCallback(const std::function<void(Ts ...)>& callback)
	{
		_callbacks.push_back(callback);
	}

	/**
	 * \brief Execute is a method that calls the functions registered in the callbacks_.
	 * \param args are the arguments needed to call the registered functions.
	 */
	void Execute(Ts ... args)
	{
		for (auto& callback : _callbacks)
		{
			callback(args...);
		}
	}

private:
	std::vector<std::function<void(Ts ...)>> _callbacks;
};
}
