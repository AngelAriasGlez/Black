#ifndef _THREAD_TASK_H
#define _THREAD_TASK_H

#include <set>
#include <future>
#include <functional>

class TaskIn {
public:

	template<typename L1, typename L2>
	TaskIn(L1 work, L2 post) {
		mWorkTask = std::async(std::launch::async, work);
		mPostCall = post;
	}

	~TaskIn() {
	}

	bool finish() {

        if (mWorkTask.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			mPostCall(mWorkTask.get());
			return true;
		}
		return false;
	}

private:
	std::future<void*> mWorkTask;
	std::function<void(void*)> mPostCall;
};
class Task {
public:
	template<typename L1, typename L2>
	Task(L1 work, L2 post) {
		auto taskIn = new TaskIn(work, post);
		TASKS.insert(taskIn);
	}


	static void updateAll() {
		for (auto it = TASKS.begin(); it != TASKS.end(); ) {
			if ((*it)->finish()) {
				delete * it;
				it = TASKS.erase(it);
			}
			else {
				++it;
			}
		}
	}
		

	

private:
	static std::set<TaskIn*> TASKS;
};


#endif
