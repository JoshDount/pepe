/**
 * Control Panel Component
 * Main interface for route calculation and algorithm selection
 */

import { useState, useCallback, FC } from 'react';
import { useRouteActions, useIsComputing, useGraphState } from '../store';
import { styleTokens } from '../styles/tokens';
import type { RouteParams } from '../types/api';

interface ControlPanelProps {
  onRouteCalculated?: (result: any) => void;
}

const ControlPanel: FC<ControlPanelProps> = ({ onRouteCalculated }) => {
  const { calculateRoute, clearRoute } = useRouteActions();
  const isComputing = useIsComputing();
  const { loaded, size } = useGraphState();

  // Form state
  const [formData, setFormData] = useState<RouteParams>({
    source: 1,
    target: 50,
    algo: 'dijkstra',
    metric: 'distance',
    debug: true,
  });

  const [errors, setErrors] = useState<Record<string, string>>({});

  // Validation
  const validateForm = useCallback((): boolean => {
    const newErrors: Record<string, string> = {};

    if (!formData.source || formData.source <= 0) {
      newErrors.source = 'Source must be a positive integer';
    }
    if (!formData.target || formData.target <= 0) {
      newErrors.target = 'Target must be a positive integer';
    }
    if (formData.source === formData.target) {
      newErrors.target = 'Target must be different from source';
    }
    if (loaded && (formData.source > size || formData.target > size)) {
      newErrors.range = `Node IDs must be between 1 and ${size}`;
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
      console.error('Route calculation failed:', error);
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
    <div className={styleTokens.card}>
      {/* Header */}
      <div className="flex items-center justify-between mb-6">
        <div>
          <h2 className={styleTokens.text.heading}>Route Planning Control</h2>
          <p className={styleTokens.text.caption}>
            Configure parameters and calculate optimal routes
          </p>
        </div>

        {/* Status Indicator */}
        <div className="flex items-center space-x-2">
          <div className={`w-3 h-3 rounded-full ${
            !loaded ? 'bg-text-muted' :
            isComputing ? 'bg-accent-warning animate-pulse' :
            'bg-accent-success'
          }`} />
          <span className="text-sm text-text-secondary">
            {!loaded ? 'No Graph' : isComputing ? 'Computing...' : 'Ready'}
          </span>
        </div>
      </div>

      {/* Main Form */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
        {/* Left Column - Route Parameters */}
        <div className="space-y-6">
          <div>
            <h3 className="font-semibold text-text-primary mb-4">Route Parameters</h3>
            
            {/* Source and Target */}
            <div className="grid grid-cols-2 gap-4 mb-4">
              <div>
                <label className="block text-sm font-medium text-text-secondary mb-2">
                  Source Node
                </label>
                <input
                  type="number"
                  min="1"
                  max={loaded ? size : undefined}
                  value={formData.source}
                  onChange={(e) => handleInputChange('source', parseInt(e.target.value) || 1)}
                  className={`${styleTokens.input} w-full ${errors.source ? 'border-accent-error' : ''}`}
                  placeholder="Enter source node ID"
                  disabled={isComputing}
                />
                {errors.source && (
                  <p className="text-xs text-accent-error mt-1">{errors.source}</p>
                )}
              </div>

              <div>
                <label className="block text-sm font-medium text-text-secondary mb-2">
                  Target Node
                </label>
                <input
                  type="number"
                  min="1"
                  max={loaded ? size : undefined}
                  value={formData.target}
                  onChange={(e) => handleInputChange('target', parseInt(e.target.value) || 1)}
                  className={`${styleTokens.input} w-full ${errors.target ? 'border-accent-error' : ''}`}
                  placeholder="Enter target node ID"
                  disabled={isComputing}
                />
                {errors.target && (
                  <p className="text-xs text-accent-error mt-1">{errors.target}</p>
                )}
              </div>
            </div>

            {/* Range Error */}
            {errors.range && (
              <p className="text-xs text-accent-error mb-4">{errors.range}</p>
            )}

            {/* Random Button */}
            <button
              onClick={handleRandomNodes}
              disabled={!loaded || isComputing}
              className={`${styleTokens.button.secondary} text-sm mb-4`}
            >
              Random Nodes
            </button>
          </div>

          {/* Algorithm Selection */}
          <div>
            <label className="block text-sm font-medium text-text-secondary mb-2">
              Algorithm
            </label>
            <div className="grid grid-cols-2 gap-2">
              <button
                onClick={() => handleInputChange('algo', 'dijkstra')}
                disabled={isComputing}
                className={`p-3 rounded-lg border transition-all ${
                  formData.algo === 'dijkstra'
                    ? 'bg-accent-primary border-accent-primary text-white'
                    : 'bg-bg-surface border-border-default text-text-primary hover:border-accent-primary'
                }`}
              >
                <div className="font-semibold">Dijkstra</div>
                <div className="text-xs opacity-80">Guaranteed shortest path</div>
              </button>

              <button
                onClick={() => handleInputChange('algo', 'astar')}
                disabled={isComputing}
                className={`p-3 rounded-lg border transition-all ${
                  formData.algo === 'astar'
                    ? 'bg-accent-primary border-accent-primary text-white'
                    : 'bg-bg-surface border-border-default text-text-primary hover:border-accent-primary'
                }`}
              >
                <div className="font-semibold">A*</div>
                <div className="text-xs opacity-80">Heuristic-guided</div>
              </button>
            </div>
          </div>

          {/* Metric Selection */}
          <div>
            <label className="block text-sm font-medium text-text-secondary mb-2">
              Optimization Metric
            </label>
            <select
              value={formData.metric}
              onChange={(e) => handleInputChange('metric', e.target.value as 'time' | 'distance' | 'cost')}
              className={`${styleTokens.input} w-full`}
              disabled={isComputing}
            >
              <option value="distance">Distance</option>
              <option value="time">Time</option>
              <option value="cost">Cost</option>
            </select>
          </div>
        </div>

        {/* Right Column - Options and Actions */}
        <div className="space-y-6">
          <div>
            <h3 className="font-semibold text-text-primary mb-4">Options</h3>
            
            {/* Debug Mode */}
            <div className="flex items-center justify-between p-3 bg-bg-elevated rounded-lg">
              <div>
                <div className="font-medium text-text-primary">Debug Mode</div>
                <div className="text-sm text-text-secondary">Enable algorithm timeline</div>
              </div>
              <label className="relative inline-flex items-center cursor-pointer">
                <input
                  type="checkbox"
                  checked={formData.debug}
                  onChange={(e) => handleInputChange('debug', e.target.checked)}
                  className="sr-only peer"
                  disabled={isComputing}
                />
                <div className="w-11 h-6 bg-bg-surface peer-focus:ring-2 peer-focus:ring-accent-primary rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-accent-primary"></div>
              </label>
            </div>
          </div>

          {/* Graph Info */}
          {loaded && (
            <div className="bg-bg-elevated rounded-lg p-4">
              <h4 className="font-medium text-text-primary mb-3">Graph Information</h4>
              <div className="space-y-2 text-sm">
                <div className="flex justify-between">
                  <span className="text-text-secondary">Nodes:</span>
                  <span className="text-text-primary font-medium">{size.toLocaleString()}</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-text-secondary">Status:</span>
                  <span className="text-accent-success font-medium">Loaded</span>
                </div>
                <div className="flex justify-between">
                  <span className="text-text-secondary">Range:</span>
                  <span className="text-text-primary font-medium">1 - {size}</span>
                </div>
              </div>
            </div>
          )}

          {/* Action Buttons */}
          <div className="space-y-3">
            <button
              onClick={handleCalculateRoute}
              disabled={!loaded || isComputing}
              className={`${styleTokens.button.primary} w-full py-3 ${
                isComputing ? 'opacity-50 cursor-not-allowed' : ''
              }`}
            >
              {isComputing ? (
                <div className="flex items-center justify-center space-x-2">
                  <div className="w-4 h-4 border-2 border-white border-t-transparent rounded-full animate-spin" />
                  <span>Computing Route...</span>
                </div>
              ) : (
                'Calculate Route'
              )}
            </button>

            <button
              onClick={handleClearRoute}
              disabled={isComputing}
              className={`${styleTokens.button.secondary} w-full`}
            >
              Clear Route
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ControlPanel;