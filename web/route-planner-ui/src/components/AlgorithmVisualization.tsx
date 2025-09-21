/**
 * Algorithm Visualization Component
 * Step-by-step visualization of the pathfinding algorithm
 */

import { useState, useEffect, FC } from 'react';
import VisualAlgorithmMap from './VisualAlgorithmMap';

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

interface CityNode {
  id: number;
  lat: number;
  lon: number;
}

interface AlgorithmVisualizationProps {
  isCalculating: boolean;
  routeFound: boolean;
  routeNodes: number[];
  algorithmSteps: AlgorithmStep[];
  cityNodes: CityNode[];
  calculatedRoute: CityNode[];
  onStepChange?: (step: AlgorithmStep) => void;
}

const AlgorithmVisualization: FC<AlgorithmVisualizationProps> = ({
  isCalculating,
  routeFound,
  routeNodes,
  algorithmSteps,
  cityNodes,
  calculatedRoute,
  onStepChange
}) => {
  const [currentStep, setCurrentStep] = useState(0);
  const [isPlaying, setIsPlaying] = useState(false);
  const [playbackSpeed, setPlaybackSpeed] = useState(1000); // ms

  // Reset current step when new steps are available
  useEffect(() => {
    if (algorithmSteps.length > 0) {
      setCurrentStep(0);
    }
  }, [algorithmSteps]);

  // Auto-play steps
  useEffect(() => {
    if (isPlaying && currentStep < algorithmSteps.length - 1) {
      const timer = setTimeout(() => {
        setCurrentStep(prev => prev + 1);
      }, playbackSpeed);
      return () => clearTimeout(timer);
    } else if (currentStep >= algorithmSteps.length - 1) {
      setIsPlaying(false);
    }
  }, [isPlaying, currentStep, algorithmSteps.length, playbackSpeed]);

  const handleStepChange = (stepIndex: number) => {
    setCurrentStep(stepIndex);
    if (algorithmSteps[stepIndex]) {
      onStepChange?.(algorithmSteps[stepIndex]);
    }
  };

  const getActionIcon = (action: string) => {
    switch (action) {
      case 'initialize': return '🚀';
      case 'explore': return '🔍';
      case 'select_min': return '⚡';
      case 'found_target': return '🎯';
      default: return '📝';
    }
  };

  const getActionColor = (action: string) => {
    switch (action) {
      case 'initialize': return 'bg-blue-600';
      case 'explore': return 'bg-green-600';
      case 'select_min': return 'bg-yellow-600';
      case 'found_target': return 'bg-red-600';
      default: return 'bg-gray-600';
    }
  };

  if (!isCalculating && algorithmSteps.length === 0) {
    return (
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
        <h3 className="text-xl font-bold text-white mb-4">🎬 Visualización del Algoritmo</h3>
        <div className="text-center py-8">
          <div className="text-gray-400 mb-4">
            Selecciona nodos y calcula una ruta para ver la visualización paso a paso
          </div>
          <div className="text-sm text-gray-500">
            La visualización mostrará cómo el algoritmo explora el grafo
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="space-y-6">
      {/* Visual Algorithm Map */}
      {algorithmSteps.length > 0 && cityNodes.length > 0 && (
        <VisualAlgorithmMap
          nodes={cityNodes}
          algorithmSteps={algorithmSteps}
          currentStep={currentStep}
          route={calculatedRoute}
          onStepChange={setCurrentStep}
        />
      )}

      {/* Algorithm Details Panel */}
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
        <div className="flex items-center justify-between mb-6">
          <h3 className="text-xl font-bold text-white">📊 Detalles del Algoritmo</h3>
          <div className="flex items-center gap-2">
            <span className="text-sm text-gray-400">
              Paso {currentStep + 1} de {algorithmSteps.length}
            </span>
          </div>
        </div>

      {/* Current Step Display */}
      {algorithmSteps[currentStep] && (
        <div className="mb-6">
          <div className={`${getActionColor(algorithmSteps[currentStep].action)} rounded-lg p-4 mb-4`}>
            <div className="flex items-center gap-3 mb-2">
              <span className="text-2xl">{getActionIcon(algorithmSteps[currentStep].action)}</span>
              <div>
                <h4 className="text-white font-semibold">
                  Paso {algorithmSteps[currentStep].step}: {algorithmSteps[currentStep].action.toUpperCase()}
                </h4>
                <p className="text-gray-200 text-sm">
                  {algorithmSteps[currentStep].description}
                </p>
              </div>
            </div>
          </div>

          {/* Step Details */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-4">
            <div className="bg-gray-700 rounded-lg p-3">
              <div className="text-gray-300 text-sm mb-1">Nodo Actual</div>
              <div className="text-white font-mono text-lg">
                {algorithmSteps[currentStep].currentNode}
              </div>
            </div>
            <div className="bg-gray-700 rounded-lg p-3">
              <div className="text-gray-300 text-sm mb-1">Nodos Explorados</div>
              <div className="text-white font-mono text-lg">
                {algorithmSteps[currentStep].exploredNodes.length}
              </div>
            </div>
            <div className="bg-gray-700 rounded-lg p-3">
              <div className="text-gray-300 text-sm mb-1">Frontera</div>
              <div className="text-white font-mono text-lg">
                {algorithmSteps[currentStep].frontier.length}
              </div>
            </div>
          </div>

          {/* Distances */}
          <div className="bg-gray-700 rounded-lg p-3">
            <div className="text-gray-300 text-sm mb-2">
              Distancias Conocidas: {Object.keys(algorithmSteps[currentStep].distances).length} nodos
            </div>
            <div className="flex flex-wrap gap-2 max-h-32 overflow-y-auto">
              {Object.entries(algorithmSteps[currentStep].distances)
                .sort(([,a], [,b]) => (a as number) - (b as number))
                .slice(0, 20)
                .map(([node, distance]) => (
                <div key={node} className="bg-gray-600 rounded px-2 py-1 text-xs">
                  <span className="text-blue-400">{node}:</span>
                  <span className="text-white ml-1">{Math.round(distance as number)}m</span>
                </div>
              ))}
              {Object.keys(algorithmSteps[currentStep].distances).length > 20 && (
                <div className="text-gray-400 text-xs px-2 py-1">
                  ... y {Object.keys(algorithmSteps[currentStep].distances).length - 20} más
                </div>
              )}
            </div>
          </div>
        </div>
      )}

      {/* Controls */}
      <div className="flex items-center justify-between mb-4">
        <div className="flex items-center gap-2">
          <button
            onClick={() => setIsPlaying(!isPlaying)}
            disabled={currentStep >= algorithmSteps.length - 1}
            className="px-4 py-2 bg-blue-600 hover:bg-blue-700 disabled:bg-gray-600 disabled:cursor-not-allowed text-white rounded-lg text-sm font-medium transition-colors"
          >
            {isPlaying ? '⏸️ Pausar' : '▶️ Reproducir'}
          </button>
          <button
            onClick={() => {
              setCurrentStep(0);
              setIsPlaying(false);
            }}
            className="px-4 py-2 bg-gray-600 hover:bg-gray-700 text-white rounded-lg text-sm font-medium transition-colors"
          >
            🔄 Reiniciar
          </button>
        </div>

        <div className="flex items-center gap-2">
          <label className="text-gray-300 text-sm">Velocidad:</label>
          <select
            value={playbackSpeed}
            onChange={(e) => setPlaybackSpeed(Number(e.target.value))}
            className="bg-gray-700 text-white rounded px-2 py-1 text-sm"
          >
            <option value={500}>Rápido</option>
            <option value={1000}>Normal</option>
            <option value={2000}>Lento</option>
          </select>
        </div>
      </div>

      {/* Step Navigation */}
      <div className="space-y-2">
        <div className="text-gray-300 text-sm mb-2">Navegación por Pasos:</div>
        <div className="flex flex-wrap gap-1">
          {algorithmSteps.map((step, index) => (
            <button
              key={index}
              onClick={() => handleStepChange(index)}
              className={`px-3 py-1 rounded text-xs font-medium transition-colors ${
                index === currentStep
                  ? 'bg-blue-600 text-white'
                  : index < currentStep
                    ? 'bg-green-600 text-white'
                    : 'bg-gray-600 text-gray-300 hover:bg-gray-500'
              }`}
            >
              {index + 1}
            </button>
          ))}
        </div>
      </div>

      {/* Progress Bar */}
      <div className="mt-4">
        <div className="bg-gray-700 rounded-full h-2">
          <div
            className="bg-blue-600 h-2 rounded-full transition-all duration-300"
            style={{ width: `${algorithmSteps.length > 0 ? ((currentStep + 1) / algorithmSteps.length) * 100 : 0}%` }}
          />
        </div>
      </div>
      </div>
    </div>
  );
};

export default AlgorithmVisualization;
