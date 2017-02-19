#pragma once
#include <chrono>
#include <iostream>

using namespace std::chrono;
namespace TerraX
{
	class timekeeper
	{
	public:
		time_point<high_resolution_clock> start_;

	public:
		timekeeper() {
			start_ = high_resolution_clock::now();
		}
		~timekeeper() {
			auto t = duration_cast<milliseconds>(high_resolution_clock::now() - start_);
			std::cout << t.count() << "ms" << std::endl;
		}
	};

#ifdef ENABLE_TIMEKEEPER
#define  TIMER_START timekeeper _t();
#else
#define TIMER_START ((void)0);
#endif
}