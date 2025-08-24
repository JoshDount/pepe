/**
 * Algorithm Timeline Component
 * Real-time visualization of algorithm execution steps with SSE streaming
 */

import { useEffect, useState, useCallback, FC } from 'react';
import { useTimelineState, useTimelineActions, useCurrentRoute } from '../store';
import { styleTokens } from '../styles/tokens';
import type { ExpansionStep } from '../types/api';

interface AlgorithmTimelineProps {
  maxVisibleSteps?: number;
}

const AlgorithmTimeline: FC<AlgorithmTimelineProps> = ({ 
  maxVisibleSteps = 50 
}) => {
  const { steps, streaming, stepCount, currentStep } = useTimelineState();
  const { startStreaming, stopStreaming, clearTimeline } = useTimelineActions();
  const currentRoute = useCurrentRoute();
  
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
      <div className={styleTokens.card}>
        <div className="flex items-center justify-between mb-4">
          <h3 className={styleTokens.text.heading}>Algorithm Timeline</h3>
          <span className="text-sm text-text-muted">No timeline data</span>
        </div>
        
        <div className="text-center py-12">
          <div className="w-16 h-16 bg-bg-elevated rounded-full flex items-center justify-center mx-auto mb-4">
            <svg className="w-8 h-8 text-text-muted" fill="currentColor" viewBox="0 0 20 20">
              <path d="M10 18a8 8 0 100-16 8 8 0 000 16zm1-13a1 1 0 10-2 0v4a1 1 0 00.293.707l2.828 2.829a1 1 0 101.415-1.415L11 9.586V5z" />
            </svg>
          </div>
          <h4 className={styleTokens.text.heading}>No Algorithm Steps</h4>
          <p className={styleTokens.text.caption}>
            Enable debug mode and calculate a route to see algorithm execution steps
          </p>
        </div>
      </div>
    );
  }

  return (
    <div className={styleTokens.card}>
      {/* Header */}
      <div className="flex items-center justify-between mb-6">
        <div>
          <h3 className={styleTokens.text.heading}>Algorithm Timeline</h3>
          <p className={styleTokens.text.caption}>
            {streaming ? 'Streaming live updates...' : 
             `Showing ${displaySteps.length} of ${stepCount} total steps`}
          </p>
        </div>
        
        <div className="flex items-center space-x-3">
          {/* Streaming Indicator */}
          {streaming && (
            <div className="flex items-center space-x-2 text-accent-primary">
              <div className="w-2 h-2 bg-accent-primary rounded-full animate-pulse" />
              <span className="text-sm font-medium">Live</span>
            </div>
          )}
          
          {/* Auto-scroll Toggle */}
          <button
            onClick={handleToggleAutoScroll}
            className={`text-sm px-3 py-1 rounded-md transition-colors ${
              autoScroll 
                ? 'bg-accent-primary text-white' 
                : 'bg-bg-elevated text-text-secondary hover:text-text-primary'
            }`}
          >
            Auto-scroll
          </button>
          
          {/* Clear Button */}
          <button
            onClick={handleClearTimeline}
            disabled={streaming}
            className={`${styleTokens.button.secondary} text-sm px-3 py-1`}
          >
            Clear
          </button>
        </div>
      </div>

      {/* Stats Bar */}
      <div className="grid grid-cols-3 gap-4 mb-6">
        <div className="text-center">
          <div className="text-xl font-bold text-accent-primary">{stepCount}</div>
          <div className="text-xs text-text-muted">Total Steps</div>
        </div>
        
        <div className="text-center">
          <div className="text-xl font-bold text-accent-success">{currentRoute.expanded}</div>
          <div className="text-xs text-text-muted">Nodes Expanded</div>
        </div>
        
        <div className="text-center">
          <div className="text-xl font-bold text-accent-warning">
            {steps.length > 0 ? steps[steps.length - 1]?.currentNode || '—' : '—'}
          </div>
          <div className="text-xs text-text-muted">Current Node</div>
        </div>
      </div>

      {/* Timeline Header */}
      <div className="grid grid-cols-6 gap-2 p-3 bg-bg-elevated rounded-lg mb-2 text-xs font-semibold text-text-secondary">
        <div>Step</div>
        <div>Current</div>
        <div>G-Score</div>
        <div>F-Score</div>
        <div>Open Set</div>
        <div>Closed Set</div>
      </div>

      {/* Timeline Steps */}
      <div className="space-y-1 max-h-96 overflow-y-auto custom-scrollbar">
        {displaySteps.map((step, index) => (
          <div
            key={step.stepId}
            className={getStepRowClass(step)}
            onClick={() => handleStepClick(step.stepId)}
          >
            <div className="font-mono text-accent-primary">
              #{step.stepId}
            </div>
            
            <div className="font-bold text-text-primary">
              {step.currentNode}
            </div>
            
            <div className="font-mono text-accent-success">
              {step.gScore.toFixed(2)}
            </div>
            
            <div className="font-mono text-accent-warning">
              {step.fScore ? step.fScore.toFixed(2) : '—'}
            </div>
            
            <div className="text-text-secondary text-xs truncate" title={formatNodeList(step.openSet)}>
              [{formatNodeList(step.openSet)}]
            </div>
            
            <div className="text-text-muted text-xs truncate" title={formatNodeList(step.closedSet)}>
              [{formatNodeList(step.closedSet)}]
            </div>
          </div>
        ))}
        
        {streaming && (
          <div className="grid grid-cols-6 gap-2 p-3 bg-accent-primary/10 rounded-lg text-sm">
            <div className="col-span-6 flex items-center justify-center space-x-2 text-accent-primary">
              <div className="w-4 h-4 border-2 border-accent-primary border-t-transparent rounded-full animate-spin" />
              <span>Waiting for next step...</span>
            </div>
          </div>
        )}
      </div>

      {/* Selected Step Details */}
      {selectedStep !== null && (
        <div className="mt-6 p-4 bg-bg-elevated rounded-lg border border-border-muted">
          <h4 className="font-semibold text-text-primary mb-3">Step #{selectedStep} Details</h4>
          {(() => {
            const step = steps.find(s => s.stepId === selectedStep);
            if (!step) return <p className="text-text-muted">Step not found</p>;
            
            return (
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4 text-sm">
                <div>
                  <div className="space-y-2">
                    <div className="flex justify-between">
                      <span className="text-text-secondary">Current Node:</span>
                      <span className="font-bold text-accent-primary">{step.currentNode}</span>
                    </div>
                    
                    <div className="flex justify-between">
                      <span className="text-text-secondary">G-Score:</span>
                      <span className="font-mono text-accent-success">{step.gScore.toFixed(3)}</span>
                    </div>
                    
                    {step.fScore && (
                      <div className="flex justify-between">
                        <span className="text-text-secondary">F-Score:</span>
                        <span className="font-mono text-accent-warning">{step.fScore.toFixed(3)}</span>
                      </div>
                    )}
                    
                    <div className="flex justify-between">
                      <span className="text-text-secondary">Timestamp:</span>
                      <span className="font-mono text-text-primary">{step.timestamp}ms</span>
                    </div>
                  </div>
                </div>
                
                <div>
                  <div className="space-y-3">
                    <div>
                      <div className="text-text-secondary mb-1">Open Set ({step.openSet.length} nodes):</div>
                      <div className="bg-bg-surface p-2 rounded text-xs font-mono max-h-16 overflow-y-auto">
                        {step.openSet.length > 0 ? step.openSet.join(', ') : 'Empty'}
                      </div>
                    </div>
                    
                    <div>
                      <div className="text-text-secondary mb-1">Closed Set ({step.closedSet.length} nodes):</div>
                      <div className="bg-bg-surface p-2 rounded text-xs font-mono max-h-16 overflow-y-auto">
                        {step.closedSet.length > 0 ? step.closedSet.join(', ') : 'Empty'}
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
        <div className="mt-4 p-3 bg-bg-elevated rounded-lg">
          <div className="flex items-center justify-between text-sm">
            <span className="text-text-secondary">
              Buffer: {steps.length}/5000 steps
            </span>
            
            <div className="flex items-center space-x-4">
              <span className="text-text-muted">
                Algorithm: {currentRoute.algorithm?.toUpperCase() || 'Unknown'}
              </span>
              
              <div className="w-32 bg-bg-surface rounded-full h-2">
                <div 
                  className="bg-accent-primary h-2 rounded-full transition-all duration-300"
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