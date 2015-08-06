#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

#include <semaphore.h>
#include <vector>
#include <iostream>
#include "util.h"
using namespace std;

class data_pool {
	public:
		typedef client_info::id_type 	id_type;
		typedef std::pair<id_type, string>	data_type;
		enum{MAX_SLOT = 10};
		
		data_pool()
			: _head(0), _tail(0)
		{
			sem_init(&_full, 0, 0);
			sem_init(&_empty, 0, MAX_SLOT);
			_msg_pool.resize(MAX_SLOT);
		}
		~data_pool()
		{
			sem_destroy(&_full);
			sem_destroy(&_empty);
		}

		int push(const data_type value);
		data_type pop();

		data_type &head();
		data_type &tail();

	private:
		inline static int sem_p(sem_t *sem)
		{
			return sem_wait(sem);
		}
		inline static int sem_v(sem_t *sem)
		{
			return sem_post(sem);
		}


	private:

		vector<data_type>	_msg_pool;
		int					_head;
		int					_tail;
		sem_t				_full;
		sem_t				_empty;
};

int data_pool::push(const data_type value)
{
	data_pool::sem_p(&_empty);
	_msg_pool[_head] = value;
	data_pool::sem_v(&_full);
	_head = (_head + 1)%MAX_SLOT;

	return 0;
}

typename data_pool::data_type data_pool::pop()
{
	data_pool::sem_p(&_full);
	data_type data = _msg_pool[_tail];
	_msg_pool[_tail].first = 0;
	_msg_pool[_tail].second = "";
	data_pool::sem_v(&_empty);
	_tail = (_tail + 1)%MAX_SLOT;

	return data;
}

typename data_pool::data_type &data_pool::data_pool::head()
{
	return _msg_pool[(_head -1)%MAX_SLOT];
}

typename data_pool::data_type &data_pool::data_pool::tail()
{
	return _msg_pool[_tail];
}


#endif
