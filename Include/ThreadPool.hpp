#pragma once

#include <list>
#include <mutex>
#include <thread>
#include <unordered_map>

class ThreadPool
{
private:
	std::list<std::thread> m_threadPool;
	std::unordered_map<std::thread::id, decltype(m_threadPool)::iterator> m_threadIDs;
	std::mutex m_mutex;
	std::size_t m_threads;

public:
	explicit ThreadPool(std::size_t threads)
	: m_threads(threads)
	{
	}

	bool CanQueue() const noexcept
	{
		return m_threadPool.size() < m_threads;
	}

	template<typename Callback, typename... Arg>
	void Queue(Callback&& callback, Arg&&... args)
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		std::thread thread([this](auto&& callback, auto&&... args) mutable {
			std::forward<decltype(callback)>(callback)(std::forward<decltype(args)>(args)...);

			std::lock_guard<std::mutex> guard(m_mutex);
			auto iter = m_threadIDs.find(std::this_thread::get_id());

			if (iter != m_threadIDs.end()) {
				iter->second->detach();
				m_threadPool.erase(iter->second);
				m_threadIDs.erase(iter);
			}
		}, std::forward<Callback>(callback), std::forward<Arg>(args)...);

		auto id = thread.get_id();
		m_threadPool.emplace_back(std::move(thread));
		m_threadIDs[id] = std::prev(m_threadPool.end());
	}

	void JoinAll()
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		while (!m_threadPool.empty()) {
			bool joined = false;

			for (auto& thread : m_threadPool) {
				if (thread.joinable()) {
					joined = true;
					m_mutex.unlock();
					thread.join();
					break;
				}
			}

			if (joined)
				m_mutex.lock();
		}
	}
};
