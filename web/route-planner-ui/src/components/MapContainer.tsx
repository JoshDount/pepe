/**
 * Map Container Component
 * Interactive map visualization for routes and algorithms
 * Note: This is a basic implementation. Full MapLibre GL integration will be added later.
 */

import { useEffect, useRef, useState, FC } from 'react';
import { useGraphState, useCurrentRoute, useRouteCoordinates } from '../store';
import { styleTokens } from '../styles/tokens';

interface MapContainerProps {
  height?: string;
}

const MapContainer: FC<MapContainerProps> = ({ height = '500px' }) => {
  const mapRef = useRef<HTMLDivElement>(null);
  const { loaded, bounds } = useGraphState();
  const currentRoute = useCurrentRoute();
  const routeCoordinates = useRouteCoordinates();
  const [mapReady, setMapReady] = useState(false);

  // Placeholder data for visualization
  const [nodes] = useState(() => {
    const mockNodes = [];
    for (let i = 1; i <= 20; i++) {
      mockNodes.push({
        id: i,
        x: Math.random() * 800 + 50,
        y: Math.random() * 400 + 50,
        label: i.toString(),
      });
    }
    return mockNodes;
  });

  // Mock edges
  const [edges] = useState(() => {
    const mockEdges = [];
    for (let i = 0; i < nodes.length - 1; i++) {
      if (Math.random() > 0.3) {
        mockEdges.push({
          from: nodes[i],
          to: nodes[i + 1],
          weight: Math.random() * 10 + 1,
        });
      }
    }
    // Add some random connections
    for (let i = 0; i < 10; i++) {
      const from = nodes[Math.floor(Math.random() * nodes.length)];
      const to = nodes[Math.floor(Math.random() * nodes.length)];
      if (from !== to) {
        mockEdges.push({
          from,
          to,
          weight: Math.random() * 10 + 1,
        });
      }
    }
    return mockEdges;
  });

  useEffect(() => {
    // Simulate map initialization
    const timer = setTimeout(() => {
      setMapReady(true);
    }, 1000);

    return () => clearTimeout(timer);
  }, []);

  const isNodeInRoute = (nodeId: number): boolean => {
    return currentRoute.nodes.includes(nodeId);
  };

  const isSourceNode = (nodeId: number): boolean => {
    return currentRoute.nodes[0] === nodeId;
  };

  const isTargetNode = (nodeId: number): boolean => {
    return currentRoute.nodes[currentRoute.nodes.length - 1] === nodeId;
  };

  const getNodeStyle = (nodeId: number): string => {
    if (isSourceNode(nodeId)) return 'bg-accent-success border-accent-success';
    if (isTargetNode(nodeId)) return 'bg-accent-warning border-accent-warning';
    if (isNodeInRoute(nodeId)) return 'bg-accent-primary border-accent-primary';
    return 'bg-bg-surface border-border-default hover:border-accent-primary';
  };

  const getEdgeStyle = (from: any, to: any): string => {
    const isInRoute = isNodeInRoute(from.id) && isNodeInRoute(to.id) && 
                     Math.abs(currentRoute.nodes.indexOf(from.id) - currentRoute.nodes.indexOf(to.id)) === 1;
    return isInRoute ? 'stroke-accent-primary stroke-2' : 'stroke-border-muted stroke-1';
  };

  if (!loaded) {
    return (
      <div className={`${styleTokens.card} flex items-center justify-center`} style={{ height }}>
        <div className="text-center">
          <div className="w-16 h-16 bg-bg-elevated rounded-full flex items-center justify-center mx-auto mb-4">
            <svg className="w-8 h-8 text-text-muted" fill="currentColor" viewBox="0 0 20 20">
              <path d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4zM3 10a1 1 0 011-1h6a1 1 0 011 1v6a1 1 0 01-1 1H4a1 1 0 01-1-1v-6zM14 9a1 1 0 00-1 1v6a1 1 0 001 1h2a1 1 0 001-1v-6a1 1 0 00-1-1h-2z" />
            </svg>
          </div>
          <h3 className={styleTokens.text.heading}>No Graph Loaded</h3>
          <p className={styleTokens.text.caption}>Load a graph to visualize routes and algorithms</p>
        </div>
      </div>
    );
  }

  return (
    <div className={styleTokens.card} style={{ height }}>
      {/* Map Header */}
      <div className="flex items-center justify-between mb-4">
        <div>
          <h3 className={styleTokens.text.heading}>Route Visualization</h3>
          <p className={styleTokens.text.caption}>
            {currentRoute.nodes.length > 0 
              ? `Showing ${currentRoute.algorithm.toUpperCase()} route with ${currentRoute.nodes.length} nodes`
              : 'Interactive graph visualization'
            }
          </p>
        </div>
        
        <div className="flex items-center space-x-4">
          {/* Legend */}
          <div className="flex items-center space-x-4 text-xs">
            <div className="flex items-center space-x-1">
              <div className="w-3 h-3 bg-accent-success rounded-full" />
              <span className="text-text-secondary">Source</span>
            </div>
            <div className="flex items-center space-x-1">
              <div className="w-3 h-3 bg-accent-warning rounded-full" />
              <span className="text-text-secondary">Target</span>
            </div>
            <div className="flex items-center space-x-1">
              <div className="w-3 h-3 bg-accent-primary rounded-full" />
              <span className="text-text-secondary">Route</span>
            </div>
          </div>
        </div>
      </div>

      {/* Map Container */}
      <div 
        ref={mapRef} 
        className="relative bg-bg-elevated rounded-lg border border-border-default overflow-hidden"
        style={{ height: 'calc(100% - 80px)' }}
      >
        {!mapReady ? (
          /* Loading State */
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="text-center">
              <div className="w-8 h-8 border-2 border-accent-primary border-t-transparent rounded-full animate-spin mx-auto mb-2" />
              <p className={styleTokens.text.caption}>Loading map...</p>
            </div>
          </div>
        ) : (
          /* SVG Visualization */
          <svg className="w-full h-full" viewBox="0 0 900 500">
            {/* Grid Background */}
            <defs>
              <pattern id="grid" width="50" height="50" patternUnits="userSpaceOnUse">
                <path d="M 50 0 L 0 0 0 50" fill="none" stroke="#1A2028" strokeWidth="1" />
              </pattern>
            </defs>
            <rect width="100%" height="100%" fill="url(#grid)" />
            
            {/* Edges */}
            {edges.map((edge, index) => (
              <line
                key={index}
                x1={edge.from.x}
                y1={edge.from.y}
                x2={edge.to.x}
                y2={edge.to.y}
                className={getEdgeStyle(edge.from, edge.to)}
                strokeDasharray={isNodeInRoute(edge.from.id) && isNodeInRoute(edge.to.id) ? "0" : "3,3"}
              />
            ))}
            
            {/* Nodes */}
            {nodes.map((node) => (
              <g key={node.id}>
                <circle
                  cx={node.x}
                  cy={node.y}
                  r="15"
                  className={`${getNodeStyle(node.id)} transition-all duration-200 cursor-pointer`}
                  fill={isSourceNode(node.id) ? '#10B981' : isTargetNode(node.id) ? '#F59E0B' : isNodeInRoute(node.id) ? '#B91C1C' : '#12171F'}
                  stroke={isSourceNode(node.id) ? '#10B981' : isTargetNode(node.id) ? '#F59E0B' : isNodeInRoute(node.id) ? '#B91C1C' : '#1A2028'}
                  strokeWidth="2"
                />
                <text
                  x={node.x}
                  y={node.y + 4}
                  textAnchor="middle"
                  className="text-xs font-semibold fill-white pointer-events-none"
                >
                  {node.label}
                </text>
              </g>
            ))}
            
            {/* Route Path Highlight */}
            {currentRoute.nodes.length > 1 && (
              <path
                d={`M ${nodes.find(n => n.id === currentRoute.nodes[0])?.x || 0} ${nodes.find(n => n.id === currentRoute.nodes[0])?.y || 0} ` +
                   currentRoute.nodes.slice(1).map(nodeId => {
                     const node = nodes.find(n => n.id === nodeId);
                     return `L ${node?.x || 0} ${node?.y || 0}`;
                   }).join(' ')}
                fill="none"
                stroke="#B91C1C"
                strokeWidth="3"
                strokeDasharray="5,5"
                className="animate-pulse"
              />
            )}
          </svg>
        )}
        
        {/* Map Controls */}
        <div className="absolute top-4 right-4 flex flex-col space-y-2">
          <button className="bg-bg-surface border border-border-default p-2 rounded hover:bg-bg-elevated transition-colors">
            <svg className="w-4 h-4 text-text-primary" fill="currentColor" viewBox="0 0 20 20">
              <path d="M10 3a1 1 0 011 1v5h5a1 1 0 110 2h-5v5a1 1 0 11-2 0v-5H4a1 1 0 110-2h5V4a1 1 0 011-1z" />
            </svg>
          </button>
          <button className="bg-bg-surface border border-border-default p-2 rounded hover:bg-bg-elevated transition-colors">
            <svg className="w-4 h-4 text-text-primary" fill="currentColor" viewBox="0 0 20 20">
              <path d="M5 12a1 1 0 102 0V6.414l1.293 1.293a1 1 0 001.414-1.414l-3-3a1 1 0 00-1.414 0l-3 3a1 1 0 001.414 1.414L5 6.414V12zM15 8a1 1 0 10-2 0v5.586l-1.293-1.293a1 1 0 00-1.414 1.414l3 3a1 1 0 001.414 0l3-3a1 1 0 00-1.414-1.414L15 13.586V8z" />
            </svg>
          </button>
        </div>
      </div>
      
      {/* Map Footer */}
      {currentRoute.nodes.length > 0 && (
        <div className="mt-4 p-3 bg-bg-elevated rounded-lg border border-border-muted">
          <div className="flex items-center justify-between text-sm">
            <span className="text-text-secondary">
              Route: {currentRoute.nodes.join(' → ')}
            </span>
            <span className="text-accent-primary font-medium">
              Cost: {currentRoute.cost.toFixed(2)} | Algorithm: {currentRoute.algorithm.toUpperCase()}
            </span>
          </div>
        </div>
      )}
    </div>
  );
};

export default MapContainer;
