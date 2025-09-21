/**
 * Unified Control Panel Component
 * Combines all controls in a single compact panel with tabs
 */

import { FC, useState } from 'react';
import { useCalculateRoute, useClearRoute, useGraphState, useRegenerateGraph, useGraphK, useSetGraphK } from '../store';

const UnifiedControlPanel: FC = () => {
  const [activeTab, setActiveTab] = useState<'config' | 'graph' | 'stats'>('config');
  const [sourceNode, setSourceNode] = useState<number>(1);
  const [targetNode, setTargetNode] = useState<number>(50);
  const [algorithm, setAlgorithm] = useState<'dijkstra' | 'astar'>('dijkstra');
  const [metric, setMetric] = useState<'time' | 'distance' | 'cost'>('distance');
  
  const calculateRoute = useCalculateRoute();
  const clearRoute = useClearRoute();
  const { loaded, size } = useGraphState();
  const regenerateGraph = useRegenerateGraph();
  const graphK = useGraphK();
  const setGraphK = useSetGraphK();

  const handleCalculate = async () => {
    if (!sourceNode || !targetNode) return;
    
    await calculateRoute({
      source: sourceNode,
      target: targetNode,
      algo: algorithm,
      metric,
      debug: true,
    });
  };

  const handleClear = () => {
    clearRoute();
    setSourceNode(1);
    setTargetNode(50);
  };

  const handleRegenerateGraph = async () => {
    await regenerateGraph(size);
  };

  const tabs = [
    { 
      id: 'config' as const, 
      label: 'Configuración', 
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M11.49 3.17c-.38-1.56-2.6-1.56-2.98 0a1.532 1.532 0 01-2.286.948c-1.372-.836-2.942.734-2.106 2.106.54.886.061 2.042-.947 2.287-1.561.379-1.561 2.6 0 2.978a1.532 1.532 0 01.947 2.287c-.836 1.372.734 2.942 2.106 2.106a1.532 1.532 0 012.287.947c.379 1.561 2.6 1.561 2.978 0a1.533 1.533 0 012.287-.947c1.372.836 2.942-.734 2.106-2.106a1.533 1.533 0 01.947-2.287c1.561-.379 1.561-2.6 0-2.978a1.532 1.532 0 01-.947-2.287c.836-1.372-.734-2.942-2.106-2.106a1.532 1.532 0 01-2.287-.947zM10 13a3 3 0 100-6 3 3 0 000 6z" clipRule="evenodd" />
        </svg>
      )
    },
    { 
      id: 'graph' as const, 
      label: 'Grafo', 
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path d="M2 11a1 1 0 011-1h2a1 1 0 011 1v5a1 1 0 01-1 1H3a1 1 0 01-1-1v-5zM8 7a1 1 0 011-1h2a1 1 0 011 1v9a1 1 0 01-1 1H9a1 1 0 01-1-1V7zM14 4a1 1 0 011-1h2a1 1 0 011 1v12a1 1 0 01-1 1h-2a1 1 0 01-1-1V4z" />
        </svg>
      )
    },
    { 
      id: 'stats' as const, 
      label: 'Estadísticas', 
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path d="M2.003 5.884L10 9.882l7.997-3.998A2 2 0 0016 4H4a2 2 0 00-1.997 1.884z" />
          <path d="M18 8.118l-8 4-8-4V14a2 2 0 002 2h12a2 2 0 002-2V8.118z" />
        </svg>
      )
    },
  ];

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl h-fit">
      {/* Tab Navigation */}
      <div className="flex border-b border-gray-600">
        {tabs.map((tab) => (
          <button
            key={tab.id}
            onClick={() => setActiveTab(tab.id)}
            className={`
              flex-1 px-4 py-3 text-sm font-medium transition-all duration-200
              ${activeTab === tab.id
                ? 'bg-blue-600 text-white border-b-2 border-blue-400'
                : 'text-gray-400 hover:text-white hover:bg-gray-700/50'
              }
            `}
          >
            <span className="mr-2">{tab.icon}</span>
            {tab.label}
          </button>
        ))}
      </div>

      {/* Tab Content */}
      <div className="p-6">
        {activeTab === 'config' && (
          <div className="space-y-6">
            {/* Graph Configuration */}
            <div>
              <h3 className="text-lg font-semibold text-white mb-4">Configuración de Grafo</h3>
              <div className="space-y-4">
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Número de Nodos: {size}
                  </label>
                  <div className="flex space-x-2">
                    <button
                      onClick={handleRegenerateGraph}
                      className="flex-1 bg-blue-600 hover:bg-blue-700 text-white font-semibold px-3 py-2 rounded-lg transition-colors flex items-center justify-center"
                    >
                      <svg className="w-4 h-4 mr-2" fill="currentColor" viewBox="0 0 20 20">
                        <path fillRule="evenodd" d="M4 2a1 1 0 011 1v2.101a7.002 7.002 0 0111.601 2.566 1 1 0 11-1.885.666A5.002 5.002 0 005.999 7H9a1 1 0 010 2H4a1 1 0 01-1-1V3a1 1 0 011-1zm.008 9.057a1 1 0 011.276.61A5.002 5.002 0 0014.001 13H11a1 1 0 110-2h5a1 1 0 011 1v5a1 1 0 11-2 0v-2.101a7.002 7.002 0 01-11.601-2.566 1 1 0 01.61-1.276z" clipRule="evenodd" />
                      </svg>
                      Regenerar
                    </button>
                  </div>
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Conectividad (K): {graphK}
                  </label>
                  <input
                    type="range"
                    min="2"
                    max="10"
                    value={graphK}
                    onChange={(e) => setGraphK(parseInt(e.target.value))}
                    className="w-full h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
                  />
                  <div className="flex justify-between text-xs text-gray-400 mt-1">
                    <span>2 (mínimo)</span>
                    <span>10 (máximo)</span>
                  </div>
                </div>
              </div>
            </div>

            {/* Node Configuration */}
            <div>
              <h3 className="text-lg font-semibold text-white mb-4">Configuración de Nodos</h3>
              <div className="space-y-4">
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Nodo Origen
                  </label>
                  <div className="flex space-x-2">
                    <input
                      type="number"
                      value={sourceNode}
                      onChange={(e) => setSourceNode(parseInt(e.target.value) || 1)}
                      className="flex-1 px-3 py-2 bg-gray-700 border border-gray-600 rounded-lg text-white focus:border-blue-500 focus:outline-none"
                      min="1"
                      max={size}
                    />
                    <button
                      onClick={() => setSourceNode(Math.floor(Math.random() * size) + 1)}
                      className="bg-gray-600 hover:bg-gray-700 text-white px-3 py-2 rounded-lg transition-colors"
                      title="Nodo aleatorio"
                    >
                      <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
                        <path fillRule="evenodd" d="M4 2a1 1 0 011 1v2.101a7.002 7.002 0 0111.601 2.566 1 1 0 11-1.885.666A5.002 5.002 0 005.999 7H9a1 1 0 010 2H4a1 1 0 01-1-1V3a1 1 0 011-1zm.008 9.057a1 1 0 011.276.61A5.002 5.002 0 0014.001 13H11a1 1 0 110-2h5a1 1 0 011 1v5a1 1 0 11-2 0v-2.101a7.002 7.002 0 01-11.601-2.566 1 1 0 01.61-1.276z" clipRule="evenodd" />
                      </svg>
                    </button>
                  </div>
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Nodo Destino
                  </label>
                  <div className="flex space-x-2">
                    <input
                      type="number"
                      value={targetNode}
                      onChange={(e) => setTargetNode(parseInt(e.target.value) || 50)}
                      className="flex-1 px-3 py-2 bg-gray-700 border border-gray-600 rounded-lg text-white focus:border-blue-500 focus:outline-none"
                      min="1"
                      max={size}
                    />
                    <button
                      onClick={() => setTargetNode(Math.floor(Math.random() * size) + 1)}
                      className="bg-gray-600 hover:bg-gray-700 text-white px-3 py-2 rounded-lg transition-colors"
                      title="Nodo aleatorio"
                    >
                      <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
                        <path fillRule="evenodd" d="M4 2a1 1 0 011 1v2.101a7.002 7.002 0 0111.601 2.566 1 1 0 11-1.885.666A5.002 5.002 0 005.999 7H9a1 1 0 010 2H4a1 1 0 01-1-1V3a1 1 0 011-1zm.008 9.057a1 1 0 011.276.61A5.002 5.002 0 0014.001 13H11a1 1 0 110-2h5a1 1 0 011 1v5a1 1 0 11-2 0v-2.101a7.002 7.002 0 01-11.601-2.566 1 1 0 01.61-1.276z" clipRule="evenodd" />
                      </svg>
                    </button>
                  </div>
                </div>
              </div>
            </div>

            {/* Algorithm Configuration */}
            <div>
              <h3 className="text-lg font-semibold text-white mb-4">Algoritmo</h3>
              <div className="space-y-4">
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Tipo de Algoritmo
                  </label>
                  <select
                    value={algorithm}
                    onChange={(e) => setAlgorithm(e.target.value as 'dijkstra' | 'astar')}
                    className="w-full px-3 py-2 bg-gray-700 border border-gray-600 rounded-lg text-white focus:border-blue-500 focus:outline-none"
                  >
                    <option value="dijkstra">Dijkstra</option>
                    <option value="astar">A*</option>
                  </select>
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-300 mb-2">
                    Métrica
                  </label>
                  <select
                    value={metric}
                    onChange={(e) => setMetric(e.target.value as 'time' | 'distance' | 'cost')}
                    className="w-full px-3 py-2 bg-gray-700 border border-gray-600 rounded-lg text-white focus:border-blue-500 focus:outline-none"
                  >
                    <option value="time">Tiempo</option>
                    <option value="distance">Distancia</option>
                    <option value="cost">Costo</option>
                  </select>
                </div>
              </div>
            </div>

            {/* Action Buttons */}
            <div className="flex space-x-3">
              <button
                onClick={handleCalculate}
                disabled={!loaded}
                className="flex-1 bg-blue-600 hover:bg-blue-700 text-white font-semibold px-4 py-3 rounded-lg transition-colors disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center"
              >
                <svg className="w-4 h-4 mr-2" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zM9.555 7.168A1 1 0 008 8v4a1 1 0 001.555.832l3-2a1 1 0 000-1.664l-3-2z" clipRule="evenodd" />
                </svg>
                Calcular
              </button>
              <button
                onClick={handleClear}
                className="bg-gray-600 hover:bg-gray-700 text-white font-semibold px-4 py-3 rounded-lg transition-colors flex items-center justify-center"
              >
                <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z" clipRule="evenodd" />
                </svg>
              </button>
            </div>
          </div>
        )}

        {activeTab === 'graph' && (
          <div className="space-y-6">
            <h3 className="text-lg font-semibold text-white mb-4">Información del Grafo</h3>
            <div className="space-y-4">
              <div className="bg-gray-700/50 rounded-lg p-4">
                <div className="text-sm text-gray-400">Nodos disponibles</div>
                <div className="text-2xl font-bold text-white">{size}</div>
                <div className="text-xs text-gray-500">Rango: 1 - {size}</div>
              </div>
              <div className="bg-gray-700/50 rounded-lg p-4">
                <div className="text-sm text-gray-400">Estado</div>
                <div className="flex items-center mt-1">
                  <div className={`w-2 h-2 rounded-full mr-2 ${loaded ? 'bg-green-500' : 'bg-red-500'}`} />
                  <span className="text-sm text-white">{loaded ? 'Cargado' : 'No cargado'}</span>
                </div>
              </div>
            </div>
          </div>
        )}

        {activeTab === 'stats' && (
          <div className="space-y-6">
            <h3 className="text-lg font-semibold text-white mb-4">Estadísticas Rápidas</h3>
            <div className="space-y-3">
              <div className="bg-gray-700/50 rounded-lg p-3">
                <div className="text-xs text-gray-400">Algoritmo actual</div>
                <div className="text-sm font-medium text-white capitalize">{algorithm}</div>
              </div>
              <div className="bg-gray-700/50 rounded-lg p-3">
                <div className="text-xs text-gray-400">Métrica</div>
                <div className="text-sm font-medium text-white capitalize">{metric}</div>
              </div>
              <div className="bg-gray-700/50 rounded-lg p-3">
                <div className="text-xs text-gray-400">Configuración</div>
                <div className="text-sm font-medium text-white">{sourceNode} → {targetNode}</div>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default UnifiedControlPanel;
