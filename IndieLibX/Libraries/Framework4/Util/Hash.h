
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

class Hash
{
public:
	Hash(const unsigned int dim, const unsigned int entry_count, const unsigned int capasity)
	{
		curr = mem = (unsigned char *) malloc(entry_count * sizeof(HashEntry *) + capasity * (sizeof(HashEntry) + sizeof(unsigned int) * dim));

		m_NumDims = dim;
		m_Count = 0;
		m_NumEntries = entry_count;
		//entries = new HashEntry *[entry_count];
		m_Entries = (HashEntry **) NewMem(entry_count * sizeof(HashEntry *));

		memset(m_Entries, 0, entry_count * sizeof(HashEntry *));
	}

	~Hash()
	{
		free(mem);
/*
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
*/
	}

	bool Insert(const unsigned int* value, unsigned int* index)
	{
		unsigned int hash = 0;//0xB3F05C27;
		unsigned int i = 0;
		do
		{
			hash += value[i];
			hash += (hash << 11);
			//hash ^= (hash >> 6);
			i++;
		} while (i < m_NumDims);

		hash %= m_NumEntries;

		HashEntry* entry = m_Entries[hash];

		while (entry)
		{
			if (memcmp(value, entry->value, m_NumDims * sizeof(unsigned int)) == 0)
			{
				*index = entry->index;
				return true;
			}

			entry = entry->next;
		}
		
		//HashEntry *newEntry = new HashEntry;
		//newEntry->value = new unsigned int[m_NumDims];

		HashEntry* new_entry = (HashEntry *) NewMem(sizeof(HashEntry));
		new_entry->value = (unsigned int *) NewMem(sizeof(unsigned int) * m_NumDims);


		memcpy(new_entry->value, value, m_NumDims * sizeof(unsigned int));
		new_entry->index = m_Count++;

		new_entry->next = m_Entries[hash];
		m_Entries[hash] = new_entry;

		*index = new_entry->index;
		return false;
	}

	unsigned int GetCount() const { return m_Count; }

protected:
	unsigned int m_NumDims;
	unsigned int m_Count;
	unsigned int m_NumEntries;

	struct HashEntry
	{
		unsigned int* value;
		HashEntry* next;
		unsigned int index;
	};
	HashEntry** m_Entries;


	void* NewMem(const unsigned int size)
	{
		unsigned char *rmem = curr;
		curr += size;
		return rmem;
	}

	unsigned char *mem, *curr;
};
