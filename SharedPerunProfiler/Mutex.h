/*
 * Bachelor thesis: Performance Analysis of C# Programs
 *
 * Author: Hajek Vojtech (xhajek51)
 * This class is based on: https://github.com/zodiacon/DotNextMoscow2019
 */
#pragma once

#include <memory>

class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	struct Impl;
	std::unique_ptr<Impl> _impl;
};


