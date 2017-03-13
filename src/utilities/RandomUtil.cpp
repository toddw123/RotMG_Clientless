#include "RandomUtil.h"

std::random_device RandomUtil::rd;
std::mt19937 RandomUtil::engine;

void RandomUtil::init()
{
	engine.seed(rd());
}

float RandomUtil::genRandomFloat()
{
	std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
	return dist(engine);
}
float RandomUtil::genRandomFloat(float low, float high)
{
	if (low >= high) return low;
	std::uniform_real_distribution<float> dist{ low, high };
	return dist(engine);
}

double RandomUtil::genRandomDouble()
{
	std::uniform_real_distribution<double> dist{ 0.0, 1.0 };
	return dist(engine);
}
double RandomUtil::genRandomDouble(double low, double high)
{
	if (low >= high) return low;
	std::uniform_real_distribution<double> dist{ low, high };
	return dist(engine);
}

int RandomUtil::genRandomInt()
{
	std::uniform_int_distribution<int> dist{ 0, 1 };
	return dist(engine);
}
int RandomUtil::genRandomInt(int low, int high)
{
	std::uniform_int_distribution<int> dist{ low, high };
	return dist(engine);
}