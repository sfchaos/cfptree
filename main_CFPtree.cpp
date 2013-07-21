/*
 * main_CFPtree.cpp
 *
 *  Created on: Aug 19, 2012
 *      Author: sfchaos
 */

#include <iostream>

#include "CFPtree.hpp"

using namespace cfp;

int main()
{
  unsigned int minsup = 3;

	//CFPtree<std::string, unsigned int> cfptree(minsup);
	cfp::CFPtree cfptree(minsup);

	std::string fn="../data/test3.dat";
	cfptree.readTran(fn);
	cfptree.construct();

	return 0;
}
