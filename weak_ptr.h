#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_

#include "shared_ptr.h"

namespace pointers {

    template<typename T>
    class WeakPtr {
    public:
        WeakPtr() {
            ptr_ = nullptr;
            counter_ = new Counter;
            counter_->counter_ = 0;
            counter_->weak_counter_ = 1;
        }

        explicit WeakPtr(SharedPtr<T> shared_ptr) {
            ptr_ = shared_ptr.Get();
            counter_ = shared_ptr.counter_;
            counter_->AddWeak();
        }

        WeakPtr(const WeakPtr &other_weak) {
            ptr_ = other_weak.ptr_;
            counter_ = other_weak.counter_;
            counter_->AddWeak();
        }

        WeakPtr(WeakPtr &&other_weak) noexcept {
            ptr_ = other_weak.ptr_;
            counter_ = other_weak.counter_;
            other_weak.ptr_ = nullptr;
            other_weak.counter_ = nullptr;
        }

        WeakPtr &operator=(const WeakPtr &other_weak) {
            if (this == &other_weak) {
                return *this;
            }
            WeakDelete();
            ptr_ = other_weak.ptr_;
            counter_ = other_weak.counter_;
            counter_->AddWeak();
            return *this;
        }

        WeakPtr &operator=(WeakPtr &&other_weak)  noexcept {
            if (this == &other_weak) {
                return *this;
            }
            WeakDelete();
            ptr_ = other_weak.ptr_;
            counter_ = other_weak.counter_;
            other_weak.ptr_ = nullptr;
            other_weak.counter_ = nullptr;
            return *this;
        }

        ~WeakPtr() {
            WeakDelete();
        }

        T *Get() {
            return ptr_;
        }

        const T *Get() const {
            return ptr_;
        }

        void Reset() {
            WeakDelete();
            ptr_ = nullptr;
            counter_ = new Counter;
            counter_->counter_ = 0;
            counter_->weak_counter_ = 1;
        }

        bool Expired() {
            return (ptr_ == nullptr || counter_->GetCounter() == 0);
        }

        SharedPtr<T> Lock() {
            if (Expired()) {
                return SharedPtr<T>();
            }
            SharedPtr<T> shared;
            delete shared.counter_;
            shared.ptr_ = ptr_;
            shared.counter_ = counter_;
            ++(*counter_);
            return shared;
        }

        bool operator==(const WeakPtr &other_ptr) const {
            return (ptr_ == other_ptr.ptr_);
        }

        void WeakDelete() {
            if (counter_ == nullptr) {
                return;
            }
            counter_->SubstractWeak();
            if (counter_->GetWeakCounter() == 0 && counter_->GetCounter() == 0) {
                delete counter_;
            }
            ptr_ = nullptr;
            counter_ = nullptr;
        }

    private:
        T *ptr_;
        Counter *counter_{nullptr};
    };
}  // namespace pointers

#endif  // WEAK_PTR_H_
