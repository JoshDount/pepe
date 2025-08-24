# Route Transit Simulator - Qt Desktop GUI Design

## 🖥️ Desktop Application with Qt Framework

### Overview
A modern desktop application using Qt6 framework that provides a rich graphical interface for the Route Transit Simulator.

### Technical Specifications

#### Framework: Qt6 with C++20
- **GUI Framework**: Qt6 (Qt Widgets + Qt Quick for modern UI)
- **Graphics**: Qt Graphics View Framework for graph visualization
- **Charts**: Qt Charts for performance monitoring
- **Networking**: Qt Network for potential future web integration
- **Threading**: Qt Concurrent for background algorithm execution

#### Application Architecture
```cpp
// Main Application Structure
class RTSMainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit RTSMainWindow(QWidget *parent = nullptr);
    ~RTSMainWindow();

private slots:
    void onCreateGraph();
    void onRunAlgorithm();
    void onStartSimulation();
    void onShowPerformance();

private:
    // Core components
    std::unique_ptr<rts::core::graph::WeightedGraph> graph_;
    std::unique_ptr<rts::core::traffic::TrafficSimulation> traffic_sim_;
    std::unique_ptr<rts::core::simulation::EventQueue> event_queue_;
    
    // UI components
    GraphVisualizationWidget* graph_widget_;
    AlgorithmControlWidget* algorithm_controls_;
    PerformanceMonitorWidget* performance_monitor_;
    TrafficSimulationWidget* traffic_dashboard_;
    LogWidget* log_widget_;
    
    // Layouts
    QSplitter* main_splitter_;
    QTabWidget* control_tabs_;
    QStatusBar* status_bar_;
    QMenuBar* menu_bar_;
    QToolBar* tool_bar_;
};
```

### Key Features

#### 1. Interactive Graph Visualization
```cpp
class GraphVisualizationWidget : public QGraphicsView {
    Q_OBJECT
    
public:
    explicit GraphVisualizationWidget(QWidget *parent = nullptr);
    
    void setGraph(const rts::core::graph::WeightedGraph* graph);
    void visualizeAlgorithm(AlgorithmType type, uint32_t start, uint32_t target);
    void highlightPath(const std::vector<uint32_t>& path);
    void showTrafficStates();

signals:
    void nodeSelected(uint32_t node_id);
    void edgeSelected(uint32_t from, uint32_t to);

private:
    QGraphicsScene* scene_;
    std::map<uint32_t, QGraphicsEllipseItem*> node_items_;
    std::vector<QGraphicsLineItem*> edge_items_;
    QPropertyAnimation* animation_;
};
```

#### 2. Algorithm Control Panel
```cpp
class AlgorithmControlWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit AlgorithmControlWidget(QWidget *parent = nullptr);

private slots:
    void onAlgorithmChanged();
    void onRunClicked();
    void onStepClicked();
    void onResetClicked();

private:
    QComboBox* algorithm_combo_;
    QSpinBox* start_node_spin_;
    QSpinBox* target_node_spin_;
    QSlider* animation_speed_slider_;
    QPushButton* run_button_;
    QPushButton* step_button_;
    QPushButton* reset_button_;
    QProgressBar* progress_bar_;
};
```

#### 3. Real-time Performance Monitoring
```cpp
class PerformanceMonitorWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit PerformanceMonitorWidget(QWidget *parent = nullptr);
    
    void updateMetrics(const PerformanceMetrics& metrics);

private:
    QChart* performance_chart_;
    QChartView* chart_view_;
    QLabel* memory_label_;
    QLabel* cpu_label_;
    QLabel* execution_time_label_;
    QTimer* update_timer_;
};
```

#### 4. Traffic Simulation Dashboard
```cpp
class TrafficSimulationWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit TrafficSimulationWidget(QWidget *parent = nullptr);
    
    void setTrafficSimulation(rts::core::traffic::TrafficSimulation* sim);
    void updateTrafficStates();

private slots:
    void onStartSimulation();
    void onTriggerIncident();
    void onSetWeatherFactor();

private:
    QTableWidget* traffic_table_;
    QChart* traffic_chart_;
    QSlider* weather_slider_;
    QPushButton* start_button_;
    QComboBox* incident_combo_;
};
```

### UI Layout Design

#### Main Window Layout
```
┌─────────────────────────────────────────────────────────────────┐
│ File  Edit  View  Algorithm  Simulation  Benchmark  Help       │ Menu Bar
├─────────────────────────────────────────────────────────────────┤
│ [New] [Open] [Save] │ [▶Run] [⏸Step] [⏹Stop] │ [📊Stats]      │ Tool Bar
├─────────────────────┴─────────────────────────┴─────────────────┤
│                                                                 │
│  ┌─────────────────────────────┬─────────────────────────────┐  │
│  │                             │  ┌─ Algorithm Controls ──┐  │  │
│  │                             │  │ Algorithm: [Dijkstra] │  │  │
│  │     Graph Visualization     │  │ Start: [1] Target: [10]│  │  │
│  │                             │  │ Speed: [────●─────]    │  │  │
│  │        Interactive          │  │ [▶ Run] [⏸ Step]     │  │  │
│  │         Network             │  └─────────────────────────┘  │  │
│  │                             │                              │  │
│  │      (Drag & Drop,          │  ┌─ Performance Monitor ──┐  │  │
│  │       Zoom, Pan)            │  │ Time: 11.2ms          │  │  │
│  │                             │  │ Memory: 2.5MB         │  │  │
│  │                             │  │ CPU: 15%              │  │  │
│  │                             │  │ [Real-time Chart]     │  │  │
│  │                             │  └─────────────────────────┘  │  │
│  │                             │                              │  │
│  │                             │  ┌─ Traffic Simulation ───┐  │  │
│  │                             │  │ Status: [Running]      │  │  │
│  │                             │  │ Incidents: 3 active    │  │  │
│  │                             │  │ Weather: ●────────     │  │  │
│  │                             │  │ [Start] [Incident]     │  │  │
│  └─────────────────────────────┴─────────────────────────────┘  │
│                                                                 │
│  ┌─ Execution Log ────────────────────────────────────────────┐  │
│  │ [12:34:56] Algorithm started: Dijkstra(1 → 10)            │  │
│  │ [12:34:56] Visiting node 1 (distance: 0.0)                │  │
│  │ [12:34:56] Visiting node 3 (distance: 2.5)                │  │
│  │ [12:34:57] Path found: 1 → 3 → 7 → 10 (total: 15.2)      │  │
│  └────────────────────────────────────────────────────────────┘  │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│ Ready │ Nodes: 100 │ Edges: 387 │ Memory: 2.5MB │ Status: OK    │ Status Bar
└─────────────────────────────────────────────────────────────────┘
```

### Implementation Plan

#### Phase 1: Basic Framework (Week 1)
1. **Setup Qt Project**
   - Create CMake configuration with Qt6
   - Basic main window with menu and toolbar
   - Integration with existing C++ backend

2. **Core Integration**
   - Link existing graph and algorithm classes
   - Basic graph display (simple nodes and edges)
   - Algorithm execution wrapper

#### Phase 2: Interactive Visualization (Week 2)
1. **Advanced Graphics**
   - Interactive node/edge selection
   - Zoom and pan functionality
   - Animated algorithm execution

2. **Algorithm Controls**
   - Parameter input controls
   - Step-by-step execution
   - Animation speed control

#### Phase 3: Performance & Simulation (Week 3)
1. **Performance Monitoring**
   - Real-time performance charts
   - Memory usage visualization
   - Benchmark integration

2. **Traffic Simulation**
   - Traffic state visualization
   - Incident management UI
   - Simulation controls

### CMake Configuration
```cmake
# Qt GUI Module
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

add_executable(rts_gui
    main.cpp
    RTSMainWindow.cpp
    GraphVisualizationWidget.cpp
    AlgorithmControlWidget.cpp
    PerformanceMonitorWidget.cpp
    TrafficSimulationWidget.cpp
    LogWidget.cpp
)

target_link_libraries(rts_gui
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    rts_core
    rts_viz
)

# Enable Qt MOC
set_target_properties(rts_gui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
```

### Advantages of Qt GUI
- **Native Performance**: Direct C++ integration
- **Rich Widgets**: Professional desktop application feel
- **Cross-Platform**: Works on Windows, Linux, macOS
- **Advanced Graphics**: Hardware-accelerated visualization
- **Integrated Tools**: Built-in profiling and debugging

### Installation Requirements
```bash
# Install Qt6 development libraries
# Windows: Qt Online Installer
# Linux: sudo apt-get install qt6-base-dev qt6-charts-dev
# macOS: brew install qt6

# Build with Qt support
cmake .. -DQT_GUI=ON -DCMAKE_PREFIX_PATH=/path/to/qt6
make rts_gui
```

### Features Comparison

| Feature | CLI | Web Interface | Qt Desktop |
|---------|-----|---------------|------------|
| **Performance** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Ease of Use** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Visualization** | ⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Deployment** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **Interactivity** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Development Time** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ |

### Recommended Implementation Order
1. **Immediate**: Use the web interface (already created)
2. **Short-term**: Enhance web interface with real backend integration
3. **Long-term**: Develop Qt desktop application for professional use