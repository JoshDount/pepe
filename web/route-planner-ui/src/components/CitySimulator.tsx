/**
 * City Simulator Component
 * Realistic route planning using real city data
 */

import { useState, useEffect, useCallback, FC } from 'react';
import { useLoadGraph, useCalculateRoute, useClearRoute, useGraphState } from '../store';
import { styleTokens } from '../styles/tokens';
import CityStats from './CityStats';
import CityMap from './CityMap';
import EducationalPanel from './EducationalPanel';
import DemoExamples from './DemoExamples';
import AlgorithmVisualization from './AlgorithmVisualization';

interface CityNode {
  id: number;
  lat: number;
  lon: number;
  elevation?: number | null;
}

interface CitySimulatorProps {
  onRouteCalculated?: (result: any) => void;
}

const CitySimulator: FC<CitySimulatorProps> = ({ onRouteCalculated }) => {
  const loadGraph = useLoadGraph();
  const calculateRoute = useCalculateRoute();
  const clearRoute = useClearRoute();
  const { loaded } = useGraphState();
  
  const [cityNodes, setCityNodes] = useState<CityNode[]>([]);
  const [loading, setLoading] = useState(false);
  const [selectedSource, setSelectedSource] = useState<number | null>(null);
  const [selectedTarget, setSelectedTarget] = useState<number | null>(null);
  const [searchRadius, setSearchRadius] = useState(1000); // meters
  const [nearbyNodes, setNearbyNodes] = useState<CityNode[]>([]);
  const [selectedLocation, setSelectedLocation] = useState<{lat: number, lon: number} | null>(null);
  const [calculatedRoute, setCalculatedRoute] = useState<CityNode[]>([]);
  const [streetEdges, setStreetEdges] = useState<Array<{from: number, to: number, weight: number, street_name: string}>>([]);
  
  // Educational metrics
  const [executionTime, setExecutionTime] = useState(0);
  const [nodesExplored, setNodesExplored] = useState(0);
  const [algorithmUsed, setAlgorithmUsed] = useState<'dijkstra' | 'astar' | 'bfs' | 'dfs'>('dijkstra');
  const [demoMode, setDemoMode] = useState(false);
  const [algorithmSteps, setAlgorithmSteps] = useState<Array<{
    step: number;
    action: string;
    currentNode: number;
    exploredNodes: number[];
    frontier: number[];
    distances: Record<number, number>;
    description: string;
    timestamp: number;
  }>>([]);

  // Load city nodes from JSON file
  const loadCityData = useCallback(async () => {
    setLoading(true);
    try {
      // Try to load the real streets dataset first
      let response = await fetch('/calles_culiacan_centro.json');
      if (!response.ok) {
        // Fallback to original nodes dataset
        response = await fetch('/nodos_ciudad.json');
        if (!response.ok) {
          throw new Error('Failed to load city data');
        }
        const nodes: CityNode[] = await response.json();
        setCityNodes(nodes);
        console.log(`Loaded ${nodes.length} city nodes (fallback dataset)`);
      } else {
        const data = await response.json();
        setCityNodes(data.nodes);
        setStreetEdges(data.edges);
        console.log(`Loaded ${data.nodes.length} city nodes with ${data.edges.length} real street connections`);
        console.log('🗺️ Using real street data from OpenStreetMap!');
      }
    } catch (error) {
      console.error('Error loading city data:', error);
      alert('Error al cargar los datos de la ciudad. Asegúrate de que el archivo de calles esté en la carpeta public.');
    } finally {
      setLoading(false);
    }
  }, []);

  // Calculate distance between two points using Haversine formula
  const calculateDistance = useCallback((lat1: number, lon1: number, lat2: number, lon2: number): number => {
    const R = 6371000; // Earth's radius in meters
    const dLat = (lat2 - lat1) * Math.PI / 180;
    const dLon = (lon2 - lon1) * Math.PI / 180;
    const a = Math.sin(dLat/2) * Math.sin(dLat/2) +
              Math.cos(lat1 * Math.PI / 180) * Math.cos(lat2 * Math.PI / 180) *
              Math.sin(dLon/2) * Math.sin(dLon/2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
    return R * c;
  }, []);

  // Find nearby nodes within search radius
  const findNearbyNodes = useCallback((lat: number, lon: number, radius: number) => {
    const nearby = cityNodes.filter(node => {
      const distance = calculateDistance(lat, lon, node.lat, node.lon);
      return distance <= radius;
    }).sort((a, b) => {
      const distA = calculateDistance(lat, lon, a.lat, a.lon);
      const distB = calculateDistance(lat, lon, b.lat, b.lon);
      return distA - distB;
    }).slice(0, 10); // Limit to 10 closest nodes
    
    setNearbyNodes(nearby);
  }, [cityNodes, calculateDistance]);

  // Handle map click to select location
  const handleMapClick = useCallback((lat: number, lon: number) => {
    setSelectedLocation({ lat, lon });
    findNearbyNodes(lat, lon, searchRadius);
  }, [findNearbyNodes, searchRadius]);

  // Load city data on component mount
  useEffect(() => {
    loadCityData();
  }, [loadCityData]);

  // Create graph from city nodes
  const createCityGraph = useCallback(async () => {
    if (cityNodes.length === 0) return;
    
    setLoading(true);
    try {
      // For demo purposes, we'll create a simplified graph
      // In a real implementation, you'd create edges based on road connectivity
      await loadGraph({
        nodes: Math.min(cityNodes.length, 1000), // Limit for performance
        edges: 0, // Will be calculated based on proximity
        directed: false,
      });
      
      console.log('City graph created successfully');
    } catch (error) {
      console.error('Error creating city graph:', error);
    } finally {
      setLoading(false);
    }
  }, [cityNodes, loadGraph]);

  // Create a street-based graph from real street data
  const createRealStreetGraph = useCallback((nodes: CityNode[], edges: Array<{from: number, to: number, weight: number, street_name: string}>) => {
    const graph = new Map<number, { node: CityNode; neighbors: Array<{ node: CityNode; distance: number; street_name: string }> }>();
    
    // Initialize graph with all nodes
    nodes.forEach(node => {
      graph.set(node.id, { node, neighbors: [] });
    });
    
    // Add real street connections
    edges.forEach(edge => {
      const fromEntry = graph.get(edge.from);
      const toEntry = graph.get(edge.to);
      
      if (fromEntry && toEntry) {
        fromEntry.neighbors.push({ 
          node: toEntry.node, 
          distance: edge.weight,
          street_name: edge.street_name
        });
      }
    });
    
    console.log('🗺️ Real street graph created:', {
      totalNodes: graph.size,
      totalEdges: edges.length,
      avgConnections: Array.from(graph.values()).reduce((sum, entry) => sum + entry.neighbors.length, 0) / graph.size
    });
    
    return graph;
  }, []);

  // Create a street-based graph from city nodes (fallback for old dataset)
  const createStreetGraph = useCallback((nodes: CityNode[]) => {
    const graph = new Map<number, { node: CityNode; neighbors: Array<{ node: CityNode; distance: number }> }>();
    
    // Initialize graph with all nodes
    nodes.forEach(node => {
      graph.set(node.id, { node, neighbors: [] });
    });
    
    // Connect nearby nodes (within reasonable walking/driving distance)
    // This is a simplified approach - ideally you'd have real street connectivity data
    const maxConnectionDistance = 100; // meters - reduced for more realistic connections
    
    nodes.forEach(node => {
      const nodeEntry = graph.get(node.id)!;
      
      // Find nearby nodes
      const nearbyNodes = nodes
        .filter(otherNode => otherNode.id !== node.id)
        .map(otherNode => ({
          node: otherNode,
          distance: calculateDistance(node.lat, node.lon, otherNode.lat, otherNode.lon)
        }))
        .filter(item => item.distance <= maxConnectionDistance)
        .sort((a, b) => a.distance - b.distance);
      
      // Connect to closest nodes in different directions (simulating street grid)
      const maxConnections = 4; // Limit connections to simulate street intersections
      const connectedDirections = new Set<string>();
      
      for (const nearby of nearbyNodes) {
        if (nodeEntry.neighbors.length >= maxConnections) break;
        
        // Calculate direction (N, S, E, W, NE, NW, SE, SW)
        const latDiff = nearby.node.lat - node.lat;
        const lonDiff = nearby.node.lon - node.lon;
        const direction = `${latDiff >= 0 ? 'N' : 'S'}${lonDiff >= 0 ? 'E' : 'W'}`;
        
        // Only connect if we don't have a connection in this direction yet
        if (!connectedDirections.has(direction)) {
          nodeEntry.neighbors.push({ node: nearby.node, distance: nearby.distance });
          connectedDirections.add(direction);
        }
      }
    });
    
    console.log('🗺️ Street graph created (fallback):', {
      totalNodes: graph.size,
      avgConnections: Array.from(graph.values()).reduce((sum, entry) => sum + entry.neighbors.length, 0) / graph.size
    });
    
    return graph;
  }, [calculateDistance]);

  // Dijkstra's algorithm for street-based routing with visualization steps
  const findStreetRoute = useCallback((graph: Map<number, any>, sourceId: number, targetId: number): CityNode[] => {
    const distances = new Map<number, number>();
    const previous = new Map<number, number>();
    const unvisited = new Set<number>();
    const steps: Array<{
      step: number;
      action: string;
      currentNode: number;
      exploredNodes: number[];
      frontier: number[];
      distances: Record<number, number>;
      description: string;
      timestamp: number;
    }> = [];
    
    let stepCounter = 0;
    
    // Initialize distances
    graph.forEach((_, nodeId) => {
      distances.set(nodeId, nodeId === sourceId ? 0 : Infinity);
      unvisited.add(nodeId);
    });
    
    // Step 1: Initialization
    steps.push({
      step: ++stepCounter,
      action: 'initialize',
      currentNode: sourceId,
      exploredNodes: [],
      frontier: [sourceId],
      distances: { [sourceId]: 0 },
      description: 'Inicializando: distancia del origen = 0, todos los demás = ∞',
      timestamp: Date.now()
    });
    
    while (unvisited.size > 0) {
      // Find unvisited node with minimum distance
      let currentId = -1;
      let minDistance = Infinity;
      
      for (const nodeId of unvisited) {
        const distance = distances.get(nodeId)!;
        if (distance < minDistance) {
          minDistance = distance;
          currentId = nodeId;
        }
      }
      
      if (currentId === -1) break;
      
      // Step: Select minimum distance node
      const exploredNodes = Array.from(graph.keys()).filter(id => !unvisited.has(id));
      const frontier = Array.from(unvisited);
      
      // Create a clean distances object with only finite values
      const cleanDistances: Record<number, number> = {};
      distances.forEach((dist, nodeId) => {
        if (dist !== Infinity) {
          cleanDistances[nodeId] = dist;
        }
      });
      
      steps.push({
        step: ++stepCounter,
        action: 'select_min',
        currentNode: currentId,
        exploredNodes,
        frontier,
        distances: cleanDistances,
        description: `Seleccionando nodo con menor distancia: ${currentId} (distancia: ${Math.round(minDistance)}m)`,
        timestamp: Date.now()
      });
      
      if (currentId === targetId) {
        // Found target
        // Create a clean distances object with only finite values
        const cleanDistances: Record<number, number> = {};
        distances.forEach((dist, nodeId) => {
          if (dist !== Infinity) {
            cleanDistances[nodeId] = dist;
          }
        });
        
        steps.push({
          step: ++stepCounter,
          action: 'found_target',
          currentNode: currentId,
          exploredNodes: Array.from(graph.keys()).filter(id => !unvisited.has(id)),
          frontier: [],
          distances: cleanDistances,
          description: '¡Destino encontrado! Reconstruyendo camino óptimo',
          timestamp: Date.now()
        });
        break;
      }
      
      unvisited.delete(currentId);
      const currentEntry = graph.get(currentId);
      
      if (!currentEntry) continue;
      
      // Step: Explore neighbors
      const updatedNeighbors: number[] = [];
      for (const neighbor of currentEntry.neighbors) {
        const neighborId = neighbor.node.id;
        if (!unvisited.has(neighborId)) continue;
        
        const newDistance = distances.get(currentId)! + neighbor.distance;
        const currentDistance = distances.get(neighborId)!;
        
        if (newDistance < currentDistance) {
          distances.set(neighborId, newDistance);
          previous.set(neighborId, currentId);
          updatedNeighbors.push(neighborId);
        }
      }
      
      if (updatedNeighbors.length > 0) {
        // Create a clean distances object with only finite values
        const cleanDistances: Record<number, number> = {};
        distances.forEach((dist, nodeId) => {
          if (dist !== Infinity) {
            cleanDistances[nodeId] = dist;
          }
        });
        
        steps.push({
          step: ++stepCounter,
          action: 'explore',
          currentNode: currentId,
          exploredNodes: Array.from(graph.keys()).filter(id => !unvisited.has(id)),
          frontier: Array.from(unvisited),
          distances: cleanDistances,
          description: `Explorando nodo ${currentId}: actualizando ${updatedNeighbors.length} vecinos`,
          timestamp: Date.now()
        });
      }
    }
    
    // Store steps for visualization
    setAlgorithmSteps(steps);
    
    // Reconstruct path
    const path: CityNode[] = [];
    let currentId = targetId;
    
    while (currentId !== undefined) {
      const nodeEntry = graph.get(currentId);
      if (nodeEntry) {
        path.unshift(nodeEntry.node);
      }
      currentId = previous.get(currentId)!;
    }
    
    // Check if path was found
    if (path.length === 0 || path[0].id !== sourceId) {
      console.log('❌ No path found between nodes');
      return [];
    }
    
    console.log('✅ Street route found:', {
      pathLength: path.length,
      totalDistance: distances.get(targetId) || 0,
      path: path.map(node => node.id),
      stepsGenerated: steps.length
    });
    
    return path;
  }, []);

  // Calculate route between selected nodes
  const handleCalculateRoute = useCallback(async () => {
    if (!selectedSource || !selectedTarget) {
      alert('Por favor selecciona nodos de origen y destino');
      return;
    }

    try {
      // Find source and target nodes
      const sourceNode = cityNodes.find(node => node.id === selectedSource);
      const targetNode = cityNodes.find(node => node.id === selectedTarget);
      
      if (!sourceNode || !targetNode) {
        alert('No se encontraron los nodos seleccionados');
        return;
      }

      console.log('🚀 Starting street-based route calculation...');
      
      // Start timing for educational metrics
      const startTime = performance.now();
      
      // Create street graph - use real edges if available, otherwise fallback
      let streetGraph;
      if (streetEdges.length > 0) {
        console.log('🗺️ Using real street connections from OpenStreetMap');
        streetGraph = createRealStreetGraph(cityNodes, streetEdges);
      } else {
        console.log('⚠️ Using fallback proximity-based connections');
        streetGraph = createStreetGraph(cityNodes);
      }
      
      // Find route using Dijkstra
      const route = findStreetRoute(streetGraph, selectedSource, selectedTarget);
      
      // Calculate educational metrics
      const endTime = performance.now();
      const executionTime = endTime - startTime;
      setExecutionTime(executionTime);
      
      // Estimate nodes explored (in a real implementation, this would be tracked by the algorithm)
      const estimatedExplored = Math.min(streetGraph.size, route.length * 3);
      setNodesExplored(estimatedExplored);
      
      if (route.length === 0) {
        alert('No se pudo encontrar una ruta siguiendo las calles. Intenta con nodos más cercanos.');
        return;
      }
      
      setCalculatedRoute(route);
      
      console.log('🛣️ Street-based route calculated:', route);
      
      // Also try to update the store for compatibility
      await calculateRoute({
        source: selectedSource,
        target: selectedTarget,
        algo: 'dijkstra',
        metric: 'distance',
        debug: true,
      });
      
      onRouteCalculated?.({ source: selectedSource, target: selectedTarget });
    } catch (error) {
      console.error('Error calculating route:', error);
    }
  }, [selectedSource, selectedTarget, cityNodes, streetEdges, createRealStreetGraph, createStreetGraph, findStreetRoute, calculateRoute, onRouteCalculated]);

  // Handle demo example selection
  const handleDemoExample = useCallback((sourceId: number, targetId: number, description: string) => {
    setSelectedSource(sourceId);
    setSelectedTarget(targetId);
    setDemoMode(true);
    console.log(`🎓 Demo example selected: ${description}`);
  }, []);

  // Get city bounds for map centering
  const getCityBounds = useCallback(() => {
    if (cityNodes.length === 0) return null;
    
    const lats = cityNodes.map(node => node.lat);
    const lons = cityNodes.map(node => node.lon);
    
    return {
      minLat: Math.min(...lats),
      maxLat: Math.max(...lats),
      minLon: Math.min(...lons),
      maxLon: Math.max(...lons),
      centerLat: (Math.min(...lats) + Math.max(...lats)) / 2,
      centerLon: (Math.min(...lons) + Math.max(...lons)) / 2,
    };
  }, [cityNodes]);

  const bounds = getCityBounds();

  return (
    <div className="space-y-6">
      {/* Demo Mode Indicator */}
      {demoMode && (
        <div className="bg-blue-600/20 border border-blue-500/30 rounded-lg p-4">
          <div className="flex items-center gap-3">
            <div className="w-3 h-3 bg-blue-500 rounded-full animate-pulse"></div>
            <div className="text-blue-300 font-medium">🎓 Modo Demostración</div>
            <div className="text-blue-400 text-sm">Ejemplo educativo seleccionado</div>
          </div>
        </div>
      )}

      {/* Educational Components Row */}
      <div className="grid grid-cols-1 xl:grid-cols-3 gap-6">
        {/* Educational Panel */}
        <div className="xl:col-span-1">
          <EducationalPanel
            algorithm={algorithmUsed}
            isCalculating={loading}
            routeFound={calculatedRoute.length > 0}
            routeLength={calculatedRoute.length}
            totalDistance={calculatedRoute.reduce((total, node, i) => {
              if (i === 0) return 0;
              return total + calculateDistance(
                calculatedRoute[i-1].lat, calculatedRoute[i-1].lon,
                node.lat, node.lon
              );
            }, 0)}
            executionTime={executionTime}
            nodesExplored={nodesExplored}
          />
        </div>

        {/* Demo Examples */}
        <div className="xl:col-span-1">
          <DemoExamples
            onSelectExample={handleDemoExample}
            availableNodes={cityNodes}
          />
        </div>

        {/* Algorithm Visualization */}
        <div className="xl:col-span-1">
          <AlgorithmVisualization
            isCalculating={loading}
            routeFound={calculatedRoute.length > 0}
            routeNodes={calculatedRoute.map(node => node.id)}
            algorithmSteps={algorithmSteps}
            cityNodes={cityNodes}
            calculatedRoute={calculatedRoute}
          />
        </div>
      </div>

      {/* Main Simulation - Three Layer Design */}
      <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 min-h-[700px]">
        
        {/* Layer 1: Compact Controls (25%) */}
        <div className="lg:col-span-3 space-y-4">
          {/* Unified Control Panel */}
          <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
            <h3 className="text-lg font-bold text-white mb-4">🎛️ Controles</h3>
            
            {/* Data Status */}
            <div className="mb-4 p-3 bg-gray-700/50 rounded-lg">
              <div className="flex justify-between items-center mb-2">
                <span className="text-gray-300 text-sm">Nodos:</span>
                <span className="text-white font-mono text-sm">{cityNodes.length.toLocaleString()}</span>
              </div>
              <div className="flex justify-between items-center mb-3">
                <span className="text-gray-300 text-sm">Estado:</span>
                <span className={`text-sm font-medium ${loading ? 'text-yellow-400' : 'text-green-400'}`}>
                  {loading ? 'Cargando...' : 'Cargado'}
                </span>
              </div>
              <div className="flex gap-2">
                <button
                  onClick={loadCityData}
                  disabled={loading}
                  className="flex-1 bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 text-white px-3 py-2 rounded-lg font-medium transition-colors text-sm"
                >
                  {loading ? 'Cargando...' : 'Recargar'}
                </button>
                <button
                  onClick={createCityGraph}
                  disabled={cityNodes.length === 0 || loading}
                  className="flex-1 bg-green-600 hover:bg-green-700 disabled:bg-gray-600 text-white px-3 py-2 rounded-lg font-medium transition-colors text-sm"
                >
                  Crear Grafo
                </button>
              </div>
            </div>

            {/* Route Planning */}
            <div className="space-y-4">
              <div>
                <label className="block text-gray-300 text-sm mb-2">Nodo de Origen</label>
                <div className="flex gap-2">
                  <input
                    type="number"
                    placeholder="ID del nodo"
                    value={selectedSource || ''}
                    onChange={(e) => setSelectedSource(parseInt(e.target.value) || null)}
                    className="flex-1 bg-gray-700 border border-gray-600 rounded-lg px-3 py-2 text-white text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                  />
                  <button
                    onClick={() => {
                      const randomNode = cityNodes[Math.floor(Math.random() * cityNodes.length)];
                      setSelectedSource(randomNode.id);
                    }}
                    className="px-3 py-2 bg-gray-600 hover:bg-gray-700 text-white rounded-lg text-sm transition-colors"
                    title="Nodo aleatorio"
                  >
                    🎲
                  </button>
                </div>
              </div>

              <div>
                <label className="block text-gray-300 text-sm mb-2">Nodo de Destino</label>
                <div className="flex gap-2">
                  <input
                    type="number"
                    placeholder="ID del nodo"
                    value={selectedTarget || ''}
                    onChange={(e) => setSelectedTarget(parseInt(e.target.value) || null)}
                    className="flex-1 bg-gray-700 border border-gray-600 rounded-lg px-3 py-2 text-white text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                  />
                  <button
                    onClick={() => {
                      const randomNode = cityNodes[Math.floor(Math.random() * cityNodes.length)];
                      setSelectedTarget(randomNode.id);
                    }}
                    className="px-3 py-2 bg-gray-600 hover:bg-gray-700 text-white rounded-lg text-sm transition-colors"
                    title="Nodo aleatorio"
                  >
                    🎲
                  </button>
                </div>
              </div>

              <button
                onClick={handleCalculateRoute}
                disabled={!selectedSource || !selectedTarget || !loaded}
                className="w-full bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 text-white px-4 py-3 rounded-lg font-medium transition-colors"
              >
                {loading ? 'Calculando...' : 'Calcular Ruta'}
              </button>

              {calculatedRoute.length > 0 && (
                <div className="bg-green-600/20 border border-green-500/30 rounded-lg p-3">
                  <div className="text-green-300 text-sm font-medium mb-1">
                    ✅ Ruta calculada
                  </div>
                  <div className="text-green-400 text-xs">
                    {calculatedRoute.length} nodos • {Math.round(calculatedRoute.reduce((total, node, i) => {
                      if (i === 0) return 0;
                      return total + calculateDistance(
                        calculatedRoute[i-1].lat, calculatedRoute[i-1].lon,
                        node.lat, node.lon
                      );
                    }, 0))}m total
                  </div>
                </div>
              )}
              
              <button
                onClick={() => {
                  setSelectedSource(null);
                  setSelectedTarget(null);
                  setCalculatedRoute([]);
                  setDemoMode(false);
                  setAlgorithmSteps([]);
                  clearRoute();
                }}
                className="w-full bg-gray-600 hover:bg-gray-700 text-white px-4 py-2 rounded-lg font-medium transition-colors text-sm"
              >
                Limpiar
              </button>
            </div>
          </div>
        </div>

        {/* Layer 2: Central Map (50%) - Primary Focus */}
        <div className="lg:col-span-6">
          <div className="h-full">
            <CityMap 
              nodes={cityNodes}
              selectedSource={selectedSource}
              selectedTarget={selectedTarget}
              calculatedRoute={calculatedRoute}
              onNodeClick={(node) => {
                if (!selectedSource) {
                  setSelectedSource(node.id);
                } else if (!selectedTarget) {
                  setSelectedTarget(node.id);
                }
              }}
              onMapClick={handleMapClick}
            />
          </div>
        </div>

        {/* Layer 3: Compact Stats (25%) */}
        <div className="lg:col-span-3 space-y-4">
          {/* City Statistics */}
          <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
            <h3 className="text-lg font-bold text-white mb-4">📊 Estadísticas</h3>
            
            <div className="space-y-3">
              <div className="flex justify-between items-center">
                <span className="text-gray-300 text-sm">Total de nodos:</span>
                <span className="text-white font-mono text-sm">{cityNodes.length.toLocaleString()}</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-gray-300 text-sm">Área aproximada:</span>
                <span className="text-white font-mono text-sm">15.58 km²</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-gray-300 text-sm">Densidad:</span>
                <span className="text-white font-mono text-sm">127.3 nodos/km²</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-gray-300 text-sm">Centro:</span>
                <span className="text-white font-mono text-xs">24.784, -107.387</span>
              </div>
            </div>
          </div>

          {/* Nearby Nodes */}
          {nearbyNodes.length > 0 && (
            <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
              <h3 className="text-lg font-bold text-white mb-3">📍 Nodos Cercanos</h3>
              <div className="space-y-2 max-h-64 overflow-y-auto">
                {nearbyNodes.slice(0, 8).map((node) => (
                  <div
                    key={node.id}
                    className="bg-gray-700 rounded-lg p-2 cursor-pointer hover:bg-gray-600 transition-colors"
                    onClick={() => {
                      if (!selectedSource) {
                        setSelectedSource(node.id);
                      } else if (!selectedTarget) {
                        setSelectedTarget(node.id);
                      }
                    }}
                  >
                    <div className="text-white font-mono text-xs">ID: {node.id}</div>
                    <div className="text-gray-400 text-xs">
                      {node.lat.toFixed(4)}, {node.lon.toFixed(4)}
                    </div>
                  </div>
                ))}
              </div>
            </div>
          )}

          {/* Route Info */}
          {calculatedRoute.length > 0 && (
            <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4">
              <h3 className="text-lg font-bold text-white mb-3">🛣️ Información de Ruta</h3>
              <div className="space-y-2">
                <div className="flex justify-between items-center">
                  <span className="text-gray-300 text-sm">Distancia total:</span>
                  <span className="text-white font-mono text-sm">
                    {Math.round(calculatedRoute.reduce((total, node, i) => {
                      if (i === 0) return 0;
                      return total + calculateDistance(
                        calculatedRoute[i-1].lat, calculatedRoute[i-1].lon,
                        node.lat, node.lon
                      );
                    }, 0))}m
                  </span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-gray-300 text-sm">Nodos en ruta:</span>
                  <span className="text-white font-mono text-sm">{calculatedRoute.length}</span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-gray-300 text-sm">Tipo de datos:</span>
                  <span className="text-xs text-blue-400">
                    {streetEdges.length > 0 ? 'Calles reales' : 'Aproximado'}
                  </span>
                </div>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default CitySimulator;
