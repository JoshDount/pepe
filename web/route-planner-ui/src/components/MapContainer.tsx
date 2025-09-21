/**
 * Map Container Component
 * Interactive map visualization for routes and algorithms
 * Note: This is a basic implementation. Full MapLibre GL integration will be added later.
 */

import { useEffect, useRef, useState, FC, useCallback } from 'react';
import { useGraphState, useGraphNodes, useGraphEdges, useRouteNodes, useRouteCost, useRouteAlgorithm } from '../store';

interface MapContainerProps { height?: string }
import { useMapHeight } from '../store';

const MapContainer: FC<MapContainerProps> = ({ height }) => {
  const mapRef = useRef<HTMLDivElement>(null);
  const { loaded } = useGraphState();
  const mapHeight = useMapHeight();
  const graphNodes = useGraphNodes();
  const graphEdges = useGraphEdges();
  const routeNodes = useRouteNodes();
  const routeCost = useRouteCost();
  const routeAlgorithm = useRouteAlgorithm();
  const [mapReady, setMapReady] = useState(false);
  const [isFullscreen, setIsFullscreen] = useState(false);
  const [scale, setScale] = useState(1);
  const [panX, setPanX] = useState(0);
  const [panY, setPanY] = useState(0);

  useEffect(() => {
    if (loaded && graphNodes.length > 0) setMapReady(true);
  }, [loaded, graphNodes]);

  const isNodeInRoute = (nodeId: number): boolean => {
    return routeNodes.includes(nodeId);
  };

  const isSourceNode = (nodeId: number): boolean => {
    return routeNodes[0] === nodeId;
  };

  const isTargetNode = (nodeId: number): boolean => {
    return routeNodes[routeNodes.length - 1] === nodeId;
  };

  const getNodeStyle = (nodeId: number): string => {
    if (isSourceNode(nodeId)) return 'bg-green-500 border-green-500';
    if (isTargetNode(nodeId)) return 'bg-yellow-500 border-yellow-500';
    if (isNodeInRoute(nodeId)) return 'bg-red-600 border-red-600';
    return 'bg-gray-800 border-gray-600 hover:border-red-600';
  };

  const routePairs = new Set<string>(
    routeNodes.length > 1
      ? routeNodes.slice(0, -1).map((u, i) => `${u}-${routeNodes[i + 1]}`)
      : []
  );

  const getEdgeStyle = (fromId: number, toId: number): string => {
    const inPath = routePairs.has(`${fromId}-${toId}`) || routePairs.has(`${toId}-${fromId}`);
    return inPath ? 'stroke-red-600 stroke-2' : 'stroke-gray-600 stroke-1';
  };

  // Controls
  const zoomIn = useCallback(() => setScale((s) => Math.min(3, s + 0.2)), []);
  const zoomOut = useCallback(() => setScale((s) => Math.max(0.5, s - 0.2)), []);
  const resetView = useCallback(() => { setScale(1); setPanX(0); setPanY(0); }, []);
  const panBy = useCallback((dx: number, dy: number) => { setPanX((x) => x + dx); setPanY((y) => y + dy); }, []);
  const toggleFullscreen = useCallback(() => setIsFullscreen((f) => !f), []);

  useEffect(() => {
    const onKey = (e: KeyboardEvent) => { if (isFullscreen && e.key === 'Escape') setIsFullscreen(false); };
    window.addEventListener('keydown', onKey);
    return () => window.removeEventListener('keydown', onKey);
  }, [isFullscreen]);

  const effectiveHeight = height || `${mapHeight}px`;

  if (!loaded) {
    return (
      <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg flex items-center justify-center" style={{ height: effectiveHeight }}>
        <div className="text-center">
          <div className="w-12 h-12 sm:w-16 sm:h-16 bg-gray-700 rounded-full flex items-center justify-center mx-auto mb-3 sm:mb-4">
            <svg className="w-6 h-6 sm:w-8 sm:h-8 text-gray-400" fill="currentColor" viewBox="0 0 20 20">
              <path d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4zM3 10a1 1 0 011-1h6a1 1 0 011 1v6a1 1 0 01-1 1H4a1 1 0 01-1-1v-6zM14 9a1 1 0 00-1 1v6a1 1 0 001 1h2a1 1 0 001-1v-6a1 1 0 00-1-1h-2z" />
            </svg>
          </div>
          <h3 className="text-lg sm:text-xl font-semibold text-white mb-2">Sin Grafo Cargado</h3>
          <p className="text-xs sm:text-sm text-gray-400">Carga un grafo para visualizar rutas y algoritmos</p>
        </div>
      </div>
    );
  }

  const svgRef = useRef<SVGSVGElement>(null);

  const onWheel = useCallback((e: React.WheelEvent<SVGSVGElement>) => {
    e.preventDefault();
    const rect = svgRef.current?.getBoundingClientRect();
    if (!rect) return;
    const mx = e.clientX - rect.left;
    const my = e.clientY - rect.top;
    const s = scale;
    const s2 = Math.max(0.5, Math.min(3, s * (e.deltaY < 0 ? 1.1 : 0.9)));
    const k = s2 / s;
    setScale(s2);
    setPanX((px) => (1 - k) * mx + k * px);
    setPanY((py) => (1 - k) * my + k * py);
  }, [scale]);

  const dragging = useRef(false);
  const lastPos = useRef<{x:number;y:number}>({x:0,y:0});
  const onMouseDown = useCallback((e: React.MouseEvent<SVGSVGElement>) => {
    dragging.current = true;
    lastPos.current = { x: e.clientX, y: e.clientY };
    window.addEventListener('mousemove', onMouseMove);
    window.addEventListener('mouseup', onMouseUp);
  }, []);
  const onMouseMove = useCallback((e: MouseEvent) => {
    if (!dragging.current) return;
    const dx = e.clientX - lastPos.current.x;
    const dy = e.clientY - lastPos.current.y;
    lastPos.current = { x: e.clientX, y: e.clientY };
    setPanX((px) => px + dx);
    setPanY((py) => py + dy);
  }, []);
  const onMouseUp = useCallback(() => {
    dragging.current = false;
    window.removeEventListener('mousemove', onMouseMove);
    window.removeEventListener('mouseup', onMouseUp);
  }, [onMouseMove]);

  const SvgContent = (
    <svg 
      ref={svgRef}
      className="w-full h-full max-w-full" 
      viewBox={`0 0 900 500`} 
      preserveAspectRatio="xMidYMid slice"
      style={{ width: '100%', height: '100%' }}
      onWheel={onWheel}
      onMouseDown={onMouseDown}
    >
      <defs>
        <pattern id="grid" width="50" height="50" patternUnits="userSpaceOnUse">
          <path d="M 50 0 L 0 0 0 50" fill="none" stroke="#374151" strokeWidth="1" />
        </pattern>
      </defs>
      <rect width="100%" height="100%" fill="url(#grid)" />
      <g transform={`translate(${panX} ${panY}) scale(${scale})`}>
        {graphEdges.map((edge, index) => {
          const from = graphNodes.find(n => n.id === edge.from);
          const to = graphNodes.find(n => n.id === edge.to);
          if (!from || !to) return null;
          return (
            <line
              key={index}
              x1={from.x}
              y1={from.y}
              x2={to.x}
              y2={to.y}
              className={getEdgeStyle(from.id, to.id)}
              strokeDasharray={isNodeInRoute(from.id) && isNodeInRoute(to.id) ? '0' : '3,3'}
              opacity={mapReady ? 1 : 0}
            />
          );
        })}
        {graphNodes.map((node) => (
          <g key={node.id} opacity={mapReady ? 1 : 0}>
            <circle
              cx={node.x}
              cy={node.y}
              r="10"
              className={`${getNodeStyle(node.id)} transition-all duration-200 cursor-pointer`}
              fill={isSourceNode(node.id) ? '#10B981' : isTargetNode(node.id) ? '#F59E0B' : isNodeInRoute(node.id) ? '#B91C1C' : '#1F2937'}
              stroke={isSourceNode(node.id) ? '#10B981' : isTargetNode(node.id) ? '#F59E0B' : isNodeInRoute(node.id) ? '#B91C1C' : '#374151'}
              strokeWidth="2"
            />
            <text
              x={node.x}
              y={node.y + 2}
              textAnchor="middle"
              className="text-xs font-semibold fill-white pointer-events-none"
            >
              {node.id}
            </text>
          </g>
        ))}
        <path
          d={routeNodes.length > 1 && mapReady ?
            (() => {
              const first = graphNodes.find(n => n.id === routeNodes[0]);
              const parts: string[] = [];
              if (first) parts.push(`M ${first.x} ${first.y}`);
              for (let i = 1; i < routeNodes.length; i++) {
                const n = graphNodes.find(nn => nn.id === routeNodes[i]);
                if (n) parts.push(`L ${n.x} ${n.y}`);
              }
              return parts.join(' ');
            })() : 'M 0 0'}
          fill="none"
          stroke="#B91C1C"
          strokeWidth="3"
          strokeDasharray="5,5"
          className="animate-pulse"
          opacity={routeNodes.length > 1 && mapReady ? 1 : 0}
        />
      </g>
    </svg>
  );

  // Fullscreen overlay
  if (isFullscreen) {
    return (
      <div className="fixed inset-0 z-50 bg-gray-900 flex flex-col">
        <div className="flex-1 w-full max-w-full overflow-hidden">
          {SvgContent}
        </div>
        {/* Bottom controls */}
        <div className="bg-gray-800 border-t border-gray-700 p-3 flex items-center justify-center gap-3">
          <button onClick={() => panBy(-40, 0)} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">←</button>
          <button onClick={() => panBy(40, 0)} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">→</button>
          <button onClick={() => panBy(0, -40)} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">↑</button>
          <button onClick={() => panBy(0, 40)} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">↓</button>
          <div className="w-px h-6 bg-gray-600 mx-2" />
          <button onClick={zoomOut} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">−</button>
          <span className="text-gray-300 text-sm">{(scale * 100).toFixed(0)}%</span>
          <button onClick={zoomIn} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">＋</button>
          <button onClick={resetView} className="bg-gray-700 hover:bg-gray-600 text-white px-3 py-2 rounded">Reset</button>
          <div className="ml-auto" />
          <button onClick={toggleFullscreen} className="bg-red-600 hover:bg-red-700 text-white px-4 py-2 rounded">Salir</button>
        </div>
      </div>
    );
  }

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg w-full max-w-full overflow-hidden relative flex flex-col" style={{ height: effectiveHeight }}>
      {/* Map Header */}
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between mb-3 sm:mb-4 space-y-2 sm:space-y-0">
        <div className="min-w-0">
          <h3 className="text-lg sm:text-xl font-semibold text-white truncate">Visualización de Ruta</h3>
          <p className="text-xs sm:text-sm text-gray-400 truncate">
            {routeNodes.length > 0 
              ? `Mostrando ${routeNodes.length} nodos`
              : 'Visualización interactiva del grafo'
            }
          </p>
        </div>
        
        <div className="flex items-center space-x-2 sm:space-x-4 flex-shrink-0">
          {/* Legend */}
          <div className="flex items-center space-x-2 sm:space-x-4 text-xs">
            <div className="flex items-center space-x-1">
              <div className="w-2 h-2 sm:w-3 sm:h-3 bg-green-500 rounded-full flex-shrink-0" />
              <span className="text-gray-400 hidden sm:inline">Origen</span>
            </div>
            <div className="flex items-center space-x-1">
              <div className="w-2 h-2 sm:w-3 sm:h-3 bg-yellow-500 rounded-full flex-shrink-0" />
              <span className="text-gray-400 hidden sm:inline">Destino</span>
            </div>
            <div className="flex items-center space-x-1">
              <div className="w-2 h-2 sm:w-3 sm:h-3 bg-red-600 rounded-full flex-shrink-0" />
              <span className="text-gray-400 hidden sm:inline">Ruta</span>
            </div>
          </div>
        </div>
      </div>

      {/* Map Container */}
      <div 
        ref={mapRef} 
        className="relative bg-gray-700 rounded-lg border border-gray-600 overflow-hidden w-full max-w-full flex-1"
        style={{ minHeight: '300px' }}
      >
        {/* SVG Visualization */}
        {SvgContent}

        {/* Loading State - Overlay when not ready */}
        {!mapReady && (
          <div className="absolute inset-0 flex items-center justify-center bg-gray-700/80">
            <div className="text-center">
              <div className="w-6 h-6 sm:w-8 sm:h-8 border-2 border-red-600 border-t-transparent rounded-full animate-spin mx-auto mb-2" />
              <p className="text-xs sm:text-sm text-gray-400">Cargando mapa...</p>
            </div>
          </div>
        )}
        
        {/* Map Controls (top-right) */}
        <div className="absolute top-2 sm:top-4 right-2 sm:right-4 flex flex-col space-y-1 sm:space-y-2">
          <button onClick={zoomIn} className="bg-gray-800 border border-gray-600 p-1.5 sm:p-2 rounded hover:bg-gray-700 transition-colors" title="Acercar">
            <svg className="w-3 h-3 sm:w-4 sm:h-4 text-white" fill="currentColor" viewBox="0 0 20 20">
              <path d="M10 3a1 1 0 011 1v5h5a1 1 0 110 2h-5v5a1 1 0 11-2 0v-5H4a1 1 0 110-2h5V4a1 1 0 011-1z" />
            </svg>
          </button>
          <button onClick={zoomOut} className="bg-gray-800 border border-gray-600 p-1.5 sm:p-2 rounded hover:bg-gray-700 transition-colors" title="Alejar">
            <svg className="w-3 h-3 sm:w-4 sm:h-4 text-white" fill="currentColor" viewBox="0 0 20 20">
              <path d="M4 9a1 1 0 100 2h12a1 1 0 100-2H4z" />
            </svg>
          </button>
          <button onClick={toggleFullscreen} className="bg-gray-800 border border-gray-600 p-1.5 sm:p-2 rounded hover:bg-gray-700 transition-colors" title="Pantalla completa">
            <svg className="w-3 h-3 sm:w-4 sm:h-4 text-white" fill="currentColor" viewBox="0 0 20 20">
              <path d="M3 3h6v2H5v4H3V3zm14 0v6h-2V5h-4V3h6zM3 17v-6h2v4h4v2H3zm14-6v6h-6v-2h4v-4h2z" />
            </svg>
          </button>
        </div>
      </div>
      
      {/* Map Footer */}
      {routeNodes.length > 0 && (
        <div className="mt-3 sm:mt-4 p-2 sm:p-3 bg-gray-700 rounded-lg border border-gray-600 overflow-hidden">
          <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between text-xs sm:text-sm space-y-1 sm:space-y-0">
            <span className="text-gray-400 truncate">
              Ruta: {routeNodes.join(' → ')}
            </span>
            <span className="text-red-600 font-medium truncate">
              Costo: {routeCost.toFixed(2)} | Algoritmo: {routeAlgorithm.toUpperCase()}
            </span>
          </div>
        </div>
      )}
    </div>
  );
};

export default MapContainer;
