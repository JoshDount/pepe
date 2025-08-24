#pragma once

#include "EventQueue.hpp"
#include <iostream>
#include <sstream>

namespace rts::core::simulation {

/**
 * @brief Simple message event for testing
 */
class MessageEvent : public Event {
private:
    std::string message_;
    
public:
    MessageEvent(SimulationTime time, const std::string& message, int priority = 0)
        : Event(time, 0, "MessageEvent", priority), message_(message) {}
    
    void execute(SimulationTime current_time) override {
        // In testing, we won't actually print to avoid cluttering output
        // std::cout << "[" << current_time << "] " << message_ << std::endl;
    }
    
    std::string get_description() const override {
        return Event::get_description() + " - Message: " + message_;
    }
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<MessageEvent>(scheduled_time, message_, priority);
    }
    
    const std::string& get_message() const { return message_; }
};

/**
 * @brief Counter event that increments a shared counter
 */
class CounterEvent : public Event {
private:
    int* counter_;
    int increment_;
    
public:
    CounterEvent(SimulationTime time, int* counter, int increment = 1, int priority = 0)
        : Event(time, 0, "CounterEvent", priority), counter_(counter), increment_(increment) {}
    
    void execute(SimulationTime current_time) override {
        if (counter_) {
            *counter_ += increment_;
        }
    }
    
    std::string get_description() const override {
        return Event::get_description() + " - Increment: " + std::to_string(increment_);
    }
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<CounterEvent>(scheduled_time, counter_, increment_, priority);
    }
    
    int get_increment() const { return increment_; }
};

/**
 * @brief Self-scheduling event that schedules itself again
 */
class RecurringEvent : public Event {
private:
    EventQueue* queue_;
    SimulationTime interval_;
    int max_iterations_;
    int current_iteration_;
    int* execution_count_;
    
public:
    RecurringEvent(SimulationTime time, EventQueue* queue, SimulationTime interval, 
                  int max_iterations, int* execution_count, int priority = 0)
        : Event(time, 0, "RecurringEvent", priority), queue_(queue), interval_(interval),
          max_iterations_(max_iterations), current_iteration_(0), execution_count_(execution_count) {}
    
    void execute(SimulationTime current_time) override {
        if (execution_count_) {
            (*execution_count_)++;
        }
        
        current_iteration_++;
        
        // Schedule next iteration if not reached max
        if (queue_ && current_iteration_ < max_iterations_) {
            auto next_event = std::make_shared<RecurringEvent>(
                current_time + interval_, queue_, interval_, max_iterations_, 
                execution_count_, priority);
            next_event->current_iteration_ = current_iteration_;
            queue_->schedule_event(next_event);
        }
    }
    
    std::string get_description() const override {
        return Event::get_description() + " - Iteration: " + std::to_string(current_iteration_) + 
               "/" + std::to_string(max_iterations_);
    }
    
    std::unique_ptr<Event> clone() const override {
        auto clone = std::make_unique<RecurringEvent>(
            scheduled_time, queue_, interval_, max_iterations_, execution_count_, priority);
        clone->current_iteration_ = current_iteration_;
        return clone;
    }
    
    int get_current_iteration() const { return current_iteration_; }
    int get_max_iterations() const { return max_iterations_; }
    SimulationTime get_interval() const { return interval_; }
};

/**
 * @brief Event that triggers multiple other events
 */
class TriggerEvent : public Event {
private:
    EventQueue* queue_;
    std::vector<std::shared_ptr<Event>> triggered_events_;
    
public:
    TriggerEvent(SimulationTime time, EventQueue* queue, int priority = 0)
        : Event(time, 0, "TriggerEvent", priority), queue_(queue) {}
    
    void add_triggered_event(std::shared_ptr<Event> event) {
        triggered_events_.push_back(event);
    }
    
    void execute(SimulationTime current_time) override {
        if (queue_) {
            for (auto& event : triggered_events_) {
                if (event) {
                    // Schedule triggered events for immediate execution
                    event->scheduled_time = current_time;
                    queue_->schedule_event(event);
                }
            }
        }
    }
    
    std::string get_description() const override {
        return Event::get_description() + " - Triggers: " + std::to_string(triggered_events_.size()) + " events";
    }
    
    std::unique_ptr<Event> clone() const override {
        auto clone = std::make_unique<TriggerEvent>(scheduled_time, queue_, priority);
        clone->triggered_events_ = triggered_events_;
        return clone;
    }
    
    size_t get_triggered_event_count() const { return triggered_events_.size(); }
};

/**
 * @brief Conditional event that only executes if condition is met
 */
class ConditionalEvent : public Event {
private:
    std::function<bool()> condition_;
    std::shared_ptr<Event> conditional_event_;
    
public:
    ConditionalEvent(SimulationTime time, std::function<bool()> condition, 
                    std::shared_ptr<Event> event, int priority = 0)
        : Event(time, 0, "ConditionalEvent", priority), 
          condition_(condition), conditional_event_(event) {}
    
    void execute(SimulationTime current_time) override {
        if (condition_ && condition_() && conditional_event_) {
            conditional_event_->scheduled_time = current_time;
            conditional_event_->execute(current_time);
        }
    }
    
    std::string get_description() const override {
        std::string desc = Event::get_description() + " - Condition: ";
        desc += (condition_ ? "Set" : "Not Set");
        if (conditional_event_) {
            desc += ", Target: " + conditional_event_->event_type;
        }
        return desc;
    }
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<ConditionalEvent>(scheduled_time, condition_, conditional_event_, priority);
    }
    
    bool has_condition() const { return condition_ != nullptr; }
    bool has_target_event() const { return conditional_event_ != nullptr; }
};

/**
 * @brief Sample simulation using the event system
 */
class SampleSimulation : public DiscreteEventSimulation {
private:
    int message_count_;
    int counter_value_;
    int recurring_executions_;
    
public:
    SampleSimulation(SimulationTime duration = 100.0) 
        : DiscreteEventSimulation(duration), message_count_(0), counter_value_(0), recurring_executions_(0) {}
    
    void initialize() override {
        // Schedule some initial events
        auto msg1 = std::make_shared<MessageEvent>(10.0, "First message");
        auto msg2 = std::make_shared<MessageEvent>(20.0, "Second message");
        auto msg3 = std::make_shared<MessageEvent>(15.0, "Third message", 1); // Lower priority
        
        auto counter1 = std::make_shared<CounterEvent>(25.0, &counter_value_, 5);
        auto counter2 = std::make_shared<CounterEvent>(35.0, &counter_value_, 10);
        
        auto recurring = std::make_shared<RecurringEvent>(
            5.0, &event_queue_, 15.0, 5, &recurring_executions_);
        
        event_queue_.schedule_event(msg1);
        event_queue_.schedule_event(msg2);
        event_queue_.schedule_event(msg3);
        event_queue_.schedule_event(counter1);
        event_queue_.schedule_event(counter2);
        event_queue_.schedule_event(recurring);
        
        message_count_ = 3; // Track scheduled messages
    }
    
    void finalize() override {
        // Can add cleanup or final statistics here
    }
    
    // Getters for testing
    int get_message_count() const { return message_count_; }
    int get_counter_value() const { return counter_value_; }
    int get_recurring_executions() const { return recurring_executions_; }
};

} // namespace rts::core::simulation