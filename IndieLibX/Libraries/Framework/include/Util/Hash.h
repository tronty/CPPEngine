/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\Hash.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _HASH_H_
#define _HASH_H_

#ifndef NULL
#define NULL 0
#endif

struct HashEntry {
	unsigned int *value;
	HashEntry *next;
	unsigned int index;
};


class HashTable {
public:
	HashTable(const unsigned int dim, const unsigned int entryCount){
		nDim = dim;
		count = 0;
		nEntries = entryCount;
		entries = new HashEntry *[entryCount];
		stx_memset(entries, 0, entryCount * sizeof(HashEntry *));
	}

	~HashTable(){
		for (unsigned int i = 0; i < nEntries; i++){
			HashEntry *entry = entries[i];
			while (entry){
				HashEntry *nextEntry = entry->next;
				delete entry->value;
				delete entry;
				entry = nextEntry;
			}
		}
		delete entries;
	}

	bool insert(const unsigned int *value, unsigned int *index){
		unsigned int hash = 0;
		unsigned int i = 0;
		do {
			hash += value[i];
			hash += (hash << 10);
			i++;
		} while (i < nDim);

		hash %= nEntries;

		HashEntry *entry = entries[hash];

		while (entry){
			if (memcmp(value, entry->value, nDim * sizeof(unsigned int)) == 0){
				*index = entry->index;
				return true;
			}
			entry = entry->next;
		}
		
		HashEntry *newEntry = new HashEntry;
		newEntry->value = new unsigned int[nDim];
		stx_memcpy(newEntry->value, value, nDim * sizeof(unsigned int));
		newEntry->index = count++;

		newEntry->next = entries[hash];
		entries[hash] = newEntry;

		*index = newEntry->index;
		return false;
	}

	unsigned int getCount() const { return count; }

protected:
	unsigned int nDim;
	unsigned int count;
	unsigned int nEntries;

	HashEntry **entries;
};

#endif // _HASH_H_
