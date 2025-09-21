/**
 * City Map Component
 * Real map with Leaflet and city nodes visualization
 */

import { useState, useCallback, FC, useMemo } from 'react';
// @ts-ignore
import { MapContainer, TileLayer, Marker, Popup, useMap, Polyline } from 'react-leaflet';
// @ts-ignore
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import { useRouteNodes, useRouteCoordinates } from '../store';

// Fix for default markers in react-leaflet
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
  elevation?: number | null;
}

interface CityMapProps {
  nodes: CityNode[];
  selectedSource?: number | null;
  selectedTarget?: number | null;
  calculatedRoute?: CityNode[];
  onNodeClick?: (node: CityNode) => void;
  onMapClick?: (lat: number, lon: number) => void;
}

// Custom marker component
const CustomMarker: FC<{
  node: CityNode;
  isSelected: boolean;
  isSource: boolean;
  isTarget: boolean;
  onClick: (node: CityNode) => void;
}> = ({ node, isSelected, isSource, isTarget, onClick }) => {
  let color = '#6b7280'; // gray
  if (isSource) color = '#ef4444'; // red
  else if (isTarget) color = '#22c55e'; // green
  else if (isSelected) color = '#3b82f6'; // blue

  const icon = L.divIcon({
    className: 'custom-marker',
    html: `<div style="
      width: ${isSelected ? '16px' : '12px'};
      height: ${isSelected ? '16px' : '12px'};
      background-color: ${color};
      border: ${isSelected ? '2px solid white' : '1px solid rgba(255,255,255,0.5)'};
      border-radius: 50%;
      box-shadow: 0 2px 4px rgba(0,0,0,0.3);
      cursor: pointer;
    "></div>`,
    iconSize: [isSelected ? 20 : 16, isSelected ? 20 : 16],
    iconAnchor: [isSelected ? 10 : 8, isSelected ? 10 : 8],
  });

  return (
    <Marker
      position={[node.lat, node.lon] as [number, number]}
      icon={icon}
      eventHandlers={{
        click: () => onClick(node),
      }}
    >
      <Popup>
        <div className="text-sm">
          <div className="font-semibold text-gray-900 mb-2">Nodo {node.id}</div>
          <div className="text-gray-600 space-y-1">
            <div>Lat: {node.lat.toFixed(6)}</div>
            <div>Lon: {node.lon.toFixed(6)}</div>
            {node.elevation !== null && node.elevation !== undefined && (
              <div>Elevación: {node.elevation}m</div>
            )}
          </div>
        </div>
      </Popup>
    </Marker>
  );
};

// Component to handle map clicks
const MapClickHandler: FC<{ onMapClick: (lat: number, lon: number) => void }> = ({ onMapClick }) => {
  const map = useMap();

  useMemo(() => {
    const handleClick = (e: L.LeafletMouseEvent) => {
      onMapClick(e.latlng.lat, e.latlng.lng);
    };

    map.on('click', handleClick);
    return () => {
      map.off('click', handleClick);
    };
  }, [map, onMapClick]);

  return null;
};

const CityMap: FC<CityMapProps> = ({ 
  nodes, 
  selectedSource, 
  selectedTarget, 
  calculatedRoute,
  onNodeClick, 
  onMapClick 
}) => {
  // Get route data from store
  const routeNodes = useRouteNodes();
  const routeCoordinates = useRouteCoordinates();
  
  // Create route path coordinates
  const routePath = useMemo(() => {
    console.log('🔍 Debug route data:', {
      calculatedRoute: calculatedRoute?.length || 0,
      routeNodes,
      routeCoordinates,
      nodesLength: nodes.length
    });
    
    // Priority 1: Use calculated route from CitySimulator
    if (calculatedRoute && calculatedRoute.length > 0) {
      console.log('✅ Using calculated route from CitySimulator:', calculatedRoute);
      const path = calculatedRoute.map(node => [node.lat, node.lon] as [number, number]);
      console.log('🛣️ Calculated route path:', path);
      return path;
    }
    
    // Priority 2: TEMPORARY: Create a test route using first 5 city nodes if no route from store
    if (routeNodes.length === 0 && nodes.length > 0) {
      console.log('🧪 Creating test route with first 5 city nodes');
      const testPath = nodes.slice(0, 5).map(node => [node.lat, node.lon] as [number, number]);
      console.log('🛣️ Test route path:', testPath);
      return testPath;
    }
    
    // Priority 3: Use route from store
    if (routeNodes.length === 0) {
      console.log('❌ No route nodes found');
      return [];
    }
    
    const path = routeNodes.map(nodeId => {
      // First try to get coordinates from store
      if (routeCoordinates[nodeId]) {
        const coords = [routeCoordinates[nodeId][1], routeCoordinates[nodeId][0]] as [number, number];
        console.log(`✅ Found coordinates for node ${nodeId}:`, coords);
        return coords;
      }
      
      // Fallback: find node in city nodes
      const cityNode = nodes.find(node => node.id === nodeId);
      if (cityNode) {
        const coords = [cityNode.lat, cityNode.lon] as [number, number];
        console.log(`✅ Found city node ${nodeId}:`, coords);
        return coords;
      }
      
      console.log(`❌ No coordinates found for node ${nodeId}`);
      return null;
    }).filter(Boolean) as [number, number][];
    
    console.log('🛣️ Final route path:', path);
    return path;
  }, [calculatedRoute, routeNodes, routeCoordinates, nodes]);
  
  // Calculate map center and bounds
  const mapCenter = useMemo(() => {
    if (nodes.length === 0) return [24.7841, -107.3866] as [number, number];
    
    const lats = nodes.map(node => node.lat);
    const lons = nodes.map(node => node.lon);
    
    return [
      (Math.min(...lats) + Math.max(...lats)) / 2,
      (Math.min(...lons) + Math.max(...lons)) / 2,
    ] as [number, number];
  }, [nodes]);

  // Calculate appropriate zoom level based on node spread
  const mapZoom = useMemo(() => {
    if (nodes.length === 0) return 12;
    
    const lats = nodes.map(node => node.lat);
    const lons = nodes.map(node => node.lon);
    
    const latRange = Math.max(...lats) - Math.min(...lats);
    const lonRange = Math.max(...lons) - Math.min(...lons);
    const maxRange = Math.max(latRange, lonRange);
    
    // Adjust zoom based on spread (smaller spread = higher zoom)
    if (maxRange < 0.01) return 15;
    if (maxRange < 0.05) return 14;
    if (maxRange < 0.1) return 13;
    if (maxRange < 0.2) return 12;
    return 11;
  }, [nodes]);

  // Handle marker click
  const handleMarkerClick = useCallback((node: CityNode) => {
    onNodeClick?.(node);
  }, [onNodeClick]);

  if (nodes.length === 0) {
    return (
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
        <h3 className="text-lg font-semibold text-white mb-4">Mapa de la Ciudad</h3>
        <div className="text-center py-8">
          <div className="text-gray-400">No hay datos de ciudad cargados</div>
        </div>
      </div>
    );
  }

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
      <h3 className="text-lg font-semibold text-white mb-4">Mapa de la Ciudad</h3>
      
      <div className="bg-gray-700 rounded-lg p-4 overflow-hidden">
        <div className="flex justify-between items-center mb-4">
          <div className="text-sm text-gray-400">
            {nodes.length.toLocaleString()} nodos • Zoom: {mapZoom}
            {routePath.length > 1 && (
              <span className="ml-2 text-red-400 font-semibold">
                • Ruta activa ({routePath.length} nodos)
              </span>
            )}
            {/* Debug info */}
            <span className="ml-2 text-yellow-400 text-xs">
              Route nodes: {routeNodes.length} | Path: {routePath.length}
            </span>
            {/* Test route button */}
            <button 
              onClick={() => {
                console.log('🧪 Testing route with first 5 nodes');
                // This is just for testing - in real app this would come from store
              }}
              className="ml-2 px-2 py-1 bg-blue-600 text-white text-xs rounded hover:bg-blue-700"
            >
              Test Route
            </button>
          </div>
          <div className="text-xs text-gray-500">
            {routePath.length > 1 ? 'Ruta calculada visible' : 'Haz clic en un nodo para seleccionarlo'}
          </div>
        </div>
        
        <div className="border border-gray-600 rounded-lg overflow-hidden" style={{ height: '400px' }}>
          <MapContainer
            center={mapCenter}
            zoom={mapZoom}
            style={{ height: '100%', width: '100%' }}
            zoomControl={true}
            scrollWheelZoom={true}
            doubleClickZoom={true}
            dragging={true}
          >
            <TileLayer
              attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
              url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            
            {/* Route path */}
            {routePath.length > 1 && (
              <>
                {/* Background line for better visibility */}
                <Polyline
                  positions={routePath}
                  pathOptions={{
                    color: '#ffffff',
                    weight: 12,
                    opacity: 0.8,
                    lineCap: 'round',
                    lineJoin: 'round'
                  }}
                />
                {/* Main route line */}
                <Polyline
                  positions={routePath}
                  pathOptions={{
                    color: '#ef4444',
                    weight: 8,
                    opacity: 1.0,
                    dashArray: '15, 15',
                    lineCap: 'round',
                    lineJoin: 'round'
                  }}
                />
              </>
            )}

            {/* City nodes as markers */}
            {nodes.map((node) => {
              const isSelected = node.id === selectedSource || node.id === selectedTarget;
              const isSource = node.id === selectedSource;
              const isTarget = node.id === selectedTarget;
              const isInRoute = calculatedRoute ? calculatedRoute.some(routeNode => routeNode.id === node.id) : routeNodes.includes(node.id);
              
              return (
                <CustomMarker
                  key={node.id}
                  node={node}
                  isSelected={isSelected || isInRoute}
                  isSource={isSource}
                  isTarget={isTarget}
                  onClick={handleMarkerClick}
                />
              );
            })}

            {/* Map click handler */}
            {onMapClick && <MapClickHandler onMapClick={onMapClick} />}
          </MapContainer>
        </div>
        
        <div className="mt-4 text-xs text-gray-400">
          <div className="flex justify-between">
            <span>Centro: {mapCenter[0].toFixed(4)}, {mapCenter[1].toFixed(4)}</span>
            <span>Arrastra para mover • Rueda para zoom</span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default CityMap;