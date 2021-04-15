#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cLowPassFilter.hpp"

cLowPassFilter::cLowPassFilter()
{
	// Init the vector
	for (int count = 0; count != cLowPassFilter::DEFAULTVECTORSIZE; count++)
	{
		this->vecTimes.push_back(0.0);
	}
	this->m_NextLocation = 0;

	return;
}

void cLowPassFilter::addValue(double newValue)
{
	this->vecTimes[this->m_NextLocation] = newValue;
	this->m_NextLocation++;

	// Have I gone off the end of the vector
	if (this->m_NextLocation >= this->vecTimes.size())
	{
		this->m_NextLocation = 0;
	}
	return;
}
double cLowPassFilter::getAverage(void)
{
	double total = 0.0;
	for (int index = 0; index != this->DEFAULTVECTORSIZE; index++)
	{
		total += this->vecTimes[index];
	}
	total = total / (double)this->DEFAULTVECTORSIZE;
	return total;
}
