/**
 * Results Panel Component
 * Display route calculation results, execution status, and detailed metrics
 */

import { useState, useMemo, FC } from 'react';
import { useCurrentRoute, useIsComputing, useTimelineState } from '../store';
import { styleTokens } from '../styles/tokens';

interface ResultsPanelProps {
  onExportResults?: () => void;
  onShareResults?: () => void;
}

const ResultsPanel: FC<ResultsPanelProps> = ({ 
  onExportResults, 
  onShareResults 
}) => {
  const currentRoute = useCurrentRoute();
  const isComputing = useIsComputing();
  const { stepCount, streaming } = useTimelineState();
  const [expandedSection, setExpandedSection] = useState<string | null>('summary');

  // Calculate efficiency metrics
  const efficiency = useMemo(() => {
    if (currentRoute.nodes.length === 0 || currentRoute.expanded === 0) {
      return {
        efficiency: 0,
        explorationRatio: 0,
        avgCostPerNode: 0,
        pathOptimality: 'Unknown'
      };
    }

    const pathLength = currentRoute.nodes.length;
    const nodesExpanded = currentRoute.expanded;
    const efficiency = ((pathLength / nodesExpanded) * 100);
    const explorationRatio = (nodesExpanded / pathLength);
    const avgCostPerNode = currentRoute.cost / pathLength;
    
    let pathOptimality = 'Good';
    if (efficiency > 80) pathOptimality = 'Excellent';
    else if (efficiency > 60) pathOptimality = 'Very Good';
    else if (efficiency > 40) pathOptimality = 'Good';
    else if (efficiency > 20) pathOptimality = 'Fair';
    else pathOptimality = 'Poor';

    return {
      efficiency: efficiency,
      explorationRatio,
      avgCostPerNode,
      pathOptimality
    };
  }, [currentRoute]);

  const formatExecutionTime = (ms: number): string => {
    if (ms < 1) return `${(ms * 1000).toFixed(0)}μs`;
    if (ms < 1000) return `${ms.toFixed(2)}ms`;
    return `${(ms / 1000).toFixed(2)}s`;
  };

  const formatRouteNodes = (nodes: number[]): string => {
    if (nodes.length === 0) return 'No route';
    if (nodes.length <= 10) return nodes.join(' → ');
    return `${nodes.slice(0, 3).join(' → ')} ... ${nodes.slice(-3).join(' → ')} (${nodes.length} nodes)`;
  };

  const getStatusIcon = () => {
    if (isComputing) {
      return (
        <div className="w-5 h-5 border-2 border-accent-primary border-t-transparent rounded-full animate-spin" />
      );
    }
    
    if (currentRoute.error) {
      return (
        <svg className="w-5 h-5 text-accent-hover" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
        </svg>
      );
    }
    
    if (currentRoute.nodes.length > 0) {
      return (
        <svg className="w-5 h-5 text-accent-success" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clipRule="evenodd" />
        </svg>
      );
    }
    
    return (
      <svg className="w-5 h-5 text-text-muted" fill="currentColor" viewBox="0 0 20 20">
        <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7-4a1 1 0 11-2 0 1 1 0 012 0zM9 9a1 1 0 000 2v3a1 1 0 001 1h1a1 1 0 100-2v-3a1 1 0 00-1-1H9z" clipRule="evenodd" />
      </svg>
    );
  };

  const getStatusText = () => {
    if (isComputing) return 'Computing route...';
    if (currentRoute.error) return 'Error occurred';
    if (currentRoute.nodes.length > 0) return 'Route found';
    return 'No route calculated';
  };

  const getStatusColor = () => {
    if (isComputing) return 'text-accent-primary';
    if (currentRoute.error) return 'text-accent-hover';
    if (currentRoute.nodes.length > 0) return 'text-accent-success';
    return 'text-text-muted';
  };

  const toggleSection = (section: string) => {
    setExpandedSection(expandedSection === section ? null : section);
  };

  return (
    <div className={styleTokens.card}>
      {/* Header */}
      <div className="flex items-center justify-between mb-6">
        <div>
          <h3 className={styleTokens.text.heading}>Route Results</h3>
          <div className="flex items-center space-x-2 mt-1">
            {getStatusIcon()}
            <span className={`text-sm font-medium ${getStatusColor()}`}>
              {getStatusText()}
            </span>
            {streaming && (
              <span className="text-xs text-accent-primary">(Live updates)</span>
            )}
          </div>
        </div>
        
        {currentRoute.nodes.length > 0 && (
          <div className="flex items-center space-x-2">
            {onExportResults && (
              <button
                onClick={onExportResults}
                className={`${styleTokens.button.secondary} text-sm px-3 py-1`}
              >
                Export
              </button>
            )}
            
            {onShareResults && (
              <button
                onClick={onShareResults}
                className={`${styleTokens.button.secondary} text-sm px-3 py-1`}
              >
                Share
              </button>
            )}
          </div>
        )}
      </div>

      {/* Error Display */}
      {currentRoute.error && (
        <div className="mb-6 p-4 bg-accent-hover/10 border border-accent-hover/20 rounded-lg">
          <div className="flex items-center space-x-2 mb-2">
            <svg className="w-5 h-5 text-accent-hover" fill="currentColor" viewBox="0 0 20 20">
              <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
            </svg>
            <span className="font-semibold text-accent-hover">Calculation Error</span>
          </div>
          <p className="text-text-secondary">{currentRoute.error}</p>
        </div>
      )}

      {/* Quick Stats */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-6">
        <div className="text-center">
          <div className="text-2xl font-bold text-accent-primary">
            {currentRoute.nodes.length}
          </div>
          <div className="text-xs text-text-muted">Path Nodes</div>
        </div>
        
        <div className="text-center">
          <div className="text-2xl font-bold text-accent-success">
            {currentRoute.cost > 0 ? currentRoute.cost.toFixed(1) : '—'}
          </div>
          <div className="text-xs text-text-muted">Total Cost</div>
        </div>
        
        <div className="text-center">
          <div className="text-2xl font-bold text-accent-warning">
            {currentRoute.expanded}
          </div>
          <div className="text-xs text-text-muted">Nodes Expanded</div>
        </div>
        
        <div className="text-center">
          <div className="text-2xl font-bold text-text-primary">
            {formatExecutionTime(currentRoute.executionTime)}
          </div>
          <div className="text-xs text-text-muted">Execution Time</div>
        </div>
      </div>

      {/* Expandable Sections */}
      <div className="space-y-4">
        {/* Summary Section */}
        <div className="border border-border-muted rounded-lg">
          <button
            onClick={() => toggleSection('summary')}
            className="w-full flex items-center justify-between p-4 hover:bg-bg-elevated transition-colors"
          >
            <span className="font-semibold text-text-primary">Route Summary</span>
            <svg 
              className={`w-5 h-5 text-text-secondary transition-transform ${
                expandedSection === 'summary' ? 'rotate-180' : ''
              }`} 
              fill="currentColor" 
              viewBox="0 0 20 20"
            >
              <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
            </svg>
          </button>
          
          {expandedSection === 'summary' && currentRoute.nodes.length > 0 && (
            <div className="px-4 pb-4 border-t border-border-muted">
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mt-4">
                <div className="space-y-3">
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Algorithm:</span>
                    <span className="font-semibold text-text-primary">
                      {currentRoute.algorithm?.toUpperCase()}
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Metric:</span>
                    <span className="font-semibold text-text-primary">
                      {currentRoute.metric || 'Distance'}
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Source Node:</span>
                    <span className="font-bold text-accent-success">
                      {currentRoute.nodes[0]}
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Target Node:</span>
                    <span className="font-bold text-accent-warning">
                      {currentRoute.nodes[currentRoute.nodes.length - 1]}
                    </span>
                  </div>
                </div>
                
                <div className="space-y-3">
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Path Length:</span>
                    <span className="font-semibold text-text-primary">
                      {currentRoute.nodes.length} nodes
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Avg Cost/Node:</span>
                    <span className="font-semibold text-text-primary">
                      {efficiency.avgCostPerNode.toFixed(2)}
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Efficiency:</span>
                    <span className="font-semibold text-accent-primary">
                      {efficiency.efficiency.toFixed(1)}%
                    </span>
                  </div>
                  
                  <div className="flex justify-between">
                    <span className="text-text-secondary">Quality:</span>
                    <span className={`font-semibold ${
                      efficiency.pathOptimality === 'Excellent' ? 'text-accent-success' :
                      efficiency.pathOptimality === 'Very Good' ? 'text-accent-primary' :
                      efficiency.pathOptimality === 'Good' ? 'text-accent-warning' :
                      'text-accent-hover'
                    }`}>
                      {efficiency.pathOptimality}
                    </span>
                  </div>
                </div>
              </div>
            </div>
          )}
        </div>

        {/* Path Details Section */}
        {currentRoute.nodes.length > 0 && (
          <div className="border border-border-muted rounded-lg">
            <button
              onClick={() => toggleSection('path')}
              className="w-full flex items-center justify-between p-4 hover:bg-bg-elevated transition-colors"
            >
              <span className="font-semibold text-text-primary">Path Details</span>
              <svg 
                className={`w-5 h-5 text-text-secondary transition-transform ${
                  expandedSection === 'path' ? 'rotate-180' : ''
                }`} 
                fill="currentColor" 
                viewBox="0 0 20 20"
              >
                <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
              </svg>
            </button>
            
            {expandedSection === 'path' && (
              <div className="px-4 pb-4 border-t border-border-muted">
                <div className="mt-4">
                  <h5 className="font-semibold text-text-primary mb-3">Complete Path</h5>
                  <div className="bg-bg-surface p-3 rounded-lg max-h-32 overflow-y-auto font-mono text-sm">
                    {formatRouteNodes(currentRoute.nodes)}
                  </div>
                  
                  {currentRoute.nodes.length > 10 && (
                    <p className="text-xs text-text-muted mt-2">
                      Path contains {currentRoute.nodes.length} nodes total
                    </p>
                  )}
                </div>
              </div>
            )}
          </div>
        )}

        {/* Performance Section */}
        {(currentRoute.executionTime > 0 || stepCount > 0) && (
          <div className="border border-border-muted rounded-lg">
            <button
              onClick={() => toggleSection('performance')}
              className="w-full flex items-center justify-between p-4 hover:bg-bg-elevated transition-colors"
            >
              <span className="font-semibold text-text-primary">Performance Analysis</span>
              <svg 
                className={`w-5 h-5 text-text-secondary transition-transform ${
                  expandedSection === 'performance' ? 'rotate-180' : ''
                }`} 
                fill="currentColor" 
                viewBox="0 0 20 20"
              >
                <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
              </svg>
            </button>
            
            {expandedSection === 'performance' && (
              <div className="px-4 pb-4 border-t border-border-muted">
                <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mt-4">
                  <div>
                    <h5 className="font-semibold text-text-primary mb-3">Execution Metrics</h5>
                    <div className="space-y-2 text-sm">
                      <div className="flex justify-between">
                        <span className="text-text-secondary">Execution Time:</span>
                        <span className="font-mono text-accent-primary">
                          {formatExecutionTime(currentRoute.executionTime)}
                        </span>
                      </div>
                      
                      <div className="flex justify-between">
                        <span className="text-text-secondary">Timeline Steps:</span>
                        <span className="font-mono text-accent-success">{stepCount}</span>
                      </div>
                      
                      <div className="flex justify-between">
                        <span className="text-text-secondary">Exploration Ratio:</span>
                        <span className="font-mono text-accent-warning">
                          {efficiency.explorationRatio.toFixed(2)}x
                        </span>
                      </div>
                    </div>
                  </div>
                  
                  <div>
                    <h5 className="font-semibold text-text-primary mb-3">Algorithm Efficiency</h5>
                    <div className="space-y-3">
                      <div>
                        <div className="flex justify-between text-sm mb-1">
                          <span className="text-text-secondary">Search Efficiency</span>
                          <span className="font-semibold text-accent-primary">
                            {efficiency.efficiency.toFixed(1)}%
                          </span>
                        </div>
                        <div className="w-full bg-bg-surface rounded-full h-2">
                          <div 
                            className="bg-accent-primary h-2 rounded-full transition-all duration-500"
                            style={{ width: `${Math.min(efficiency.efficiency, 100)}%` }}
                          />
                        </div>
                      </div>
                      
                      <div className="text-xs text-text-muted">
                        {efficiency.efficiency > 80 ? 
                          'Excellent - Very direct path found' :
                          efficiency.efficiency > 60 ?
                          'Very Good - Efficient pathfinding' :
                          efficiency.efficiency > 40 ?
                          'Good - Reasonable exploration' :
                          efficiency.efficiency > 20 ?
                          'Fair - Some inefficiency detected' :
                          'Poor - High exploration overhead'
                        }
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            )}
          </div>
        )}
      </div>

      {/* Empty State */}
      {currentRoute.nodes.length === 0 && !isComputing && !currentRoute.error && (
        <div className="text-center py-8">
          <div className="w-16 h-16 bg-bg-elevated rounded-full flex items-center justify-center mx-auto mb-4">
            <svg className="w-8 h-8 text-text-muted" fill="currentColor" viewBox="0 0 20 20">
              <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
          </div>
          <h4 className={styleTokens.text.heading}>No Results</h4>
          <p className={styleTokens.text.caption}>
            Configure route parameters and click "Calculate Route" to see results
          </p>
        </div>
      )}
    </div>
  );
};

export default ResultsPanel;
