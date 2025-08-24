/**
 * Main App Component
 * Route Planning Interface Application
 */

import { useCallback, useEffect } from 'react';
import Header from './components/Header';
import HeroSection from './components/HeroSection';
import ControlPanel from './components/ControlPanel';
import MapContainer from './components/MapContainer';
import MetricsGrid from './components/MetricsGrid';
import AlgorithmTimeline from './components/AlgorithmTimeline';
import ResultsPanel from './components/ResultsPanel';
import { useRouteActions, useGraphState } from './store';
import { styleTokens } from './styles/tokens';

function App() {
  const { loadGraph, updateSystemStatus } = useRouteActions();
  const { loaded } = useGraphState();

  // Initialize the application
  useEffect(() => {
    // Load initial system status
    updateSystemStatus().catch(console.error);

    // Load a demo graph for demonstration
    if (!loaded) {
      loadGraph({
        nodes: 100,
        edges: 250,
        directed: false,
      }).catch(console.error);
    }
  }, [loadGraph, updateSystemStatus, loaded]);

  // Handle header action
  const handleHeaderAction = useCallback(() => {
    // Scroll to control panel
    const controlPanel = document.getElementById('control-panel');
    if (controlPanel) {
      controlPanel.scrollIntoView({ behavior: 'smooth' });
    }
  }, []);

  // Handle hero get started
  const handleGetStarted = useCallback(() => {
    // Scroll to control panel
    const controlPanel = document.getElementById('control-panel');
    if (controlPanel) {
      controlPanel.scrollIntoView({ behavior: 'smooth' });
    }
  }, []);

  // Handle route calculation success
  const handleRouteCalculated = useCallback((_result: any) => {
    // Scroll to map to show the result
    const mapContainer = document.getElementById('map-container');
    if (mapContainer) {
      mapContainer.scrollIntoView({ behavior: 'smooth' });
    }
  }, []);

  return (
    <div className="min-h-screen bg-bg-default text-text-primary">
      {/* Fixed Header */}
      <Header onActionClick={handleHeaderAction} />

      {/* Main Content */}
      <main>
        {/* Hero Section */}
        <HeroSection onGetStarted={handleGetStarted} />

        {/* Control Panel Section */}
        <section id="control-panel" className={`${styleTokens.section} bg-bg-default`}>
          <div className={styleTokens.container}>
            <ControlPanel onRouteCalculated={handleRouteCalculated} />
          </div>
        </section>

        {/* Map and Visualization Section */}
        <section id="map-container" className={`${styleTokens.section} bg-bg-surface/30`}>
          <div className={styleTokens.container}>
            <div className="grid grid-cols-1 xl:grid-cols-3 gap-8">
              {/* Map - Takes 2/3 width on large screens */}
              <div className="xl:col-span-2">
                <MapContainer height="600px" />
              </div>
              
              {/* Side Panel - Takes 1/3 width on large screens */}
              <div className="space-y-6">
                {/* Results Panel */}
                <ResultsPanel />
                
                {/* Quick Stats Card */}
                <div className={styleTokens.card}>
                  <h3 className={`${styleTokens.text.heading} mb-4`}>Quick Stats</h3>
                  <div className="space-y-3">
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-text-secondary">Graph Status</span>
                      <span className={`text-sm font-medium ${
                        loaded ? 'text-accent-success' : 'text-text-muted'
                      }`}>
                        {loaded ? 'Loaded' : 'Not Loaded'}
                      </span>
                    </div>
                    
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-text-secondary">Visualization</span>
                      <span className="text-sm font-medium text-accent-primary">Active</span>
                    </div>
                    
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-text-secondary">Algorithm</span>
                      <span className="text-sm font-medium text-text-primary">Ready</span>
                    </div>
                  </div>
                </div>

                {/* Instructions Card */}
                <div className={styleTokens.card}>
                  <h3 className={`${styleTokens.text.heading} mb-4`}>How to Use</h3>
                  <div className="space-y-3 text-sm text-text-secondary">
                    <div className="flex items-start space-x-3">
                      <div className="w-6 h-6 bg-accent-primary rounded-full flex items-center justify-center text-white text-xs font-bold mt-0.5">1</div>
                      <div>
                        <div className="font-medium text-text-primary">Set Source & Target</div>
                        <div>Enter node IDs for route calculation</div>
                      </div>
                    </div>
                    
                    <div className="flex items-start space-x-3">
                      <div className="w-6 h-6 bg-accent-primary rounded-full flex items-center justify-center text-white text-xs font-bold mt-0.5">2</div>
                      <div>
                        <div className="font-medium text-text-primary">Choose Algorithm</div>
                        <div>Select A* or Dijkstra algorithm</div>
                      </div>
                    </div>
                    
                    <div className="flex items-start space-x-3">
                      <div className="w-6 h-6 bg-accent-primary rounded-full flex items-center justify-center text-white text-xs font-bold mt-0.5">3</div>
                      <div>
                        <div className="font-medium text-text-primary">Calculate Route</div>
                        <div>View results on map and metrics</div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </section>

        {/* Algorithm Timeline Section */}
        <section className={`${styleTokens.section} bg-bg-default`}>
          <div className={styleTokens.container}>
            <div className="text-center mb-12">
              <h2 className={`${styleTokens.text.title} mb-4`}>Algorithm Timeline</h2>
              <p className={`${styleTokens.text.caption} max-w-2xl mx-auto`}>
                Real-time visualization of algorithm execution steps and node exploration
              </p>
            </div>
            <AlgorithmTimeline />
          </div>
        </section>

        {/* Metrics Section */}
        <section className={`${styleTokens.section} bg-bg-default`}>
          <div className={styleTokens.container}>
            <div className="text-center mb-12">
              <h2 className={`${styleTokens.text.title} mb-4`}>Performance Analytics</h2>
              <p className={`${styleTokens.text.caption} max-w-2xl mx-auto`}>
                Real-time performance metrics and algorithm analysis data
              </p>
            </div>
            <MetricsGrid />
          </div>
        </section>

        {/* Features Section */}
        <section className={`${styleTokens.section} bg-bg-surface/20`}>
          <div className={styleTokens.container}>
            <div className="text-center mb-12">
              <h2 className={`${styleTokens.text.title} mb-4`}>Key Features</h2>
              <p className={`${styleTokens.text.caption} max-w-2xl mx-auto`}>
                Powerful tools for route planning and algorithm analysis
              </p>
            </div>
            
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
              {/* Feature 1 */}
              <div className={`${styleTokens.card} text-center`}>
                <div className="w-16 h-16 bg-accent-primary/20 rounded-xl flex items-center justify-center mx-auto mb-6">
                  <svg className="w-8 h-8 text-accent-primary" fill="currentColor" viewBox="0 0 20 20">
                    <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
                  </svg>
                </div>
                <h3 className={`${styleTokens.text.heading} mb-3`}>Algorithm Comparison</h3>
                <p className={styleTokens.text.caption}>
                  Compare A* and Dijkstra algorithms side by side with detailed performance metrics.
                </p>
              </div>

              {/* Feature 2 */}
              <div className={`${styleTokens.card} text-center`}>
                <div className="w-16 h-16 bg-accent-success/20 rounded-xl flex items-center justify-center mx-auto mb-6">
                  <svg className="w-8 h-8 text-accent-success" fill="currentColor" viewBox="0 0 20 20">
                    <path d="M13 6a3 3 0 11-6 0 3 3 0 016 0zM18 8a2 2 0 11-4 0 2 2 0 014 0zM14 15a4 4 0 00-8 0v3h8v-3z" />
                  </svg>
                </div>
                <h3 className={`${styleTokens.text.heading} mb-3`}>Real-time Visualization</h3>
                <p className={styleTokens.text.caption}>
                  Watch algorithms explore the search space in real-time with interactive visualization.
                </p>
              </div>

              {/* Feature 3 */}
              <div className={`${styleTokens.card} text-center`}>
                <div className="w-16 h-16 bg-accent-warning/20 rounded-xl flex items-center justify-center mx-auto mb-6">
                  <svg className="w-8 h-8 text-accent-warning" fill="currentColor" viewBox="0 0 20 20">
                    <path d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4zM3 10a1 1 0 011-1h6a1 1 0 011 1v6a1 1 0 01-1 1H4a1 1 0 01-1-1v-6zM14 9a1 1 0 00-1 1v6a1 1 0 001 1h2a1 1 0 001-1v-6a1 1 0 00-1-1h-2z" />
                  </svg>
                </div>
                <h3 className={`${styleTokens.text.heading} mb-3`}>Performance Analytics</h3>
                <p className={styleTokens.text.caption}>
                  Detailed performance metrics including execution time, nodes expanded, and memory usage.
                </p>
              </div>
            </div>
          </div>
        </section>
      </main>
    </div>
  );
}

export default App;
