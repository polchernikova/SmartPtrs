#ifndef SHARED_PTR_H_
#define SHARED_PTR_H_

#include <stdexcept>

namespace pointers {
    class Counter {
    public:
        Counter() = default;

        int GetCounter() const {
            return counter_;
        }

        int GetWeakCounter() const {
            return weak_counter_;
        }

        void operator++() {
            counter_++;
        }

        void operator--() {
            counter_--;
        }

        void AddWeak() {
            weak_counter_++;
        }

        void SubstractWeak() {
            weak_counter_--;
        }

        template<typename>
        friend
        class SharedPtr;

        template<typename>
        friend
        class WeakPtr;

    private:
        int counter_{0};
        int weak_counter_{0};
    };

    template<typename T>
    class SharedPtr {
    public:
        SharedPtr() {
            ptr_ = nullptr;
            counter_ = new Counter;
            counter_->counter_ = 1;
            counter_->weak_counter_ = 0;
        }

        explicit SharedPtr(T *ptr) : ptr_(ptr) {
            counter_ = new Counter();
            ++(*counter_);
        }

        SharedPtr(const SharedPtr &other_ptr) {
            ptr_ = other_ptr.ptr_;
            counter_ = other_ptr.counter_;
            ++(*counter_);
        }

        SharedPtr(SharedPtr &&other_ptr) noexcept {
            ptr_ = other_ptr.ptr_;
            counter_ = other_ptr.counter_;
            other_ptr.ptr_ = nullptr;
            other_ptr.counter_ = nullptr;
        }

        SharedPtr &operator=(SharedPtr const &other_ptr) {
            if (this == &other_ptr) {
                return *this;
            }
            Delete();
            ptr_ = other_ptr.ptr_;
            counter_ = other_ptr.counter_;
            ++(*counter_);
            return *this;
        }

        SharedPtr &operator=(SharedPtr &&other_ptr) noexcept {
            if (this == &other_ptr) {
                return *this;
            }
            Delete();
            ptr_ = other_ptr.ptr_;
            counter_ = other_ptr.counter_;
            other_ptr.ptr_ = nullptr;
            other_ptr.counter_ = nullptr;
            return *this;
        }

        ~SharedPtr() {
            Delete();
        }

        T *Get() {
            return ptr_;
        }

        int GetCounter() {
            return counter_->GetCounter();
        }

        const T *Get() const {
            return ptr_;
        }

        void Reset() {
            Delete();
            ptr_ = nullptr;
            counter_ = new Counter;
            counter_->counter_ = 1;
            counter_->weak_counter_ = 0;
        }

        T &operator*() {
            return *ptr_;
        }

        T *operator->() {
            return ptr_;
        }

        const T &operator*() const {
            return *ptr_;
        }

        const T *operator->() const {
            return ptr_;
        }

        void Delete() {
            if (counter_ == nullptr) {
                return;
            }
            --(*counter_);
            if (counter_->GetCounter() == 0) {
                delete ptr_;
                if (counter_->GetWeakCounter() == 0) {
                    delete counter_;
                }
            }
            ptr_ = nullptr;
            counter_ = nullptr;
        }

        bool operator==(const SharedPtr &other_ptr) const {
            return (ptr_ == other_ptr.ptr_);
        }

        bool operator==(const T *raw_ptr) const {
            return (ptr_ == raw_ptr);
        }

        bool friend operator==(const T *raw_ptr, const SharedPtr &shared_ptr) {
            return (shared_ptr == raw_ptr);
        }

        bool operator!=(const SharedPtr &other_ptr) const {
            return (ptr_ != other_ptr.ptr_);
        }

        bool operator!=(const T *raw_ptr) const {
            return (ptr_ != raw_ptr);
        }

        bool friend operator!=(const T *raw_ptr, const SharedPtr &shared_ptr) {
            return (shared_ptr != raw_ptr);
        }

        friend class Counter;

        template<typename>
        friend
        class WeakPtr;

    private:
        T *ptr_{nullptr};
        Counter *counter_{nullptr};
    };
}  // namespace pointers

#endif  // SHARED_PTR_H_
