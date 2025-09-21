/**
 * Results Panel Component
 * Display route calculation results, execution status, and detailed metrics
 */

import { useState, useMemo, FC } from 'react';
import { useRouteNodes, useRouteCost, useRouteAlgorithm, useRouteExecutionTime, useRouteExpanded, useRouteError, useRouteMetric, useIsComputing, useTimelineState } from '../store';

interface ResultsPanelProps {
  onExportResults?: () => void;
  onShareResults?: () => void;
}

const ResultsPanel: FC<ResultsPanelProps> = ({ 
  onExportResults, 
  onShareResults 
}) => {
  const routeNodes = useRouteNodes();
  const routeCost = useRouteCost();
  const routeAlgorithm = useRouteAlgorithm();
  const routeExecutionTime = useRouteExecutionTime();
  const routeExpanded = useRouteExpanded();
  const routeError = useRouteError();
  const routeMetric = useRouteMetric();
  const isComputing = useIsComputing();
  const { stepCount, streaming } = useTimelineState();
  const [expandedSection, setExpandedSection] = useState<string | null>('summary');

  // Calculate efficiency metrics
  const efficiency = useMemo(() => {
    if (routeNodes.length === 0 || routeExpanded === 0) {
      return {
        efficiency: 0,
        explorationRatio: 0,
        avgCostPerNode: 0,
        pathOptimality: 'Unknown'
      };
    }

    const pathLength = routeNodes.length;
    const nodesExpanded = routeExpanded;
    const efficiency = ((pathLength / nodesExpanded) * 100);
    const explorationRatio = (nodesExpanded / pathLength);
    const avgCostPerNode = routeCost / pathLength;
    
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
  }, [routeNodes, routeExpanded, routeCost]);

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
    
    if (routeError) {
      return (
        <svg className="w-5 h-5 text-accent-hover" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
        </svg>
      );
    }
    
    if (routeNodes.length > 0) {
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
    if (routeError) return 'Error occurred';
    if (routeNodes.length > 0) return 'Route found';
    return 'No route calculated';
  };

  const getStatusColor = () => {
    if (isComputing) return 'text-accent-primary';
    if (routeError) return 'text-accent-hover';
    if (routeNodes.length > 0) return 'text-accent-success';
    return 'text-text-muted';
  };

  const toggleSection = (section: string) => {
    setExpandedSection(expandedSection === section ? null : section);
  };

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between mb-4 sm:mb-6 space-y-3 sm:space-y-0">
        <div>
          <h3 className="text-lg sm:text-xl lg:text-2xl font-semibold text-white">Resultado</h3>
          <div className="flex items-center space-x-2 mt-1">
            {isComputing ? (
              <div className="w-4 h-4 sm:w-5 sm:h-5 border-2 border-red-600 border-t-transparent rounded-full animate-spin" />
            ) : routeError ? (
              <svg className="w-4 h-4 sm:w-5 sm:h-5 text-red-500" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
              </svg>
            ) : routeNodes.length > 0 ? (
              <svg className="w-4 h-4 sm:w-5 sm:h-5 text-green-500" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clipRule="evenodd" />
              </svg>
            ) : (
              <svg className="w-4 h-4 sm:w-5 sm:h-5 text-gray-400" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7-4a1 1 0 11-2 0 1 1 0 012 0zM9 9a1 1 0 000 2v3a1 1 0 001 1h1a1 1 0 100-2v-3a1 1 0 00-1-1H9z" clipRule="evenodd" />
              </svg>
            )}
            <span className={`text-xs sm:text-sm font-medium ${
              isComputing ? 'text-red-600' :
              routeError ? 'text-red-500' :
              routeNodes.length > 0 ? 'text-green-500' :
              'text-gray-400'
            }`}>
              {isComputing ? 'Calculando…' :
               routeError ? 'Error' :
               routeNodes.length > 0 ? 'Ruta' :
               'Sin cálculo'}
            </span>
            {streaming && (
              <span className="text-xs text-red-600">(Actualizaciones en vivo)</span>
            )}
          </div>
        </div>
        
        {/* No extra action buttons for concise UI */}
      </div>

      {/* Error Display */}
      {routeError && (
        <div className="mb-4 sm:mb-6 p-3 sm:p-4 bg-red-500/10 border border-red-500/20 rounded-lg">
          <div className="flex items-center space-x-2 mb-2">
            <svg className="w-4 h-4 sm:w-5 sm:h-5 text-red-500" fill="currentColor" viewBox="0 0 20 20">
              <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
            </svg>
            <span className="font-semibold text-red-500 text-sm">Error de Cálculo</span>
          </div>
          <p className="text-gray-400 text-xs sm:text-sm">{routeError}</p>
        </div>
      )}

      {/* Quick Stats */}
      <div className="grid grid-cols-2 sm:grid-cols-4 gap-3 sm:gap-4 mb-4 sm:mb-6">
        <div className="text-center">
          <div className="text-lg sm:text-xl lg:text-2xl font-bold text-red-600">
            {routeNodes.length}
          </div>
          <div className="text-xs text-gray-400">Nodos de Ruta</div>
        </div>
        
        <div className="text-center">
          <div className="text-lg sm:text-xl lg:text-2xl font-bold text-green-500">
            {routeCost > 0 ? routeCost.toFixed(1) : '—'}
          </div>
          <div className="text-xs text-gray-400">Costo Total</div>
        </div>
        
        <div className="text-center">
          <div className="text-lg sm:text-xl lg:text-2xl font-bold text-yellow-500">
            {routeExpanded}
          </div>
          <div className="text-xs text-gray-400">Nodos Expandidos</div>
        </div>
        
        <div className="text-center">
          <div className="text-lg sm:text-xl lg:text-2xl font-bold text-white">
            {formatExecutionTime(routeExecutionTime)}
          </div>
          <div className="text-xs text-gray-400">Tiempo de Ejecución</div>
        </div>
      </div>

      {/* Expandable Sections (hidden for concise UI) */}
      {false && (
      <div className="space-y-3 sm:space-y-4">
        {/* Summary Section */}
        <div className="border border-gray-600 rounded-lg">
          <button
            onClick={() => toggleSection('summary')}
            className="w-full flex items-center justify-between p-3 sm:p-4 hover:bg-gray-700 transition-colors"
          >
            <span className="font-semibold text-white text-sm sm:text-base">Resumen de Ruta</span>
            <svg 
              className={`w-4 h-4 sm:w-5 sm:h-5 text-gray-400 transition-transform flex-shrink-0 ${
                expandedSection === 'summary' ? 'rotate-180' : ''
              }`} 
              fill="currentColor" 
              viewBox="0 0 20 20"
            >
              <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
            </svg>
          </button>
          
          {expandedSection === 'summary' && routeNodes.length > 0 && (
            <div className="px-3 sm:px-4 pb-3 sm:pb-4 border-t border-gray-600">
              <div className="grid grid-cols-1 lg:grid-cols-2 gap-3 sm:gap-4 mt-3 sm:mt-4">
                <div className="space-y-2 sm:space-y-3 min-w-0">
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Algoritmo:</span>
                    <span className="font-semibold text-white text-xs sm:text-sm truncate ml-2">
                      {routeAlgorithm?.toUpperCase()}
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Métrica:</span>
                    <span className="font-semibold text-white text-xs sm:text-sm truncate ml-2">
                      {routeMetric || 'Distancia'}
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Nodo Origen:</span>
                    <span className="font-bold text-green-500 text-xs sm:text-sm truncate ml-2">
                      {routeNodes[0]}
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Nodo Destino:</span>
                    <span className="font-bold text-yellow-500 text-xs sm:text-sm truncate ml-2">
                      {routeNodes[routeNodes.length - 1]}
                    </span>
                  </div>
                </div>
                
                <div className="space-y-2 sm:space-y-3 min-w-0">
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Longitud de Ruta:</span>
                    <span className="font-semibold text-white text-xs sm:text-sm truncate ml-2">
                      {routeNodes.length} nodos
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Costo Promedio/Nodo:</span>
                    <span className="font-semibold text-white text-xs sm:text-sm truncate ml-2">
                      {efficiency.avgCostPerNode.toFixed(2)}
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Eficiencia:</span>
                    <span className="font-semibold text-red-600 text-xs sm:text-sm truncate ml-2">
                      {efficiency.efficiency.toFixed(1)}%
                    </span>
                  </div>
                  
                  <div className="flex justify-between min-w-0">
                    <span className="text-gray-400 text-xs sm:text-sm flex-shrink-0">Calidad:</span>
                    <span className={`font-semibold text-xs sm:text-sm truncate ml-2 ${
                      efficiency.pathOptimality === 'Excellent' ? 'text-green-500' :
                      efficiency.pathOptimality === 'Very Good' ? 'text-red-600' :
                      efficiency.pathOptimality === 'Good' ? 'text-yellow-500' :
                      'text-red-500'
                    }`}>
                      {efficiency.pathOptimality === 'Excellent' ? 'Excelente' :
                       efficiency.pathOptimality === 'Very Good' ? 'Muy Buena' :
                       efficiency.pathOptimality === 'Good' ? 'Buena' :
                       efficiency.pathOptimality === 'Fair' ? 'Regular' :
                       'Pobre'}
                    </span>
                  </div>
                </div>
              </div>
            </div>
          )}
        </div>

        {/* Path Details Section */}
        {routeNodes.length > 0 && (
          <div className="border border-gray-600 rounded-lg">
            <button
              onClick={() => toggleSection('path')}
              className="w-full flex items-center justify-between p-3 sm:p-4 hover:bg-gray-700 transition-colors"
            >
              <span className="font-semibold text-white text-sm sm:text-base">Detalles de Ruta</span>
              <svg 
                className={`w-4 h-4 sm:w-5 sm:h-5 text-gray-400 transition-transform flex-shrink-0 ${
                  expandedSection === 'path' ? 'rotate-180' : ''
                }`} 
                fill="currentColor" 
                viewBox="0 0 20 20"
              >
                <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
              </svg>
            </button>
            
            {expandedSection === 'path' && (
              <div className="px-3 sm:px-4 pb-3 sm:pb-4 border-t border-gray-600">
                <div className="mt-3 sm:mt-4">
                  <h5 className="font-semibold text-white mb-3 text-sm sm:text-base">Ruta Completa</h5>
                  <div className="bg-gray-700 p-2 sm:p-3 rounded-lg max-h-24 sm:max-h-32 overflow-y-auto font-mono text-xs sm:text-sm break-words">
                    {formatRouteNodes(routeNodes)}
                  </div>
                  
                  {routeNodes.length > 10 && (
                    <p className="text-xs text-gray-400 mt-2">
                      La ruta contiene {routeNodes.length} nodos en total
                    </p>
                  )}
                </div>
              </div>
            )}
          </div>
        )}

        {/* Performance Section */}
        {(routeExecutionTime > 0 || stepCount > 0) && (
          <div className="border border-gray-600 rounded-lg">
            <button
              onClick={() => toggleSection('performance')}
              className="w-full flex items-center justify-between p-3 sm:p-4 hover:bg-gray-700 transition-colors"
            >
              <span className="font-semibold text-white text-sm sm:text-base">Análisis de Rendimiento</span>
              <svg 
                className={`w-4 h-4 sm:w-5 sm:h-5 text-gray-400 transition-transform flex-shrink-0 ${
                  expandedSection === 'performance' ? 'rotate-180' : ''
                }`} 
                fill="currentColor" 
                viewBox="0 0 20 20"
              >
                <path fillRule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clipRule="evenodd" />
              </svg>
            </button>
            
            {expandedSection === 'performance' && (
              <div className="px-3 sm:px-4 pb-3 sm:pb-4 border-t border-gray-600">
                <div className="grid grid-cols-1 lg:grid-cols-2 gap-4 sm:gap-6 mt-3 sm:mt-4">
                  <div className="min-w-0">
                    <h5 className="font-semibold text-white mb-3 text-sm sm:text-base">Métricas de Ejecución</h5>
                    <div className="space-y-2 text-xs sm:text-sm">
                      <div className="flex justify-between min-w-0">
                        <span className="text-gray-400 flex-shrink-0">Tiempo de Ejecución:</span>
                        <span className="font-mono text-red-600 truncate ml-2">
                          {formatExecutionTime(routeExecutionTime)}
                        </span>
                      </div>
                      
                      <div className="flex justify-between min-w-0">
                        <span className="text-gray-400 flex-shrink-0">Pasos de Línea de Tiempo:</span>
                        <span className="font-mono text-green-500 truncate ml-2">{stepCount}</span>
                      </div>
                      
                      <div className="flex justify-between min-w-0">
                        <span className="text-gray-400 flex-shrink-0">Ratio de Exploración:</span>
                        <span className="font-mono text-yellow-500 truncate ml-2">
                          {efficiency.explorationRatio.toFixed(2)}x
                        </span>
                      </div>
                    </div>
                  </div>
                  
                  <div className="min-w-0">
                    <h5 className="font-semibold text-white mb-3 text-sm sm:text-base">Eficiencia del Algoritmo</h5>
                    <div className="space-y-3">
                      <div>
                        <div className="flex justify-between text-xs sm:text-sm mb-1 min-w-0">
                          <span className="text-gray-400 flex-shrink-0">Eficiencia de Búsqueda</span>
                          <span className="font-semibold text-red-600 truncate ml-2">
                            {efficiency.efficiency.toFixed(1)}%
                          </span>
                        </div>
                        <div className="w-full bg-gray-700 rounded-full h-1.5 sm:h-2">
                          <div 
                            className="bg-red-600 h-1.5 sm:h-2 rounded-full transition-all duration-500"
                            style={{ width: `${Math.min(efficiency.efficiency, 100)}%` }}
                          />
                        </div>
                      </div>
                      
                      <div className="text-xs text-gray-400 break-words">
                        {efficiency.efficiency > 80 ? 
                          'Excelente - Ruta muy directa encontrada' :
                          efficiency.efficiency > 60 ?
                          'Muy Buena - Búsqueda de ruta eficiente' :
                          efficiency.efficiency > 40 ?
                          'Buena - Exploración razonable' :
                          efficiency.efficiency > 20 ?
                          'Regular - Se detectó cierta ineficiencia' :
                          'Pobre - Alto overhead de exploración'
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
      )}

      {/* Empty State */}
      {routeNodes.length === 0 && !isComputing && !routeError && (
        <div className="text-center py-6 sm:py-8">
          <div className="w-12 h-12 sm:w-16 sm:h-16 bg-gray-700 rounded-full flex items-center justify-center mx-auto mb-3 sm:mb-4">
            <svg className="w-6 h-6 sm:w-8 sm:h-8 text-gray-400" fill="currentColor" viewBox="0 0 20 20">
              <path d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
          </div>
          <h4 className="text-lg sm:text-xl font-semibold text-white mb-2">Sin Resultados</h4>
          <p className="text-xs sm:text-sm text-gray-400">
            Configura los parámetros de ruta y haz clic en "Calcular Ruta" para ver resultados
          </p>
        </div>
      )}
    </div>
  );
};

export default ResultsPanel;
