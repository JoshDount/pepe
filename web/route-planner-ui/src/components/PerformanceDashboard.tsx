/**
 * Performance Dashboard Component
 * Compact performance metrics in a separate tab
 */

import { FC, useEffect } from 'react';
import { useMetricsState, useSystemState, useUpdateMetrics, useUpdateSystemStatus } from '../store';

const PerformanceDashboard: FC = () => {
  const { p50, p95, memoryMB, cpuPercent, requestsPerSecond, lastUpdated } = useMetricsState();
  const { status, connections, uptime } = useSystemState();
  const updateMetrics = useUpdateMetrics();
  const updateSystemStatus = useUpdateSystemStatus();

  // Update metrics every 3 seconds
  useEffect(() => {
    const interval = setInterval(() => {
      updateMetrics();
      updateSystemStatus();
    }, 3000);

    // Initial update
    updateMetrics();
    updateSystemStatus();

    return () => clearInterval(interval);
  }, [updateMetrics, updateSystemStatus]);

  const formatTime = (timestamp: number) => {
    return new Date(timestamp).toLocaleTimeString();
  };

  const formatUptime = (milliseconds: number) => {
    const hours = Math.floor(milliseconds / (1000 * 60 * 60));
    const minutes = Math.floor((milliseconds % (1000 * 60 * 60)) / (1000 * 60));
    const seconds = Math.floor((milliseconds % (1000 * 60)) / 1000);
    
    if (hours > 0) {
      return `${hours}h ${minutes}m`;
    } else if (minutes > 0) {
      return `${minutes}m ${seconds}s`;
    } else {
      return `${seconds}s`;
    }
  };

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'online': return 'text-green-500';
      case 'computing': return 'text-yellow-500';
      case 'offline': return 'text-red-500';
      default: return 'text-gray-400';
    }
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="text-center mb-8">
        <h2 className="text-3xl sm:text-4xl font-extrabold text-white mb-3">
          Dashboard de Rendimiento
        </h2>
        <p className="text-sm text-gray-400">
          Métricas del sistema en tiempo real
        </p>
      </div>

      {/* System Status */}
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
        <h3 className="text-lg font-semibold text-white mb-4">Estado del Sistema</h3>
        <div className="grid grid-cols-1 sm:grid-cols-4 gap-4">
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Estado</div>
            <div className="flex items-center">
              <div className={`w-2 h-2 rounded-full mr-2 ${
                status === 'online' ? 'bg-green-500' : 
                status === 'computing' ? 'bg-yellow-500' : 'bg-red-500'
              }`} />
              <span className={`text-sm font-medium ${getStatusColor(status)}`}>
                {status === 'online' ? 'En línea' : 
                 status === 'computing' ? 'Calculando' : 'Desconectado'}
              </span>
            </div>
          </div>
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Conexiones</div>
            <div className="text-lg font-bold text-white">{connections}</div>
          </div>
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Tiempo activo</div>
            <div className="text-sm text-white">{formatUptime(uptime)}</div>
          </div>
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Última actualización</div>
            <div className="text-sm text-white">{formatTime(lastUpdated)}</div>
          </div>
        </div>
      </div>

      {/* Performance Metrics */}
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
        <h3 className="text-lg font-semibold text-white mb-4">Métricas de Rendimiento</h3>
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4">
          {/* Latency */}
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Latencia P50</div>
            <div className="text-2xl font-bold text-white">{p50.toFixed(1)}ms</div>
            <div className="text-xs text-gray-500">percentil 50</div>
          </div>
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Latencia P95</div>
            <div className="text-2xl font-bold text-white">{p95.toFixed(1)}ms</div>
            <div className="text-xs text-gray-500">percentil 95</div>
          </div>
          
          {/* Memory */}
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Memoria</div>
            <div className="text-2xl font-bold text-white">{memoryMB.toFixed(0)}MB</div>
            <div className="text-xs text-gray-500">en uso</div>
          </div>
          
          {/* CPU */}
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">CPU</div>
            <div className="text-2xl font-bold text-white">{cpuPercent.toFixed(1)}%</div>
            <div className="text-xs text-gray-500">utilización</div>
          </div>
          
          {/* Throughput */}
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Rendimiento</div>
            <div className="text-2xl font-bold text-white">{requestsPerSecond.toFixed(1)}</div>
            <div className="text-xs text-gray-500">req/s</div>
          </div>
          
          {/* Health Score */}
          <div className="bg-gray-700/50 rounded-lg p-4">
            <div className="text-xs text-gray-400 mb-1">Salud del Sistema</div>
            <div className="text-2xl font-bold text-green-500">
              {Math.round(100 - (cpuPercent + memoryMB / 10) / 2)}%
            </div>
            <div className="text-xs text-gray-500">puntuación</div>
          </div>
        </div>
      </div>

      {/* Performance Chart Placeholder */}
      <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
        <h3 className="text-lg font-semibold text-white mb-4">Tendencias de Rendimiento</h3>
        <div className="h-48 bg-gray-700/30 rounded-lg flex items-center justify-center">
          <div className="text-center">
            <svg className="w-12 h-12 text-gray-500 mx-auto mb-2" fill="currentColor" viewBox="0 0 20 20">
              <path d="M2 11a1 1 0 011-1h2a1 1 0 011 1v5a1 1 0 01-1 1H3a1 1 0 01-1-1v-5zM8 7a1 1 0 011-1h2a1 1 0 011 1v9a1 1 0 01-1 1H9a1 1 0 01-1-1V7zM14 4a1 1 0 011-1h2a1 1 0 011 1v12a1 1 0 01-1 1h-2a1 1 0 01-1-1V4z" />
            </svg>
            <p className="text-sm text-gray-400">Gráfico de rendimiento</p>
            <p className="text-xs text-gray-500">Próximamente</p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default PerformanceDashboard;
