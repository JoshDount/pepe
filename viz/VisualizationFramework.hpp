#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>

namespace rts::viz {

/**
 * @brief Color codes for console output
 */
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    
    // Background colors
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";
}

/**
 * @brief Base visualization framework
 */
class Visualizer {
protected:
    bool use_colors_;
    size_t width_;
    size_t height_;
    std::string title_;
    
public:
    Visualizer(const std::string& title = "", size_t width = 80, size_t height = 24, bool use_colors = true)
        : title_(title), width_(width), height_(height), use_colors_(use_colors) {}
    
    virtual ~Visualizer() = default;
    
    /**
     * @brief Set color usage
     */
    void set_colors(bool enabled) { use_colors_ = enabled; }
    
    /**
     * @brief Set dimensions
     */
    void set_dimensions(size_t width, size_t height) {
        width_ = width;
        height_ = height;
    }
    
    /**
     * @brief Apply color formatting
     */
    std::string colorize(const std::string& text, const std::string& color) const {
        if (!use_colors_) return text;
        return color + text + Colors::RESET;
    }
    
    /**
     * @brief Print a horizontal line
     */
    void print_separator(char ch = '-', const std::string& color = "") const {
        std::string line(width_, ch);
        if (!color.empty() && use_colors_) {
            std::cout << colorize(line, color) << "\n";
        } else {
            std::cout << line << "\n";
        }
    }
    
    /**
     * @brief Print a title with formatting
     */
    void print_title() const {
        if (title_.empty()) return;
        
        size_t padding = (width_ - title_.length()) / 2;
        std::string formatted_title = std::string(padding, ' ') + title_;
        
        print_separator('=', Colors::CYAN);
        std::cout << colorize(formatted_title, Colors::BOLD + Colors::WHITE) << "\n";
        print_separator('=', Colors::CYAN);
    }
    
    /**
     * @brief Clear the screen (console)
     */
    void clear_screen() const {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    /**
     * @brief Print centered text
     */
    void print_centered(const std::string& text, const std::string& color = "") const {
        size_t padding = (width_ - text.length()) / 2;
        std::string centered = std::string(padding, ' ') + text;
        
        if (!color.empty()) {
            std::cout << colorize(centered, color) << "\n";
        } else {
            std::cout << centered << "\n";
        }
    }
    
    /**
     * @brief Create a progress bar
     */
    std::string create_progress_bar(double progress, size_t bar_width = 20, 
                                   char filled = '█', char empty = '░') const {
        size_t filled_width = static_cast<size_t>(progress * bar_width);
        std::string bar = std::string(filled_width, filled) + 
                         std::string(bar_width - filled_width, empty);
        
        if (use_colors_) {
            std::string colored_filled = colorize(std::string(filled_width, filled), Colors::GREEN);
            std::string colored_empty = colorize(std::string(bar_width - filled_width, empty), Colors::DIM);
            return "[" + colored_filled + colored_empty + "]";
        }
        
        return "[" + bar + "]";
    }
    
    /**
     * @brief Create a grid for 2D visualizations
     */
    std::vector<std::vector<std::string>> create_grid(size_t rows, size_t cols, 
                                                     const std::string& default_value = " ") const {
        return std::vector<std::vector<std::string>>(rows, std::vector<std::string>(cols, default_value));
    }
    
    /**
     * @brief Print a grid
     */
    void print_grid(const std::vector<std::vector<std::string>>& grid) const {
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                std::cout << cell;
            }
            std::cout << "\n";
        }
    }
    
    /**
     * @brief Create ASCII art numbers
     */
    std::vector<std::string> get_ascii_number(int digit) const {
        std::map<int, std::vector<std::string>> ascii_digits = {
            {0, {"███", "█ █", "█ █", "█ █", "███"}},
            {1, {" █ ", "██ ", " █ ", " █ ", "███"}},
            {2, {"███", "  █", "███", "█  ", "███"}},
            {3, {"███", "  █", "███", "  █", "███"}},
            {4, {"█ █", "█ █", "███", "  █", "  █"}},
            {5, {"███", "█  ", "███", "  █", "███"}},
            {6, {"███", "█  ", "███", "█ █", "███"}},
            {7, {"███", "  █", "  █", "  █", "  █"}},
            {8, {"███", "█ █", "███", "█ █", "███"}},
            {9, {"███", "█ █", "███", "  █", "███"}}
        };
        
        auto it = ascii_digits.find(digit);
        if (it != ascii_digits.end()) {
            return it->second;
        }
        
        return {"   ", "   ", "   ", "   ", "   "}; // Empty for unknown digits
    }
    
    /**
     * @brief Print ASCII art number
     */
    void print_ascii_number(int number, const std::string& color = "") const {
        std::vector<std::string> digits_str;
        std::string num_str = std::to_string(number);
        
        for (char c : num_str) {
            int digit = c - '0';
            digits_str.push_back(std::to_string(digit));
        }
        
        // Get ASCII representation for each digit
        std::vector<std::vector<std::string>> ascii_representations;
        for (char c : num_str) {
            int digit = c - '0';
            ascii_representations.push_back(get_ascii_number(digit));
        }
        
        // Print line by line
        for (size_t line = 0; line < 5; ++line) {
            for (size_t digit_idx = 0; digit_idx < ascii_representations.size(); ++digit_idx) {
                if (digit_idx > 0) std::cout << " "; // Space between digits
                
                if (!color.empty()) {
                    std::cout << colorize(ascii_representations[digit_idx][line], color);
                } else {
                    std::cout << ascii_representations[digit_idx][line];
                }
            }
            std::cout << "\n";
        }
    }
    
    /**
     * @brief Wait for user input
     */
    void wait_for_input(const std::string& prompt = "Press Enter to continue...") const {
        std::cout << colorize(prompt, Colors::YELLOW);
        std::cin.ignore();
        std::cin.get();
    }
    
    /**
     * @brief Print statistics in a formatted table
     */
    void print_statistics_table(const std::map<std::string, double>& stats) const {
        std::cout << colorize("┌─────────────────────────┬──────────────┐", Colors::CYAN) << "\n";
        std::cout << colorize("│", Colors::CYAN) << colorize(" Metric                  ", Colors::BOLD) 
                  << colorize("│", Colors::CYAN) << colorize(" Value        ", Colors::BOLD)
                  << colorize("│", Colors::CYAN) << "\n";
        std::cout << colorize("├─────────────────────────┼──────────────┤", Colors::CYAN) << "\n";
        
        for (const auto& [metric, value] : stats) {
            std::cout << colorize("│ ", Colors::CYAN) 
                      << std::left << std::setw(23) << metric
                      << colorize(" │ ", Colors::CYAN)
                      << std::right << std::setw(12) << std::fixed << std::setprecision(3) << value
                      << colorize(" │", Colors::CYAN) << "\n";
        }
        
        std::cout << colorize("└─────────────────────────┴──────────────┘", Colors::CYAN) << "\n";
    }
    
    /**
     * @brief Create a simple chart
     */
    void print_bar_chart(const std::vector<std::pair<std::string, double>>& data,
                         const std::string& title = "",
                         size_t max_bar_width = 40) const {
        if (data.empty()) return;
        
        if (!title.empty()) {
            print_centered(title, Colors::BOLD + Colors::WHITE);
            std::cout << "\n";
        }
        
        // Find maximum value for scaling
        double max_value = 0.0;
        for (const auto& [label, value] : data) {
            max_value = std::max(max_value, value);
        }
        
        if (max_value == 0.0) max_value = 1.0; // Avoid division by zero
        
        // Print bars
        for (const auto& [label, value] : data) {
            size_t bar_width = static_cast<size_t>((value / max_value) * max_bar_width);
            
            std::cout << std::left << std::setw(15) << label << " ";
            std::cout << colorize(std::string(bar_width, '█'), Colors::GREEN);
            std::cout << " " << std::fixed << std::setprecision(2) << value << "\n";
        }
    }
};

} // namespace rts::viz