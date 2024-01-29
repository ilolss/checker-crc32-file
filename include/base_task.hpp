#ifndef TASK_HPP_
#define TASK_HPP_

class BaseTask {
public:
    virtual void operator()() = 0;
    virtual ~BaseTask() = default;
};

#endif  // TASK_HPP_