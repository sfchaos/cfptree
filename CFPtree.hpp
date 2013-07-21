/*
 * CFPtree.hpp
 *
 *  Created on: 2013/06/16
 *      Author: sfchaos
 */

#ifndef CFPTREE_HPP_
#define CFPTREE_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <cstddef>
//#include <stddef.h>

#include "Item.hpp"
//#include "Transaction.hpp"

namespace cfp {
using namespace std;
using namespace cfp;

//class Item;
bool lessVal(const pair<Item, unsigned int>& lhs, const pair<Item, unsigned int>& rhs);
//template <typename K, typename V> void printTable(const pair<K, V>& p);
void printTable(const pair<Item, unsigned int>& p);
template <typename K> void printKey(const K& k);
bool lessItem(const Item &i1, const Item &i2);
bool moreItem(const Item &i1, const Item &i2);

/**
 * A class representing entry
 *
 */

template <typename K = vector<string>, typename V = unsigned int>
class Entry
{
public:
  typedef pair<K, V> item_container;

public:
	//Entry() : minsup_(0) {}
	Entry(K item, V support) : items(map<K, V>(item, support)) {}
	//Entry(V minsup) : minsup_(minsup) {}
	/*
	void insert(item_container &ic) {
		items.insert(ic);
	}
	*/
	V getSupport() const {
		return support_;
	}

private:
	//V minsup_;
	V support_;
	map<K, V> items;
	Entry *child;
};

/**
 * A class representing node
 *
 */

template <typename K = vector<string>, typename V = unsigned int>
class Node
{
public:
	typedef vector<Entry<K, V>* > entry_container;
	//typedef vector<Entry<K, V> > entry_container;

	Node() {}
	Node(unsigned int id) : id_(id) {}
	void insert(Entry<K, V> &e) {
		entries_.push_back(&e);
	}
private:
	entry_container entries_;
	unsigned int id_;
	Node *child_;
};


/**
 * A class representing CFP-tree
 *
 */

//template <typename K, typename V = unsigned int>
//template <typename K = string, typename V = unsigned int>
class CFPtree
{
public:
	typedef Node<vector<string>, unsigned int> NodeType;
	typedef Entry<vector<string>, unsigned int> EntryType;

public:
	CFPtree(unsigned int minsup) : minsup_(minsup) {
			count = 0;
	}
	~CFPtree() {}

	void readTran(string &fn) {
		string line;
		//Transaction tmp;
		vector<Item> tmp;
		string item;
		ifstream ifs(fn.c_str());

		while (ifs && getline(ifs, line)) {
			tmp.clear();
			istringstream istrs((char *) line.c_str());
			while (istrs >> item) {
				// 同一のトランザクションで同一のアイテムのカウントは1とする
				vector<Item>::iterator itr_tran;
				if (find(tmp.begin(), tmp.end(), item) == tmp.end()) {
				//if (!tmp.find(item)) {
					tmp.push_back(Item(string(item), 1));
				}
				// Header Tableへの格納
				vector<Item>::iterator itr_item;
				if ((itr_item = find(htable.begin(), htable.end(), item)) != htable.end()) {
					itr_item->incFreq(1);
				} else {
					htable.push_back(Item(string(item), 1));
				}
			}
			tran.push_back(tmp);
			//tranDB.push_back(tmp);
		}
		// Header tableのソート
		sort(htable.begin(), htable.end(), &lessItem);
	}

	void construct() {
		// support未満のアイテムを削除する
		removeNonfreqItemsFromHtable();
		// トランザクションのアイテムを出現頻度順に並び替える
		constructCondDB();
		//sortTranByItemFreq();

		/*
		for (vector<Item>::iterator itr = htable.begin(); itr != htable.end(); itr++) {
			cout << itr->getItem() << ": " << itr->getFreq() << endl;
		}
		*/

		// CFP-treeの構築
std::cerr << "constructing CFP-tree" << std::endl;
		// 1. header tableをもとにルートノードのエントリーを生成・格納
		unsigned int nodeid = 1;
		NodeType *node = new NodeType(nodeid);
		nodeid++;

		for (vector<Item>::iterator itr = htable.begin();
				itr != htable.end(); ++itr) {
			string item = itr->getItem();
			unsigned int freq = itr->getFreq();
			// conditional transaction DBを読みながら
			// 子ノードを生成する
std::cerr << "item=" << item << std::endl;
			vector<vector<string> >  tran_item = conddb[item];
			vector<Item> htable_item;
			for (vector<vector<string> >::iterator itr_item = tran_item.begin();
					itr_item != tran_item.end(); ++itr_item) {
				vector<string>::iterator itr_item2 = itr_item->begin();
				while (itr_item2 != itr_item->end()) {
					string item = *itr_item2;
					vector<Item>::iterator itr_htbl;
					if ((itr_htbl = find(htable_item.begin(), htable_item.end(), item)) != htable_item.end()) {
						itr_htbl->incFreq(1);
					} else {
						htable_item.push_back(Item(string(item), 1));
					}
				}
			}
			sort(htable.begin(), htable.end(), &moreItem);
			// ルートノードにエントリを格納する
			//EntryType entry(item, freq);
			//node->insert(entry);
		}
		delete node;

		// 2. conditional transaction DBを読みながら，

		/*
		for (vector<pair<Item, unsigned int> >::iterator itr = htable.begin();
				itr != htable.end(); ++itr) {
			//string item = itr->first;
			//unsigned int freq = itemfreq[item];
			//std::cout << item << ": " << freq << std::endl;
		}
		*/

		/*
		for (map<string, unsigned int>::iterator itr = itemfreq.begin();
				itr != itemfreq.end(); ++itr) {
			string item = itr->first;
			vector<string> items;
			items.push_back(item);
			unsigned int freq = itr->second;
			Entry<vector<string>, unsigned int> entry(minsup_);
			pair<vector<string>, unsigned int > ip(items, freq);
			entry.insert(ip);
			node->insert(entry);
		}
		*/
		// 2. 2世代目以降のノードのエントリーを生成・格納


	}

private:
	unsigned int minsup_;
	string delimiter_;
	int count;
	// トランザクションデータベース
	//TransactionDB tranDB;
	vector<vector<Item> > tran;
	// アイテムの出現頻度
	//map<string, unsigned int> itemfreq;
	// Header Table
	//vector<pair<Item, unsigned int> > htable;
	vector<Item> htable;

	// Conditional DataBase
	map<string, vector<vector<string> > > conddb; // 各接頭辞に対するトランザクション
	//map<string, vector<vector<Item> > conddb; // 各接頭辞に対するトランザクション
	//multimap<string, vector<vector<Item> > > conddb;

	// Header tableからの非頻出アイテムの削除
	void removeNonfreqItemsFromHtable() {
		//typename map<string, unsigned int>::iterator itr = itemfreq.begin();
		vector<Item>::iterator itr = htable.begin();
		while (itr != htable.end()) {
			unsigned int freq = itr->getFreq();
	 		if (freq < minsup_) {
				itr = htable.erase(itr);
				//htable.erase(itr++);
			} else {
				++itr;
			}
		}
	}

	void sortTranByItemFreq() {
		for (vector<vector<Item> >::iterator itr1=tran.begin();
			itr1 != tran.end(); ++itr1) {
			for (vector<Item>::iterator itr2=itr1->begin(); itr2 != itr1->end();) {
				string item = itr2->getItem();
				vector<Item>::iterator itemitr = find(htable.begin(), htable.end(), item);
				if (itemitr != htable.end()) {
					// 頻出アイテムは頻度を更新
					//itr2->setFreq(itemitr->second);
					itr2->setFreq(itemitr->getFreq());
					itr2++;
				} else {
					// 非頻出アイテムは削除
					itr2 = (*itr1).erase(itr2);
				}
			}
			sort(itr1->begin(), itr1->end(), &lessItem);
			string item = itr1->begin()->getItem();
			vector<string> tmp;
			for (vector<Item>::iterator itr_tmp = itr1->begin()+1;
				itr_tmp != itr1->end(); ++itr_tmp) {
				tmp.push_back(itr_tmp->getItem());
			}
			//pair<string, vector<string> > p(item, tmp);
			//conddb.insert(p);
		}
	}

	void constructCondDB() {
		for (vector<vector<Item> >::iterator itr1=tran.begin();
			itr1 != tran.end(); ++itr1) {
			for (vector<Item>::iterator itr2=itr1->begin(); itr2 != itr1->end();) {
				string item = itr2->getItem();
				//map<string, unsigned int>::iterator itemitr = itemfreq.find(item);
				vector<Item>::iterator itemitr = find(htable.begin(), htable.end(), item);
				if (itemitr != htable.end()) {
					// 頻出アイテムは頻度を更新
					itr2->setFreq(itemitr->getFreq());
					itr2++;
				} else {
					// 非頻出アイテムは削除
					itr2 = (*itr1).erase(itr2);
				}
			}
			// トランザクションのアイテムを出現頻度の昇順でソート
			sort(itr1->begin(), itr1->end(), &lessItem);
			string item = itr1->begin()->getItem();
			vector<string> tmp;
			//for (vector<item>::iterator itr_tmp = itr1->begin()+1;
			//	itr_tmp != itr1->end(); ++itr_tmp) {
			//	std::cout << itr_tmp->getItem() << std::endl;
				//tmp.push_back(itr_tmp->getItem());
			//}

			/*
			vector<Item> tmp;
			for (vector<Item>::iterator itr_tmp = itr1->begin()+1;
				itr_tmp != itr1->end(); ++itr_tmp) {
				tmp.push_back(*itr_tmp);
			}
			*/

			if (conddb.find(item) != conddb.end()) {
				conddb[item].push_back(tmp);
			} else {
				vector<vector<string> > tmp2;
				tmp2.push_back(tmp);
				conddb.insert(make_pair(item, tmp2));
			}
		}
	}
};


/*
template <typename K, typename V>
bool lessVal(const pair<K, V>& lhs, const pair<K, V>& rhs)
{
	return lhs.second < rhs.second;
}
*/

bool lessVal(const pair<Item, unsigned int>& lhs, const pair<Item, unsigned int>& rhs)
{
	return lhs.first.getFreq() < rhs.first.getFreq();
}

void printTable(const pair<Item, unsigned int>& p)
{
	cout << "key: " << p.first.getItem() << " " << "value: " << p.second << endl;
}

template <typename K>
void printKey(const K& k)
{
	cout << k << endl;
}

template <typename K, typename V>
bool lessValuePair(const pair<K, V>& p, V minsup)
{
	return p.second < minsup;
}

bool lessItem(const Item &i1, const Item &i2)
{
	if (i1.getFreq() == i2.getFreq()) {
		return i1.getItem() < i2.getItem();
	}
	return i1.getFreq() < i2.getFreq();
}

bool moreItem(const Item &i1, const Item &i2)
{
	return !lessItem(i1, i2);
}

#endif /* CFPTREE_HPP_ */
