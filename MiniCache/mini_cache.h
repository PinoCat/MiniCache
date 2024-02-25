#pragma once

#include <mutex>
#include <list>
#include <unordered_map>

namespace Cache {
	template <typename Data, size_t Capacity>
	class MiniCache {
		using List = std::list<Data>;
		static_assert(Capacity > 0);

		std::mutex readWriteMutex;

		const size_t capacity = Capacity;
		const size_t DATA_SIZE = sizeof(Data);

		List cacheLru;
		std::unordered_map<size_t, std::pair<Data, typename List::iterator>> cache;

	public:
		MiniCache() {}

		void Put(size_t key, Data data) {
			std::lock_guard<std::mutex> guard(readWriteMutex);

			if (cache.find(key) != cache.end()) {
				cache[key].first = data;
				UpdateLru(key);
				return;
			}

			if (cache.size() >= capacity) {
				int lruKey = cacheLru.back();
				cacheLru.pop_back();
				cache.erase(lruKey);
			}

			cacheLru.push_front(key);
			cache[key] = { data, cacheLru.begin() };
			//cache[key].first = data;
			//UpdateLru(key);
		}

		Data Get(size_t key) {
			std::lock_guard<std::mutex> guard(readWriteMutex);

			if (cache.find(key) == cache.end()) {
				return -1;
			}

			UpdateLru(key);

			return cache[key].first;
		}

	private:
		void UpdateLru(size_t key) {
			cacheLru.erase(cache[key].second);
			cacheLru.push_front(key);
			cache[key].second = cacheLru.begin();
		}
	};
}
