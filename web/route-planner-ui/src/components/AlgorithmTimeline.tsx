/**
 * Algorithm Timeline Component
 * Real-time visualization of algorithm execution steps with SSE streaming
 */

import { useEffect, useState, useCallback, FC } from 'react';
import { useTimelineState, useRouteAlgorithm, useRouteExpanded, useClearTimeline } from '../store';
import { styleTokens } from '../styles/tokens';
import type { ExpansionStep } from '../types/api';

interface AlgorithmTimelineProps {
  maxVisibleSteps?: number;
}

const AlgorithmTimeline: FC<AlgorithmTimelineProps> = ({ 
  maxVisibleSteps = 50 
}) => {
  const { steps, streaming, stepCount, currentStep } = useTimelineState();
  const routeAlgorithm = useRouteAlgorithm();
  const routeExpanded = useRouteExpanded();
  const clearTimeline = useClearTimeline();
  
  const [autoScroll, setAutoScroll] = useState(true);
  const [selectedStep, setSelectedStep] = useState<number | null>(null);
  const [displaySteps, setDisplaySteps] = useState<ExpansionStep[]>([]);

  // Update display steps with rate limiting
  useEffect(() => {
    const timeoutId = setTimeout(() => {
      const visibleSteps = steps.slice(-maxVisibleSteps);
      setDisplaySteps(visibleSteps);
      
      // Auto-scroll to latest step if enabled
      if (autoScroll && visibleSteps.length > 0) {
        setSelectedStep(visibleSteps[visibleSteps.length - 1].stepId);
      }
    }, 100); // Debounce updates

    return () => clearTimeout(timeoutId);
  }, [steps, maxVisibleSteps, autoScroll]);

  const handleClearTimeline = useCallback(() => {
    clearTimeline();
    setSelectedStep(null);
  }, [clearTimeline]);

  const handleStepClick = useCallback((stepId: number) => {
    setSelectedStep(stepId);
    setAutoScroll(false);
  }, []);

  const handleToggleAutoScroll = useCallback(() => {
    setAutoScroll(prev => !prev);
  }, []);

  const formatNodeList = (nodes: number[]): string => {
    if (nodes.length === 0) return 'Empty';
    if (nodes.length <= 5) return nodes.join(', ');
    return `${nodes.slice(0, 3).join(', ')}, ... +${nodes.length - 3} more`;
  };

  const getStepRowClass = (step: ExpansionStep): string => {
    const baseClass = 'grid grid-cols-6 gap-2 p-3 rounded-lg text-sm transition-all duration-200 cursor-pointer hover:bg-bg-elevated';
    
    if (selectedStep === step.stepId) {
      return `${baseClass} bg-accent-primary/20 border border-accent-primary/30`;
    }
    
    return `${baseClass} bg-bg-surface border border-border-muted hover:border-accent-primary/50`;
  };

  if (steps.length === 0 && !streaming) {
    return (
      <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
        <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between mb-4 space-y-2 sm:space-y-0">
          <h3 className="text-lg sm:text-xl lg:text-2xl font-semibold text-white">Línea de Tiempo del Algoritmo</h3>
          <span className="text-xs sm:text-sm text-gray-400">Sin datos de línea de tiempo</span>
        </div>
        
        <div className="text-center py-8 sm:py-12">
          <div className="w-12 h-12 sm:w-16 sm:h-16 bg-gray-700 rounded-full flex items-center justify-center mx-auto mb-3 sm:mb-4">
            <svg className="w-6 h-6 sm:w-8 sm:h-8 text-gray-400" fill="currentColor" viewBox="0 0 20 20">
              <path d="M10 18a8 8 0 100-16 8 8 0 000 16zm1-13a1 1 0 10-2 0v4a1 1 0 00.293.707l2.828 2.829a1 1 0 101.415-1.415L11 9.586V5z" />
            </svg>
          </div>
          <h4 className="text-lg sm:text-xl font-semibold text-white mb-2">Sin Pasos de Algoritmo</h4>
          <p className="text-xs sm:text-sm text-gray-400">
            Habilita el modo debug y calcula una ruta para ver los pasos de ejecución del algoritmo
          </p>
        </div>
      </div>
    );
  }

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between mb-4 sm:mb-6 space-y-3 sm:space-y-0">
        <div>
          <h3 className="text-lg sm:text-xl lg:text-2xl font-semibold text-white">Línea de Tiempo del Algoritmo</h3>
          <p className="text-xs sm:text-sm text-gray-400">
            {streaming ? 'Transmisión en vivo...' : 
             `Mostrando ${displaySteps.length} de ${stepCount} pasos totales`}
          </p>
        </div>
        
        <div className="flex items-center space-x-2 sm:space-x-3">
          {/* Streaming Indicator */}
          {streaming && (
            <div className="flex items-center space-x-1 sm:space-x-2 text-red-600">
              <div className="w-1.5 h-1.5 sm:w-2 sm:h-2 bg-red-600 rounded-full animate-pulse" />
              <span className="text-xs sm:text-sm font-medium">En Vivo</span>
            </div>
          )}
          
          {/* Auto-scroll Toggle */}
          <button
            onClick={handleToggleAutoScroll}
            className={`text-xs sm:text-sm px-2 sm:px-3 py-1 rounded-md transition-colors ${
              autoScroll 
                ? 'bg-red-600 text-white' 
                : 'bg-gray-700 text-gray-300 hover:text-white'
            }`}
          >
            Auto-scroll
          </button>
          
          {/* Clear Button */}
          <button
            onClick={handleClearTimeline}
            disabled={streaming}
            className="text-xs sm:text-sm px-2 sm:px-3 py-1 bg-gray-700 hover:bg-gray-600 disabled:bg-gray-800 disabled:text-gray-500 text-white rounded-md transition-colors"
          >
            Limpiar
          </button>
        </div>
      </div>

      {/* Stats Bar */}
      <div className="grid grid-cols-3 gap-3 sm:gap-4 mb-4 sm:mb-6">
        <div className="text-center">
          <div className="text-lg sm:text-xl font-bold text-red-600">{stepCount}</div>
          <div className="text-xs text-gray-400">Total Pasos</div>
        </div>
        
        <div className="text-center">
          <div className="text-lg sm:text-xl font-bold text-green-500">{routeExpanded}</div>
          <div className="text-xs text-gray-400">Nodos Expandidos</div>
        </div>
        
        <div className="text-center">
          <div className="text-lg sm:text-xl font-bold text-yellow-500">
            {steps.length > 0 ? steps[steps.length - 1]?.currentNode || '—' : '—'}
          </div>
          <div className="text-xs text-gray-400">Nodo Actual</div>
        </div>
      </div>

      {/* Timeline Header */}
      <div className="grid grid-cols-6 gap-1 sm:gap-2 p-2 sm:p-3 bg-gray-700 rounded-lg mb-2 text-xs font-semibold text-gray-400">
        <div>Paso</div>
        <div>Actual</div>
        <div>G-Score</div>
        <div>F-Score</div>
        <div>Abierto</div>
        <div>Cerrado</div>
      </div>

      {/* Timeline Steps */}
      <div className="space-y-1 max-h-64 sm:max-h-80 lg:max-h-96 overflow-y-auto custom-scrollbar">
        {displaySteps.map((step, index) => (
          <div
            key={step.stepId}
            className={`grid grid-cols-6 gap-1 sm:gap-2 p-2 sm:p-3 rounded-lg text-xs sm:text-sm transition-all duration-200 cursor-pointer hover:bg-gray-700 ${
              selectedStep === step.stepId
                ? 'bg-red-600/20 border border-red-600/30'
                : 'bg-gray-700/50 border border-gray-600 hover:border-red-600/50'
            }`}
            onClick={() => handleStepClick(step.stepId)}
          >
            <div className="font-mono text-red-600">
              #{step.stepId}
            </div>
            
            <div className="font-bold text-white">
              {step.currentNode}
            </div>
            
            <div className="font-mono text-green-500">
              {step.gScore.toFixed(2)}
            </div>
            
            <div className="font-mono text-yellow-500">
              {step.fScore ? step.fScore.toFixed(2) : '—'}
            </div>
            
            <div className="text-gray-300 text-xs truncate" title={formatNodeList(step.openSet)}>
              [{formatNodeList(step.openSet)}]
            </div>
            
            <div className="text-gray-400 text-xs truncate" title={formatNodeList(step.closedSet)}>
              [{formatNodeList(step.closedSet)}]
            </div>
          </div>
        ))}
        
        {streaming && (
          <div className="grid grid-cols-6 gap-1 sm:gap-2 p-2 sm:p-3 bg-red-600/10 rounded-lg text-xs sm:text-sm">
            <div className="col-span-6 flex items-center justify-center space-x-2 text-red-600">
              <div className="w-3 h-3 sm:w-4 sm:h-4 border-2 border-red-600 border-t-transparent rounded-full animate-spin" />
              <span>Esperando siguiente paso...</span>
            </div>
          </div>
        )}
      </div>

      {/* Selected Step Details */}
      {selectedStep !== null && (
        <div className="mt-4 sm:mt-6 p-3 sm:p-4 bg-gray-700 rounded-lg border border-gray-600">
          <h4 className="font-semibold text-white mb-3 text-sm sm:text-base">Detalles del Paso #{selectedStep}</h4>
          {(() => {
            const step = steps.find(s => s.stepId === selectedStep);
            if (!step) return <p className="text-gray-400 text-sm">Paso no encontrado</p>;
            
            return (
              <div className="grid grid-cols-1 lg:grid-cols-2 gap-3 sm:gap-4 text-xs sm:text-sm">
                <div>
                  <div className="space-y-2">
                    <div className="flex justify-between">
                      <span className="text-gray-400">Nodo Actual:</span>
                      <span className="font-bold text-red-600">{step.currentNode}</span>
                    </div>
                    
                    <div className="flex justify-between">
                      <span className="text-gray-400">G-Score:</span>
                      <span className="font-mono text-green-500">{step.gScore.toFixed(3)}</span>
                    </div>
                    
                    {step.fScore && (
                      <div className="flex justify-between">
                        <span className="text-gray-400">F-Score:</span>
                        <span className="font-mono text-yellow-500">{step.fScore.toFixed(3)}</span>
                      </div>
                    )}
                    
                    <div className="flex justify-between">
                      <span className="text-gray-400">Timestamp:</span>
                      <span className="font-mono text-white">{step.timestamp}ms</span>
                    </div>
                  </div>
                </div>
                
                <div>
                  <div className="space-y-3">
                    <div>
                      <div className="text-gray-400 mb-1">Conjunto Abierto ({step.openSet.length} nodos):</div>
                      <div className="bg-gray-800 p-2 rounded text-xs font-mono max-h-16 overflow-y-auto">
                        {step.openSet.length > 0 ? step.openSet.join(', ') : 'Vacío'}
                      </div>
                    </div>
                    
                    <div>
                      <div className="text-gray-400 mb-1">Conjunto Cerrado ({step.closedSet.length} nodos):</div>
                      <div className="bg-gray-800 p-2 rounded text-xs font-mono max-h-16 overflow-y-auto">
                        {step.closedSet.length > 0 ? step.closedSet.join(', ') : 'Vacío'}
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            );
          })()} 
        </div>
      )}

      {/* Buffer Status */}
      {steps.length > 0 && (
        <div className="mt-3 sm:mt-4 p-2 sm:p-3 bg-gray-700 rounded-lg">
          <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between text-xs sm:text-sm space-y-2 sm:space-y-0 sm:space-x-4">
            <span className="text-gray-400">
              Buffer: {steps.length}/5000 pasos
            </span>
            
            <div className="flex flex-col sm:flex-row sm:items-center space-y-2 sm:space-y-0 sm:space-x-4">
              <span className="text-gray-500">
                Algoritmo: {routeAlgorithm?.toUpperCase() || 'Desconocido'}
              </span>
              
              <div className="w-24 sm:w-32 bg-gray-800 rounded-full h-1.5 sm:h-2">
                <div 
                  className="bg-red-600 h-1.5 sm:h-2 rounded-full transition-all duration-300"
                  style={{ width: `${(steps.length / 5000) * 100}%` }}
                />
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default AlgorithmTimeline;