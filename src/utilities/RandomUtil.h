#pragma once

#ifndef RANDOMUTIL_H
#define RANDOMUTIL_H

#include <random>

class RandomUtil
{
private:
	static std::random_device rd;
	static std::mt19937 engine;
public:
	RandomUtil() {};
	~RandomUtil() {}

	static void init();

	// These will only return a value between 0 and 1
	static float genRandomFloat();
	static double genRandomDouble();
	static int genRandomInt();

	// These will return a value between the range provided
	static float genRandomFloat(float, float);
	static double genRandomDouble(double, double);
	static int genRandomInt(int, int);

	/* i wanted to use a template but non-decimal values have to use uniform_int_distribution blablabla
	template<typename T>
	static T genRandomValue(T low, T high)
	{
		std::uniform_real_distribution<T> dist{ low, high };
		return dist(engine);
	}*/
};

#endif