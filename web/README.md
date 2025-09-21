# Route Transit Simulator - Web Interface

## Quick Start Guide

### **Fastest Way to Run**

**Double-click:** [start_web.bat](../start_web.bat)

The web server will start automatically and open your browser at `http://localhost:8080`

### **Manual Steps**

1. **Open Command Prompt/PowerShell in project directory**

2. **Navigate to web folder:**
   ```bash
   cd web
   ```

3. **Start the server:**
   ```bash
   python server.py
   ```

4. **Open browser:** Go to `http://localhost:8080`

### **What You'll See**

The web interface includes:

- **Interactive Graph Visualization**
  - Drag and zoom network graphs
  - Click nodes to select start/target points
  - Real-time algorithm animation

- **Performance Dashboard**
  - Live performance metrics
  - Memory usage monitoring
  - Algorithm execution statistics

- **Algorithm Controls**
  - Choose between Dijkstra, A*, BFS, DFS
  - Adjust graph size and animation speed
  - Step-by-step execution mode

- **Traffic Simulation**
  - Real-time traffic status
  - Incident management
  - Weather impact simulation

### **Requirements**

- **Python 3.6+** (for web server)
- **Modern browser** (Chrome, Firefox, Safari, Edge)
- **No additional installations needed**

### **Troubleshooting**

#### Problem: "Python not found"
**Solution:** Install Python from [python.org](https://python.org) or use static mode (double-click `index.html`)

#### Problem: "Port 8080 already in use"
**Solution:** 
- Stop other applications using port 8080, or
- Edit `server.py` and change `PORT = 8080` to another port

#### Problem: Browser doesn't open automatically
**Solution:** Manually navigate to `http://localhost:8080`

### **Features Demo**

1. **Generate a test graph:**
   - Click "Generate Random Graph"
   - Adjust graph size with the slider

2. **Run pathfinding algorithms:**
   - Select start and target nodes
   - Choose algorithm (Dijkstra/A*)
   - Click "Run Algorithm"

3. **Watch real-time visualization:**
   - See nodes being visited in real-time
   - Path highlighting when found
   - Performance metrics updating

### **Interactive Controls**

| Control | Function |
|---------|----------|
| **Graph Size Slider** | Adjust number of nodes (10-100) |
| **Algorithm Dropdown** | Select pathfinding algorithm |
| **Start/Target Inputs** | Set source and destination nodes |
| **Animation Speed** | Control visualization speed |
| **Generate Graph** | Create new random network |
| **Run Algorithm** | Execute selected algorithm |
| **Reset** | Clear visualization |

### **Backend Integration**

The web interface connects to the C++ backend through:
- **HTTP API endpoints** for algorithm execution
- **Real-time updates** through Server-Sent Events
- **Performance monitoring** integration
- **Traffic simulation** controls

### **Performance Features**

- **Algorithm Timing:** Microsecond precision measurements
- **Memory Tracking:** Real-time memory usage monitoring
- **Complexity Analysis:** Empirical validation of theoretical bounds
- **Comparative Benchmarks:** Side-by-side algorithm comparison

---

## **Ready to Start?**

**Just run:** `start_web.bat`

**Or manually:** `cd web && python server.py`

**Then visit:** `http://localhost:8080`

**Enjoy the visual Route Transit Simulator experience!**