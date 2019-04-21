#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <unordered_map>

template<typename KeyType, typename ValueType>
class Hashtable
{
private:
	std::unordered_map<KeyType, ValueType> m_table;
	std::mutex m_rwLock;
	std::mutex m_queueLock;
	std::size_t m_readers = 0;

public:
	std::optional<ValueType> Get(const KeyType& key)
	{
		std::optional<ValueType> value;

		m_queueLock.lock();
		m_readers += 1;

		if (m_readers == 1) // first reader, wait until writers are done
			m_rwLock.lock(); // prevent writing
		m_queueLock.unlock();

		auto iter = m_table.find(key);

		if (iter != m_table.end())
			value = iter->second;

		m_queueLock.lock();
		m_readers -= 1;

		if (m_readers == 0) // last reader, all readers are done
			m_rwLock.unlock(); // allow writing
		m_queueLock.unlock();

		return value;
	}

	void Set(const KeyType& key, ValueType value)
	{
		std::lock_guard<std::mutex> lock(m_rwLock);
		m_table[key] = std::move(value);
	}

	bool Delete(const KeyType& key)
	{
		std::lock_guard<std::mutex> lock(m_rwLock);
		auto it = m_table.find(key);

		if (it != m_table.end()) {
			m_table.erase(it);
			return true;
		}

		return false;
	}
};
