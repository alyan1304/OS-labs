#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <mutex>
#include <queue>
#include <condition_variable>
#include <utility>
#include <stdexcept>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size(size), isOpen(true) {}

    void Send(T value) {
        std::unique_lock<std::mutex> u_lck(mtx);
        
        if (!isOpen) {
            throw std::runtime_error("Channel closed");
        }

        if (buffer.size() >= buffer_size) {
            send_var.wait(u_lck, [this]() {
                return buffer.size() < buffer_size || !isOpen;
            });

            if (!isOpen) {
                throw std::runtime_error("Channel closed");
            }
        }
        
        buffer.push(std::move(value));
        recv_var.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> u_lck(mtx);
        
        if (!buffer.empty()) {
            T value = std::move(buffer.front());
            buffer.pop();
            send_var.notify_one();
            return std::make_pair(std::move(value), true);
        }
        
        if (!isOpen) {
            return std::make_pair(T(), false);
        }
        
        recv_var.wait(u_lck, [this]() { 
            return !buffer.empty() || !isOpen; 
        });
        
        if (!buffer.empty()) {
            T value = std::move(buffer.front());
            buffer.pop();
            send_var.notify_one();
            return std::make_pair(std::move(value), true);
        }
        return std::make_pair(T(), false);
    }

    void Close() {
        std::unique_lock<std::mutex> u_lck(mtx);
        if (isOpen) {
            isOpen = false;
            u_lck.unlock();
            send_var.notify_all();
            recv_var.notify_all();
        }
    }

private:
    std::queue<T> buffer;
    int buffer_size;
    std::mutex mtx;
    std::condition_variable send_var;
    std::condition_variable recv_var;
    bool isOpen;
};

#endif // BUFFERED_CHANNEL_H_