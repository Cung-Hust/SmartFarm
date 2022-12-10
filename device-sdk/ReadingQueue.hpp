#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>

#include "../models/models.hpp"

#define READING_QUEUE_DEFAULT_SIZE 100

namespace deviceservice
{
    struct ResourceValues
    {
        string deviceId;
        string resourceName;
        string value;
    };

    class ReadingQueue
    {
    private:
        mutex mutex_;
        queue<vector<ResourceValues>> queue_;
        int sizeQueue_;

    public:
        ReadingQueue()
        {
            this->sizeQueue_ = READING_QUEUE_DEFAULT_SIZE;
        }
        ReadingQueue(int size)
        {
            if (size == 0)
            {
                this->sizeQueue_ = READING_QUEUE_DEFAULT_SIZE;
            }
            else
            {
                this->sizeQueue_ = size;
            }
        }

        void push(vector<ResourceValues> resourceValues)
        {
            const std::lock_guard<std::mutex> lock(this->mutex_);
            if (this->queue_.size() < (std::size_t)this->sizeQueue_)
            {
                this->queue_.push(std::move(resourceValues));
            }
        }

        bool pop(vector<ResourceValues> &resourceValues)
        {
            const std::lock_guard<std::mutex> lock(this->mutex_);
            if (!this->queue_.empty())
            {
                resourceValues = this->queue_.front();
                this->queue_.pop();
                return true;
            }
            return false;
        }
    };
}