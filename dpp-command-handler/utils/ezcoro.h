#ifndef EZCORO_H
#define EZCORO_H

// this just helps make optional coroutine code more compact
#ifdef DPP_CORO
# include <dpp/coro/task.h>
# define AWAIT(task) co_await task
# define RETURN(res) co_return res
# define TASK(ReturnType) dpp::task<ReturnType>
#else
# define AWAIT(task) task
# define RETURN(res) return res
# define TASK(ReturnType) ReturnType
#endif

#endif // EZCORO_H
