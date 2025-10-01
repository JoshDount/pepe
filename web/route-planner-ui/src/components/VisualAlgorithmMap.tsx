import React, { useState, useEffect, useMemo } from 'react';
import { MapContainer, TileLayer, Marker, Popup, Polyline } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

// Fix for default markers
delete (L.Icon.Default.prototype as any)._getIconUrl;
L.Icon.Default.mergeOptions({
  iconRetinaUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon-2x.png',
  iconUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon.png',
  shadowUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-shadow.png',
});

interface CityNode {
  id: number;
  lat: number;
  lon: number;
}

interface AlgorithmStep {
  step: number;
  action: string;
  currentNode: number;
  exploredNodes: number[];
  frontier: number[];
  distances: Record<number, number>;
  description: string;
  timestamp: number;
}

interface VisualAlgorithmMapProps {
  nodes: CityNode[];
  algorithmSteps: AlgorithmStep[];
  currentStep: number;
  route: CityNode[];
  onStepChange?: (step: number) => void;
}

const VisualAlgorithmMap: React.FC<VisualAlgorithmMapProps> = ({
  nodes,
  algorithmSteps,
  currentStep,
  route,
  onStepChange
}) => {
  const [mapCenter, setMapCenter] = useState<[number, number]>([24.7840, -107.3867]);
  const [mapZoom, setMapZoom] = useState(14);

  // Calculate map bounds and center
  useEffect(() => {
    if (nodes.length > 0) {
      const lats = nodes.map(node => node.lat);
      const lons = nodes.map(node => node.lon);
      const minLat = Math.min(...lats);
      const maxLat = Math.max(...lats);
      const minLon = Math.min(...lons);
      const maxLon = Math.max(...lons);
      
      const centerLat = (minLat + maxLat) / 2;
      const centerLon = (minLon + maxLon) / 2;
      
      setMapCenter([centerLat, centerLon]);
      
      // Calculate appropriate zoom level
      const latDiff = maxLat - minLat;
      const lonDiff = maxLon - minLon;
      const maxDiff = Math.max(latDiff, lonDiff);
      
      if (maxDiff > 0.1) setMapZoom(12);
      else if (maxDiff > 0.05) setMapZoom(13);
      else if (maxDiff > 0.02) setMapZoom(14);
      else setMapZoom(15);
    }
  }, [nodes]);

  // Get current step data
  const currentStepData = algorithmSteps[currentStep];
  
  // Create node markers with different colors based on algorithm state
  const nodeMarkers = useMemo(() => {
    if (!currentStepData) return [];

    return nodes.map(node => {
      let color = '#6B7280'; // Default gray (unvisited)
      let size = 6;
      let className = '';

      // Determine node state
      if (currentStepData.exploredNodes.includes(node.id)) {
        color = '#10B981'; // Green (explored)
        size = 8;
        className = 'explored-node';
      } else if (currentStepData.frontier.includes(node.id)) {
        color = '#F59E0B'; // Yellow (frontier)
        size = 8;
        className = 'frontier-node';
      } else if (node.id === currentStepData.currentNode) {
        color = '#EF4444'; // Red (current)
        size = 12;
        className = 'current-node';
      }

      // Check if node is in final route
      if (route.some(routeNode => routeNode.id === node.id)) {
        color = '#3B82F6'; // Blue (route)
        size = 10;
        className = 'route-node';
      }

      // Create custom icon
      const icon = L.divIcon({
        html: `<div style="
          width: ${size}px;
          height: ${size}px;
          background-color: ${color};
          border: 2px solid white;
          border-radius: 50%;
          box-shadow: 0 2px 4px rgba(0,0,0,0.3);
          transition: all 0.3s ease;
        "></div>`,
        className: `custom-marker ${className}`,
        iconSize: [size, size],
        iconAnchor: [size / 2, size / 2]
      });

      return (
        <Marker
          key={node.id}
          position={[node.lat, node.lon]}
          icon={icon}
        >
          <Popup>
            <div className="text-sm">
              <div className="font-semibold">Nodo {node.id}</div>
              <div className="text-gray-600">
                {node.lat.toFixed(6)}, {node.lon.toFixed(6)}
              </div>
              {currentStepData.distances[node.id] !== undefined && (
                <div className="text-blue-600 font-medium">
                  Distancia: {Math.round(currentStepData.distances[node.id])}m
                </div>
              )}
            </div>
          </Popup>
        </Marker>
      );
    });
  }, [nodes, currentStepData, route]);

  // Create route polyline
  const routePolyline = useMemo(() => {
    if (route.length < 2) return null;

    const routePath = route.map(node => [node.lat, node.lon] as [number, number]);

    return (
      <Polyline
        positions={routePath}
        pathOptions={{
          color: '#3B82F6',
          weight: 6,
          opacity: 0.8,
          dashArray: '10, 10'
        }}
      />
    );
  }, [route]);

  // Create exploration lines (show connections being explored)
  const explorationLines = useMemo(() => {
    if (!currentStepData || currentStepData.action !== 'explore') return null;

    const currentStepNode = nodes.find(n => n.id === currentStepData.currentNode);
    if (!currentStepNode) return null;

    // Show lines to nodes being updated
    return Object.keys(currentStepData.distances)
      .filter(nodeId => {
        const nodeIdNum = parseInt(nodeId);
        return nodeIdNum !== currentStepData.currentNode && 
               currentStepData.distances[nodeIdNum] !== Infinity;
      })
      .map(nodeId => {
        const targetNode = nodes.find(n => n.id === parseInt(nodeId));
        if (!targetNode) return null;

        return (
          <Polyline
            key={`exploration-${currentStepData.currentNode}-${nodeId}`}
            positions={[
              [currentStepNode.lat, currentStepNode.lon],
              [targetNode.lat, targetNode.lon]
            ]}
            pathOptions={{
              color: '#F59E0B',
              weight: 3,
              opacity: 0.6,
              dashArray: '5, 5'
            }}
          />
        );
      })
      .filter(Boolean);
  }, [currentStepData, nodes]);

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
      <div className="flex items-center justify-between mb-4">
        <h3 className="text-xl font-bold text-white">🗺️ Visualización del Algoritmo</h3>
        <div className="flex items-center gap-4">
          <div className="text-sm text-gray-400">
            Paso {currentStep + 1} de {algorithmSteps.length}
          </div>
          <div className="flex flex-wrap gap-2">
            <button
              onClick={() => onStepChange?.(Math.max(0, currentStep - 1))}
              disabled={currentStep === 0}
              className="px-3 py-1 bg-gray-600 hover:bg-gray-700 disabled:bg-gray-800 disabled:cursor-not-allowed text-white rounded text-sm transition-colors"
            >
              ← Anterior
            </button>
            <button
              onClick={() => onStepChange?.(Math.min(algorithmSteps.length - 1, currentStep + 1))}
              disabled={currentStep === algorithmSteps.length - 1}
              className="px-3 py-1 bg-gray-600 hover:bg-gray-700 disabled:bg-gray-800 disabled:cursor-not-allowed text-white rounded text-sm transition-colors"
            >
              Siguiente →
            </button>
          </div>
        </div>
      </div>

      {/* Current Step Description */}
      {currentStepData && (
        <div className="mb-4 p-3 bg-gray-700 rounded-lg">
          <div className="text-white font-medium mb-1">
            Paso {currentStepData.step}: {currentStepData.action.toUpperCase()}
          </div>
          <div className="text-gray-300 text-sm">
            {currentStepData.description}
          </div>
        </div>
      )}

      {/* Map */}
      <div className="h-96 rounded-lg overflow-x-auto border border-gray-600">
        <MapContainer
          center={mapCenter}
          zoom={mapZoom}
          style={{ height: '100%', width: '100%' }}
          zoomControl={true}
        >
          <TileLayer
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
          />
          
          {/* Node markers */}
          {nodeMarkers}
          
          {/* Route polyline */}
          {routePolyline}
          
          {/* Exploration lines */}
          {explorationLines}
        </MapContainer>
      </div>

      {/* Legend */}
      <div className="mt-4 grid grid-cols-2 md:grid-cols-4 gap-3">
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 bg-gray-500 rounded-full"></div>
          <span className="text-gray-300 text-sm">No visitado</span>
        </div>
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 bg-yellow-500 rounded-full"></div>
          <span className="text-gray-300 text-sm">Frontera</span>
        </div>
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 bg-green-500 rounded-full"></div>
          <span className="text-gray-300 text-sm">Explorado</span>
        </div>
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 bg-red-500 rounded-full"></div>
          <span className="text-gray-300 text-sm">Actual</span>
        </div>
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 bg-blue-500 rounded-full"></div>
          <span className="text-gray-300 text-sm">Ruta final</span>
        </div>
      </div>

      {/* Algorithm Statistics */}
      {currentStepData && (
        <div className="mt-4 grid grid-cols-3 gap-4 text-center">
          <div className="bg-gray-700 rounded-lg p-3">
            <div className="text-gray-300 text-sm">Nodos Explorados</div>
            <div className="text-white font-bold text-lg">
              {currentStepData.exploredNodes.length}
            </div>
          </div>
          <div className="bg-gray-700 rounded-lg p-3">
            <div className="text-gray-300 text-sm">Frontera</div>
            <div className="text-white font-bold text-lg">
              {currentStepData.frontier.length}
            </div>
          </div>
          <div className="bg-gray-700 rounded-lg p-3">
            <div className="text-gray-300 text-sm">Distancias Conocidas</div>
            <div className="text-white font-bold text-lg">
              {Object.keys(currentStepData.distances).length}
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default VisualAlgorithmMap;
