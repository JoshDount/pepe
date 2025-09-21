/**
 * Main App Component
 * Route Planning Interface Application with Navigation
 */

import { useCallback, useEffect, useState } from 'react';
import Header, { TabType } from './components/Header';
import OriginalSimulator from './components/OriginalSimulator';
import CitySimulatorSection from './components/CitySimulatorSection';
import PerformanceDashboard from './components/PerformanceDashboard';
import { useLoadGraph, useUpdateSystemStatus, useGraphState } from './store';

function App() {
  const loadGraph = useLoadGraph();
  const updateSystemStatus = useUpdateSystemStatus();
  const { loaded } = useGraphState();
  const [isClient, setIsClient] = useState(false);
  const [activeTab, setActiveTab] = useState<TabType>('original');

  // Initialize the application
  useEffect(() => {
    // Set client state to handle hydration
    setIsClient(true);
    
    // Load initial system status
    updateSystemStatus().catch(console.error);
  }, [updateSystemStatus]);

  // Load demo graph separately to avoid infinite loop
  useEffect(() => {
    if (!loaded) {
      loadGraph({
        nodes: 80,
        edges: 0,
        directed: false,
      }).catch(console.error);
    }
  }, [loaded, loadGraph]);

  // Handle header action
  const handleHeaderAction = useCallback(() => {
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

  // Don't render anything until we're on the client to avoid hydration issues
  if (!isClient) {
    return (
      <div className="min-h-screen bg-gray-900 text-white">
        <div className="fixed top-0 left-0 right-0 z-50 bg-gray-800/95 backdrop-blur-sm border-b border-gray-700">
          <div className="max-w-7xl mx-auto px-6 py-4">
            <div className="flex items-center justify-between">
              <div className="flex items-center space-x-3">
                <div className="w-10 h-10 bg-red-600 rounded-lg flex items-center justify-center animate-pulse"></div>
                <div>
                  <div className="h-5 w-48 bg-gray-700 rounded animate-pulse"></div>
                  <div className="h-3 w-32 bg-gray-700 rounded mt-2 animate-pulse"></div>
                </div>
              </div>
              <div className="flex items-center space-x-6">
                <div className="h-3 w-16 bg-gray-700 rounded animate-pulse"></div>
                <div className="h-10 w-24 bg-gray-700 rounded animate-pulse"></div>
              </div>
            </div>
          </div>
        </div>
        <div className="pt-24">
          <div className="max-w-7xl mx-auto px-6 text-center">
            <div className="h-12 w-96 bg-gray-700 rounded mx-auto animate-pulse mb-4"></div>
            <div className="h-6 w-80 bg-gray-700 rounded mx-auto animate-pulse mb-8"></div>
            <div className="grid grid-cols-1 md:grid-cols-3 gap-4 max-w-3xl mx-auto mb-10">
              {[1, 2, 3].map((i) => (
                <div key={i} className="h-24 bg-gray-700 rounded-xl animate-pulse"></div>
              ))}
            </div>
            <div className="h-12 w-48 bg-gray-700 rounded mx-auto animate-pulse"></div>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-900 text-white">
      {/* Header with Navigation */}
      <Header 
        onActionClick={handleHeaderAction} 
        activeTab={activeTab} 
        onTabChange={setActiveTab} 
      />

      {/* Main Content */}
      <main className="pt-48">
        {activeTab === 'original' ? (
          <OriginalSimulator />
        ) : activeTab === 'city' ? (
          <CitySimulatorSection />
        ) : (
          <div className="px-4 sm:px-6 lg:px-8">
            <PerformanceDashboard />
          </div>
        )}
      </main>
    </div>
  );
}

export default App;