/*
 * Item.hpp
 *
 *  Created on: 2013/06/15
 *      Author: sfchaos
 */

#ifndef ITEM_HPP_
#define ITEM_HPP_

#include <string>

namespace cfp {
using namespace std;

/**
 * A class representing item
 *
 */
class Item
{
public:
  Item(const string &item) : item_(item), freq_(0) {}
	Item(const string &item, int freq) : item_(item), freq_(freq) {}
	const string &getItem() const {return item_;}
	int getFreq() const {return freq_;}
	void incFreq() {freq_++;}
	void incFreq(unsigned int n) {freq_ += n;}
	void setFreq(unsigned int n) {freq_ = n;}

	bool operator == (const string &s) {
		return item_ == s;
	}

	bool operator == (const Item &item) {
		return item_ == item.item_ && freq_ == item.freq_;
	}

	bool operator < (const Item &item) {
		if (freq_ == item.freq_) {
			return item_ < item.item_;
			//return *pItem_ < *(item.pItem_);
		}
		return freq_ < item.freq_;
	}

	/*
	ostream& operator << (ostream &os) {
		os << *pItem_ << endl;
		return os;
	}
	*/

private:
	//const string *pItem_;
	string item_;
	int freq_;
};

}

#endif /* ITEM_HPP_ */
