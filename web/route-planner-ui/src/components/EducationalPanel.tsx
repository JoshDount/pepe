/**
 * Educational Panel Component
 * Interactive educational content for Operations Research class
 */

import { useState, FC } from 'react';

interface EducationalPanelProps {
  algorithm: 'dijkstra' | 'astar' | 'bfs' | 'dfs';
  isCalculating: boolean;
  routeFound: boolean;
  routeLength: number;
  totalDistance: number;
  executionTime: number;
  nodesExplored: number;
}

const EducationalPanel: FC<EducationalPanelProps> = ({
  algorithm,
  isCalculating,
  routeFound,
  routeLength,
  totalDistance,
  executionTime,
  nodesExplored
}) => {
  const [activeTab, setActiveTab] = useState<'theory' | 'demo' | 'metrics'>('theory');

  const algorithmInfo = {
    dijkstra: {
      name: "Algoritmo de Dijkstra",
      complexity: "O((V + E) log V)",
      description: "Encuentra el camino más corto desde un nodo origen a todos los demás nodos en un grafo con pesos no negativos.",
      steps: [
        "1. Inicializar distancias: origen = 0, todos los demás = ∞",
        "2. Crear cola de prioridad con todos los nodos",
        "3. Mientras la cola no esté vacía:",
        "   a) Extraer nodo con menor distancia",
        "   b) Para cada vecino no visitado:",
        "      - Calcular nueva distancia = distancia_actual + peso_arista",
        "      - Si nueva distancia < distancia_almacenada: actualizar",
        "4. Reconstruir camino desde destino hasta origen"
      ],
      advantages: [
        "✓ Garantiza el camino óptimo",
        "✓ Funciona con pesos no negativos",
        "✓ Eficiente con cola de prioridad"
      ],
      disadvantages: [
        "✗ No funciona con pesos negativos",
        "✗ Complejidad O(V²) sin cola de prioridad"
      ]
    },
    astar: {
      name: "Algoritmo A*",
      complexity: "O(b^d)",
      description: "Algoritmo de búsqueda informada que usa una función heurística para guiar la búsqueda hacia el objetivo.",
      steps: [
        "1. Calcular f(n) = g(n) + h(n) para cada nodo",
        "2. g(n) = costo real desde origen",
        "3. h(n) = heurística (distancia euclidiana al destino)",
        "4. Explorar nodos con menor f(n) primero",
        "5. Detener cuando se alcanza el destino"
      ],
      advantages: [
        "✓ Más eficiente que Dijkstra",
        "✓ Usa información heurística",
        "✓ Encuentra camino óptimo si h(n) es admisible"
      ],
      disadvantages: [
        "✗ Requiere función heurística buena",
        "✗ Complejidad depende de la heurística"
      ]
    }
  };

  const currentAlgo = algorithmInfo[algorithm as keyof typeof algorithmInfo] || algorithmInfo.dijkstra;

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
      <div className="flex items-center justify-between mb-6">
        <h3 className="text-xl font-bold text-white">📚 Panel Educativo</h3>
        <div className="text-sm text-gray-400">
          Investigación de Operaciones
        </div>
      </div>

      {/* Tabs */}
      <div className="flex space-x-1 mb-6 bg-gray-700 rounded-lg p-1">
        <button
          onClick={() => setActiveTab('theory')}
          className={`flex-1 py-2 px-4 rounded-md text-sm font-medium transition-colors ${
            activeTab === 'theory'
              ? 'bg-blue-600 text-white'
              : 'text-gray-300 hover:text-white'
          }`}
        >
          📖 Teoría
        </button>
        <button
          onClick={() => setActiveTab('demo')}
          className={`flex-1 py-2 px-4 rounded-md text-sm font-medium transition-colors ${
            activeTab === 'demo'
              ? 'bg-blue-600 text-white'
              : 'text-gray-300 hover:text-white'
          }`}
        >
          🎯 Demostración
        </button>
        <button
          onClick={() => setActiveTab('metrics')}
          className={`flex-1 py-2 px-4 rounded-md text-sm font-medium transition-colors ${
            activeTab === 'metrics'
              ? 'bg-blue-600 text-white'
              : 'text-gray-300 hover:text-white'
          }`}
        >
          📊 Métricas
        </button>
      </div>

      {/* Content */}
      {activeTab === 'theory' && (
        <div className="space-y-6">
          <div className="bg-blue-900/30 border border-blue-600 rounded-lg p-4">
            <h4 className="text-lg font-semibold text-blue-300 mb-2">
              {currentAlgo.name}
            </h4>
            <p className="text-blue-200 text-sm mb-3">
              {currentAlgo.description}
            </p>
            <div className="text-blue-400 text-sm font-mono">
              Complejidad: {currentAlgo.complexity}
            </div>
          </div>

          <div className="bg-gray-700 rounded-lg p-4">
            <h5 className="text-white font-semibold mb-3">Pasos del Algoritmo:</h5>
            <ol className="space-y-2 text-sm text-gray-300">
              {currentAlgo.steps.map((step: string, index: number) => (
                <li key={index} className="flex items-start">
                  <span className="text-blue-400 mr-2 font-mono text-xs">
                    {step.split(' ')[0]}
                  </span>
                  <span>{step.substring(step.indexOf(' ') + 1)}</span>
                </li>
              ))}
            </ol>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="bg-green-900/30 border border-green-600 rounded-lg p-3">
              <h6 className="text-green-300 font-semibold mb-2">Ventajas:</h6>
              <ul className="space-y-1 text-sm text-green-200">
                {currentAlgo.advantages.map((adv: string, index: number) => (
                  <li key={index}>{adv}</li>
                ))}
              </ul>
            </div>
            <div className="bg-red-900/30 border border-red-600 rounded-lg p-3">
              <h6 className="text-red-300 font-semibold mb-2">Desventajas:</h6>
              <ul className="space-y-1 text-sm text-red-200">
                {currentAlgo.disadvantages.map((dis: string, index: number) => (
                  <li key={index}>{dis}</li>
                ))}
              </ul>
            </div>
          </div>
        </div>
      )}

      {activeTab === 'demo' && (
        <div className="space-y-6">
          <div className="bg-purple-900/30 border border-purple-600 rounded-lg p-4">
            <h4 className="text-lg font-semibold text-purple-300 mb-3">
              🎯 Demostración en Tiempo Real
            </h4>
            
            <div className="space-y-3">
              <div className="flex items-center justify-between">
                <span className="text-purple-200">Estado del Algoritmo:</span>
                <span className={`px-3 py-1 rounded-full text-sm font-medium ${
                  isCalculating 
                    ? 'bg-yellow-600 text-yellow-100' 
                    : routeFound 
                      ? 'bg-green-600 text-green-100'
                      : 'bg-gray-600 text-gray-100'
                }`}>
                  {isCalculating ? '🔄 Calculando...' : routeFound ? '✅ Completado' : '⏸️ En espera'}
                </span>
              </div>

              {routeFound && (
                <>
                  <div className="flex items-center justify-between">
                    <span className="text-purple-200">Nodos en la ruta:</span>
                    <span className="text-purple-100 font-mono">{routeLength}</span>
                  </div>
                  <div className="flex items-center justify-between">
                    <span className="text-purple-200">Distancia total:</span>
                    <span className="text-purple-100 font-mono">{Math.round(totalDistance)}m</span>
                  </div>
                  <div className="flex items-center justify-between">
                    <span className="text-purple-200">Nodos explorados:</span>
                    <span className="text-purple-100 font-mono">{nodesExplored}</span>
                  </div>
                </>
              )}
            </div>
          </div>

          <div className="bg-gray-700 rounded-lg p-4">
            <h5 className="text-white font-semibold mb-3">💡 Conceptos Clave:</h5>
            <div className="space-y-2 text-sm text-gray-300">
              <div className="flex items-start">
                <span className="text-blue-400 mr-2">🎯</span>
                <span><strong>Grafo:</strong> Red de nodos (intersecciones) conectados por aristas (calles)</span>
              </div>
              <div className="flex items-start">
                <span className="text-green-400 mr-2">⚖️</span>
                <span><strong>Pesos:</strong> Distancia real entre nodos (en metros)</span>
              </div>
              <div className="flex items-start">
                <span className="text-yellow-400 mr-2">🔍</span>
                <span><strong>Exploración:</strong> Nodos visitados durante la búsqueda</span>
              </div>
              <div className="flex items-start">
                <span className="text-red-400 mr-2">📏</span>
                <span><strong>Optimalidad:</strong> Garantiza el camino más corto posible</span>
              </div>
            </div>
          </div>
        </div>
      )}

      {activeTab === 'metrics' && (
        <div className="space-y-6">
          <div className="bg-indigo-900/30 border border-indigo-600 rounded-lg p-4">
            <h4 className="text-lg font-semibold text-indigo-300 mb-3">
              📊 Análisis de Rendimiento
            </h4>
            
            <div className="grid grid-cols-2 gap-4">
              <div className="bg-gray-800 rounded-lg p-3">
                <div className="text-indigo-200 text-sm mb-1">Tiempo de Ejecución</div>
                <div className="text-2xl font-bold text-indigo-100">
                  {executionTime.toFixed(2)}ms
                </div>
              </div>
              <div className="bg-gray-800 rounded-lg p-3">
                <div className="text-indigo-200 text-sm mb-1">Eficiencia</div>
                <div className="text-2xl font-bold text-indigo-100">
                  {nodesExplored > 0 ? ((routeLength / nodesExplored) * 100).toFixed(1) : 0}%
                </div>
              </div>
            </div>
          </div>

          <div className="bg-gray-700 rounded-lg p-4">
            <h5 className="text-white font-semibold mb-3">📈 Métricas de Complejidad:</h5>
            <div className="space-y-3 text-sm">
              <div className="flex justify-between items-center">
                <span className="text-gray-300">Complejidad Temporal:</span>
                <span className="text-blue-400 font-mono">{currentAlgo.complexity}</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-gray-300">Complejidad Espacial:</span>
                <span className="text-green-400 font-mono">O(V)</span>
              </div>
              <div className="flex justify-between items-center">
                <span className="text-gray-300">Tipo de Búsqueda:</span>
                <span className="text-yellow-400">Greedy + BFS</span>
              </div>
            </div>
          </div>

          <div className="bg-gray-700 rounded-lg p-4">
            <h5 className="text-white font-semibold mb-3">🎓 Aplicaciones en I.O.:</h5>
            <div className="space-y-2 text-sm text-gray-300">
              <div>• <strong>Logística:</strong> Optimización de rutas de entrega</div>
              <div>• <strong>Transporte:</strong> Planificación de redes de transporte</div>
              <div>• <strong>Telecomunicaciones:</strong> Enrutamiento de paquetes</div>
              <div>• <strong>Robótica:</strong> Navegación autónoma</div>
              <div>• <strong>Juegos:</strong> IA para pathfinding</div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default EducationalPanel;
