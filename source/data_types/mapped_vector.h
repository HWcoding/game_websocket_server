#ifndef SOURCE_DATA_TYPES_MAPPED_VECTOR_H
#define SOURCE_DATA_TYPES_MAPPED_VECTOR_H
//#include "source/data_types/mapped_vector.h"

#include <vector>
#include <unordered_map>


template<class K, class T>
class MappedVector
{
public:
	T& set(K key, T data);
	void erase(K key);
	T& operator[](const K &k);
	T& getRawData( const size_t &index );
	size_t size();
	void reserve(size_t size);
private:
	void changeKeyIndex(size_t index, size_t changeTo);
	std::vector<T> d{};
	std::unordered_map<K, size_t> IdToIndex{};
};





template<class K, class T>
T& MappedVector<K, T>::set(K key, T data)
{
	d.push_back(data);
	auto position = IdToIndex.emplace(key, 0).first;
	size_t storedIndex = d.size()-1;
	position->second = storedIndex;
	return d.back();
}

template<class K, class T>
void MappedVector<K, T>::erase(K key)
{
	size_t index = IdToIndex[key];
	if(index != d.size()-1) {
		// change the key that points to the last element
		// to point to the erased index
		changeKeyIndex(d.size()-1, index);
		// move the last element to the erased element
		d[index]=std::move(d.back());
	}
	IdToIndex.erase(key);
	d.resize(d.size()-1);
}

template<class K, class T>
T& MappedVector<K, T>::operator[](const K &k)
{
	auto it = IdToIndex.find(k);

	if(it == IdToIndex.end()) {
		return set(k, T());
	}
	else {
		return d[it->second];
	}
}

template<class K, class T>
T& MappedVector<K, T>::getRawData( const size_t &index )
{
	return d[index];
}

template<class K, class T>
size_t MappedVector<K, T>::size()
{
	return d.size();
}

template<class K, class T>
void MappedVector<K, T>::reserve(size_t size) {
	d.reserve(size);
}

template<class K, class T>
void MappedVector<K, T>::changeKeyIndex(size_t index, size_t changeTo)
{
	for(auto it=IdToIndex.begin(); it!=IdToIndex.end(); ++it) {
		if(it->second == index) {
			it->second = changeTo;
			return;
		}
	}
	throw -1;
}

#endif /* SOURCE_DATA_TYPES_MAPPED_VECTOR_H */
