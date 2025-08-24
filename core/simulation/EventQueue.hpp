#pragma once

#include "../containers/MinHeap.hpp"
#include <functional>
#include <memory>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace rts::core::simulation {

/**
 * @brief Time type for simulation events (in milliseconds since simulation start)
 */
using SimulationTime = double;

/**
 * @brief Base class for all simulation events
 */
class Event {
public:
    SimulationTime scheduled_time;
    uint32_t event_id;
    std::string event_type;
    int priority;  // Lower values = higher priority for events at same time
    
    Event(SimulationTime time, uint32_t id, const std::string& type, int prio = 0)
        : scheduled_time(time), event_id(id), event_type(type), priority(prio) {}
    
    virtual ~Event() = default;
    
    /**
     * @brief Execute the event
     * @param current_time Current simulation time
     */
    virtual void execute(SimulationTime current_time) = 0;
    
    /**
     * @brief Get event description for debugging
     */
    virtual std::string get_description() const {
        return event_type + " (ID: " + std::to_string(event_id) + 
               ", Time: " + std::to_string(scheduled_time) + ")";
    }
    
    /**
     * @brief Clone the event (for rescheduling)
     */
    virtual std::unique_ptr<Event> clone() const = 0;
    
    // Comparison operators for priority queue
    bool operator<(const Event& other) const {
        if (std::abs(scheduled_time - other.scheduled_time) < 1e-9) {
            return priority < other.priority; // Lower priority value = higher priority
        }
        return scheduled_time < other.scheduled_time;
    }
    
    bool operator>(const Event& other) const {
        if (std::abs(scheduled_time - other.scheduled_time) < 1e-9) {
            return priority > other.priority;
        }
        return scheduled_time > other.scheduled_time;
    }
    
    bool operator==(const Event& other) const {
        return std::abs(scheduled_time - other.scheduled_time) < 1e-9 && 
               priority == other.priority && event_id == other.event_id;
    }
};

/**
 * @brief Event queue implementation using custom MinHeap
 * 
 * Manages time-ordered execution of simulation events with priority support.
 * Events are processed in chronological order, with priority handling for
 * simultaneous events.
 */
class EventQueue {
private:
    /**
     * @brief Comparator for Event shared pointers in MinHeap
     */
    struct EventComparator {
        bool operator()(const std::shared_ptr<Event>& a, const std::shared_ptr<Event>& b) const {
            if (!a && !b) return false;
            if (!a) return false;  // null is "greater" than non-null
            if (!b) return true;   // non-null is "less" than null
            return *a < *b; // Use Event's operator<
        }
    };
    
    containers::MinHeap<std::shared_ptr<Event>, EventComparator> heap_;
    uint32_t next_event_id_;
    SimulationTime current_time_;
    size_t events_processed_;
    size_t events_scheduled_;
    size_t events_cancelled_;
    
    // Event statistics
    std::unordered_map<std::string, size_t> event_type_counts_;
    
public:
    EventQueue() : heap_(EventComparator()), next_event_id_(1), current_time_(0.0), 
                  events_processed_(0), events_scheduled_(0), events_cancelled_(0) {}
    
    /**
     * @brief Schedule an event for execution
     * @param event Event to schedule
     * @return Event ID for cancellation/tracking
     */
    uint32_t schedule_event(std::shared_ptr<Event> event) {
        if (!event) {
            throw std::invalid_argument("Cannot schedule null event");
        }
        
        if (event->scheduled_time < current_time_) {
            throw std::invalid_argument("Cannot schedule event in the past");
        }
        
        event->event_id = next_event_id_++;
        heap_.push(event);
        events_scheduled_++;
        event_type_counts_[event->event_type]++;
        
        return event->event_id;
    }
    
    /**
     * @brief Schedule an event at a specific time
     * @param time Simulation time for event execution
     * @param event Event to schedule
     * @param priority Priority for simultaneous events (lower = higher priority)
     * @return Event ID
     */
    template<typename EventType, typename... Args>
    uint32_t schedule_at(SimulationTime time, int priority, Args&&... args) {
        auto event = std::make_shared<EventType>(time, 0, "", priority, std::forward<Args>(args)...);
        return schedule_event(event);
    }
    
    /**
     * @brief Schedule an event after a delay from current time
     * @param delay Time delay from current simulation time
     * @param event Event to schedule
     * @return Event ID
     */
    uint32_t schedule_after(SimulationTime delay, std::shared_ptr<Event> event) {
        if (delay < 0) {
            throw std::invalid_argument("Delay cannot be negative");
        }
        
        event->scheduled_time = current_time_ + delay;
        return schedule_event(event);
    }
    
    /**
     * @brief Get the next event without removing it
     * @return Next event or nullptr if queue is empty
     */
    std::shared_ptr<Event> peek_next() const {
        if (heap_.empty()) {
            return nullptr;
        }
        return heap_.top();
    }
    
    /**
     * @brief Process the next event
     * @return Processed event or nullptr if queue is empty
     */
    std::shared_ptr<Event> process_next() {
        if (heap_.empty()) {
            return nullptr;
        }
        
        auto event = heap_.pop();
        current_time_ = event->scheduled_time;
        
        try {
            event->execute(current_time_);
            events_processed_++;
        } catch (const std::exception& e) {
            // Log error but continue simulation
            // In a real implementation, you might want to handle this differently
        }
        
        return event;
    }
    
    /**
     * @brief Process all events up to a specific time
     * @param end_time Maximum time to process events
     * @return Number of events processed
     */
    size_t process_until(SimulationTime end_time) {
        size_t processed = 0;
        
        while (!heap_.empty() && heap_.top()->scheduled_time <= end_time) {
            process_next();
            processed++;
        }
        
        // Update current time even if no events were processed
        if (current_time_ < end_time) {
            current_time_ = end_time;
        }
        
        return processed;
    }
    
    /**
     * @brief Process a specific number of events
     * @param max_events Maximum number of events to process
     * @return Number of events actually processed
     */
    size_t process_events(size_t max_events) {
        size_t processed = 0;
        
        while (!heap_.empty() && processed < max_events) {
            process_next();
            processed++;
        }
        
        return processed;
    }
    
    /**
     * @brief Cancel all events of a specific type
     * @param event_type Type of events to cancel
     * @return Number of events cancelled
     */
    size_t cancel_events_by_type(const std::string& event_type) {
        // This is a simplified implementation
        // In practice, you might want a more efficient approach
        std::vector<std::shared_ptr<Event>> remaining_events;
        size_t cancelled = 0;
        
        // Extract all events
        while (!heap_.empty()) {
            auto event = heap_.pop();
            if (event->event_type == event_type) {
                cancelled++;
                events_cancelled_++;
            } else {
                remaining_events.push_back(event);
            }
        }
        
        // Re-insert non-cancelled events
        for (auto& event : remaining_events) {
            heap_.push(event);
        }
        
        return cancelled;
    }
    
    /**
     * @brief Clear all events from the queue
     */
    void clear() {
        while (!heap_.empty()) {
            heap_.pop();
            events_cancelled_++;
        }
    }
    
    /**
     * @brief Check if queue is empty
     */
    bool empty() const {
        return heap_.empty();
    }
    
    /**
     * @brief Get number of pending events
     */
    size_t size() const {
        return heap_.size();
    }
    
    /**
     * @brief Get current simulation time
     */
    SimulationTime get_current_time() const {
        return current_time_;
    }
    
    /**
     * @brief Set current simulation time (use carefully)
     */
    void set_current_time(SimulationTime time) {
        if (time < current_time_) {
            throw std::invalid_argument("Cannot move simulation time backwards");
        }
        current_time_ = time;
    }
    
    /**
     * @brief Get next event time
     */
    SimulationTime get_next_event_time() const {
        if (heap_.empty()) {
            return std::numeric_limits<SimulationTime>::infinity();
        }
        return heap_.top()->scheduled_time;
    }
    
    /**
     * @brief Get events scheduled for a specific time window
     */
    std::vector<std::shared_ptr<Event>> get_events_in_window(SimulationTime start_time, 
                                                           SimulationTime end_time) const {
        std::vector<std::shared_ptr<Event>> events_in_window;
        
        // This is a simplified implementation that doesn't modify the heap
        // In practice, you might want a more efficient data structure for range queries
        auto heap_copy = heap_;
        
        while (!heap_copy.empty()) {
            auto event = heap_copy.pop();
            if (event->scheduled_time >= start_time && event->scheduled_time <= end_time) {
                events_in_window.push_back(event);
            }
        }
        
        return events_in_window;
    }
    
    /**
     * @brief Get simulation statistics
     */
    struct Statistics {
        size_t events_scheduled;
        size_t events_processed;
        size_t events_cancelled;
        size_t events_pending;
        SimulationTime current_time;
        SimulationTime next_event_time;
        std::unordered_map<std::string, size_t> event_type_counts;
        
        double get_processing_efficiency() const {
            return events_scheduled > 0 ? 
                static_cast<double>(events_processed) / events_scheduled : 0.0;
        }
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.events_scheduled = events_scheduled_;
        stats.events_processed = events_processed_;
        stats.events_cancelled = events_cancelled_;
        stats.events_pending = heap_.size();
        stats.current_time = current_time_;
        stats.next_event_time = get_next_event_time();
        stats.event_type_counts = event_type_counts_;
        
        return stats;
    }
    
    /**
     * @brief Reset simulation to initial state
     */
    void reset() {
        clear();
        next_event_id_ = 1;
        current_time_ = 0.0;
        events_processed_ = 0;
        events_scheduled_ = 0;
        events_cancelled_ = 0;
        event_type_counts_.clear();
    }
    
    /**
     * @brief Get debug information about queue state
     */
    std::string get_debug_info() const {
        auto stats = get_statistics();
        std::string info = "EventQueue Debug Info:\n";
        info += "  Current Time: " + std::to_string(stats.current_time) + "\n";
        info += "  Events Pending: " + std::to_string(stats.events_pending) + "\n";
        info += "  Events Processed: " + std::to_string(stats.events_processed) + "\n";
        info += "  Events Scheduled: " + std::to_string(stats.events_scheduled) + "\n";
        info += "  Events Cancelled: " + std::to_string(stats.events_cancelled) + "\n";
        info += "  Next Event Time: " + std::to_string(stats.next_event_time) + "\n";
        info += "  Processing Efficiency: " + std::to_string(stats.get_processing_efficiency() * 100) + "%\n";
        
        if (!stats.event_type_counts.empty()) {
            info += "  Event Type Counts:\n";
            for (const auto& [type, count] : stats.event_type_counts) {
                info += "    " + type + ": " + std::to_string(count) + "\n";
            }
        }
        
        return info;
    }
};

/**
 * @brief Base class for discrete event simulation
 */
class DiscreteEventSimulation {
protected:
    EventQueue event_queue_;
    SimulationTime simulation_duration_;
    bool simulation_running_;
    
public:
    DiscreteEventSimulation(SimulationTime duration = 0.0) 
        : simulation_duration_(duration), simulation_running_(false) {}
    
    virtual ~DiscreteEventSimulation() = default;
    
    /**
     * @brief Initialize the simulation
     */
    virtual void initialize() = 0;
    
    /**
     * @brief Run the simulation
     */
    virtual void run() {
        simulation_running_ = true;
        initialize();
        
        if (simulation_duration_ > 0) {
            // Run for fixed duration
            event_queue_.process_until(simulation_duration_);
        } else {
            // Run until no more events
            while (!event_queue_.empty() && simulation_running_) {
                event_queue_.process_next();
            }
        }
        
        simulation_running_ = false;
        finalize();
    }
    
    /**
     * @brief Stop the simulation
     */
    virtual void stop() {
        simulation_running_ = false;
    }
    
    /**
     * @brief Finalize the simulation (cleanup, statistics)
     */
    virtual void finalize() {}
    
    /**
     * @brief Get event queue for external access
     */
    EventQueue& get_event_queue() { return event_queue_; }
    const EventQueue& get_event_queue() const { return event_queue_; }
    
    /**
     * @brief Check if simulation is running
     */
    bool is_running() const { return simulation_running_; }
    
    /**
     * @brief Get current simulation time
     */
    SimulationTime get_current_time() const {
        return event_queue_.get_current_time();
    }
    
    /**
     * @brief Set simulation duration
     */
    void set_duration(SimulationTime duration) {
        simulation_duration_ = duration;
    }
    
    /**
     * @brief Get simulation duration
     */
    SimulationTime get_duration() const {
        return simulation_duration_;
    }
};

} // namespace rts::core::simulation