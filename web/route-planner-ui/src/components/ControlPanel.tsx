/**
 * Control Panel Component
 * Main interface for route calculation and algorithm selection
 */

import { useState, useCallback, useEffect, FC } from 'react';
import { useCalculateRoute, useClearRoute, useIsComputing, useGraphState } from '../store';
import type { RouteParams } from '../types/api';

interface ControlPanelProps {
  onRouteCalculated?: (result: any) => void;
}

const ControlPanel: FC<ControlPanelProps> = ({ onRouteCalculated }) => {
  const calculateRoute = useCalculateRoute();
  const clearRoute = useClearRoute();
  const isComputing = useIsComputing();
  const { loaded, size } = useGraphState();

  // Form state
  const [formData, setFormData] = useState<RouteParams>({
    source: 1,
    target: 50,
    algo: 'dijkstra',
    metric: 'distance',
    debug: false,
  });

  const [errors, setErrors] = useState<Record<string, string>>({});

  // Clamp form values to graph size when graph loads/changes
  useEffect(() => {
    if (!loaded || size <= 0) return;
    setFormData(prev => {
      const clamp = (v: number) => Math.min(Math.max(1, v || 1), size);
      let src = clamp(prev.source);
      let tgt = clamp(prev.target);
      if (src === tgt) tgt = clamp(src === size ? src - 1 : src + 1);
      return { ...prev, source: src, target: tgt };
    });
  }, [loaded, size]);

  // Validation
  const validateForm = useCallback((): boolean => {
    const newErrors: Record<string, string> = {};

    if (!formData.source || formData.source <= 0) {
      newErrors.source = 'El nodo origen debe ser un entero positivo';
    }
    if (!formData.target || formData.target <= 0) {
      newErrors.target = 'El nodo destino debe ser un entero positivo';
    }
    if (formData.source === formData.target) {
      newErrors.target = 'El nodo destino debe ser diferente al origen';
    }
    if (loaded && (formData.source > size || formData.target > size)) {
      newErrors.range = `Los IDs de nodos deben estar entre 1 y ${size}`;
    }

    setErrors(newErrors);
    return Object.keys(newErrors).length === 0;
  }, [formData, loaded, size]);

  // Handlers
  const handleInputChange = useCallback((field: keyof RouteParams, value: any) => {
    setFormData(prev => ({ ...prev, [field]: value }));
    if (errors[field]) {
      setErrors(prev => ({ ...prev, [field]: '' }));
    }
  }, [errors]);

  const handleCalculateRoute = useCallback(async () => {
    if (!validateForm()) return;

    try {
      await calculateRoute(formData);
      onRouteCalculated?.(formData);
    } catch (error) {
      console.error('Error en el cálculo de ruta:', error);
    }
  }, [formData, validateForm, calculateRoute, onRouteCalculated]);

  const handleClearRoute = useCallback(() => {
    clearRoute();
    setErrors({});
  }, [clearRoute]);

  const handleRandomNodes = useCallback(() => {
    if (!loaded) return;

    const source = Math.floor(Math.random() * size) + 1;
    let target = Math.floor(Math.random() * size) + 1;
    while (target === source) {
      target = Math.floor(Math.random() * size) + 1;
    }

    setFormData(prev => ({ ...prev, source, target }));
    setErrors({});
  }, [loaded, size]);

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
      {/* Header */}
      <div className="flex items-center justify-between mb-4 sm:mb-6">
        <h2 className="text-xl sm:text-2xl font-semibold text-white">Controles de Dijkstra</h2>

        {/* Status Indicator */}
        <div className="flex items-center space-x-2">
          <div className={`w-2 h-2 sm:w-3 sm:h-3 rounded-full ${
            !loaded ? 'bg-gray-500' :
            isComputing ? 'bg-yellow-500 animate-pulse' :
            'bg-green-500'
          }`} />
          <span className="text-xs sm:text-sm text-gray-400">
            {!loaded ? 'Sin Grafo' : isComputing ? 'Calculando...' : 'Listo'}
          </span>
        </div>
      </div>

      {/* Main Form */}
      <div className="grid grid-cols-1 gap-6 sm:gap-8">
        <div className="space-y-4 sm:space-y-6">
          <div>
            <h3 className="text-lg sm:text-xl font-semibold text-white mb-3 sm:mb-4">Configuración de Nodos</h3>
            
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-3 sm:gap-4">
              {/* Source Node */}
              <div>
                <label className="block text-xs sm:text-sm font-medium text-gray-300 mb-1 sm:mb-2">
                  Nodo Origen
                </label>
                <input
                  type="number"
                  value={formData.source}
                  onChange={(e) => handleInputChange('source', parseInt(e.target.value) || 0)}
                  className="bg-gray-700 border border-gray-600 rounded-lg px-2 sm:px-3 py-1.5 sm:py-2 text-white placeholder:text-gray-400 focus:border-red-600 focus:ring-2 focus:ring-red-600 focus:ring-opacity-50 w-full text-sm"
                  placeholder="Ingrese ID del nodo origen"
                  min="1"
                  max={size}
                />
                {errors.source && (
                  <p className="text-red-500 text-xs mt-1">{errors.source}</p>
                )}
              </div>

              {/* Target Node */}
              <div>
                <label className="block text-xs sm:text-sm font-medium text-gray-300 mb-1 sm:mb-2">
                  Nodo Destino
                </label>
                <input
                  type="number"
                  value={formData.target}
                  onChange={(e) => handleInputChange('target', parseInt(e.target.value) || 0)}
                  className="bg-gray-700 border border-gray-600 rounded-lg px-2 sm:px-3 py-1.5 sm:py-2 text-white placeholder:text-gray-400 focus:border-red-600 focus:ring-2 focus:ring-red-600 focus:ring-opacity-50 w-full text-sm"
                  placeholder="Ingrese ID del nodo destino"
                  min="1"
                  max={size}
                />
                {errors.target && (
                  <p className="text-red-500 text-xs mt-1">{errors.target}</p>
                )}
              </div>
            </div>

            {/* Random Nodes Button */}
            <button
              onClick={handleRandomNodes}
              disabled={!loaded}
              className="mt-3 sm:mt-4 bg-gray-700 hover:bg-gray-600 disabled:bg-gray-800 disabled:text-gray-500 text-white font-medium py-1.5 sm:py-2 px-3 sm:px-4 rounded-lg transition-colors text-xs sm:text-sm"
            >
              Generar Nodos Aleatorios
            </button>

            {errors.range && (
              <p className="text-red-500 text-xs mt-2">{errors.range}</p>
            )}
          </div>
        </div>
      </div>

      {/* Action Buttons */}
      <div className="flex flex-col sm:flex-row gap-3 sm:gap-4 mt-6 sm:mt-8 pt-4 sm:pt-6 border-t border-gray-600">
        <button
          onClick={handleCalculateRoute}
          disabled={isComputing || !loaded}
          className="flex-1 bg-red-600 hover:bg-red-700 disabled:bg-gray-700 disabled:text-gray-400 text-white font-semibold py-2.5 sm:py-3 px-4 sm:px-6 rounded-lg transition-colors text-sm"
        >
          {isComputing ? (
            <div className="flex items-center justify-center space-x-2">
              <div className="w-3 h-3 sm:w-4 sm:h-4 border-2 border-white border-t-transparent rounded-full animate-spin" />
              <span>Calculando Ruta...</span>
            </div>
          ) : (
            'Calcular'
          )}
        </button>

        <button
          onClick={handleClearRoute}
          disabled={isComputing}
          className="bg-gray-700 hover:bg-gray-600 disabled:bg-gray-800 disabled:text-gray-500 text-white font-medium py-2.5 sm:py-3 px-4 sm:px-6 rounded-lg border border-gray-600 transition-colors text-sm"
        >
          Limpiar
        </button>
      </div>

      {/* Graph Info */}
      {loaded && (
        <div className="mt-4 sm:mt-6 p-3 sm:p-4 bg-gray-700/50 rounded-lg border border-gray-600">
          <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between text-xs sm:text-sm space-y-1 sm:space-y-0">
            <span className="text-gray-400">Información del Grafo:</span>
            <span className="text-white font-medium">
              {size} nodos disponibles (1-{size})
            </span>
          </div>
        </div>
      )}
    </div>
  );
};

export default ControlPanel;
