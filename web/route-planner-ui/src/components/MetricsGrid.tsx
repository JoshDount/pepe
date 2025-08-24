/**
 * Metrics Grid Component
 * Performance metrics and system analytics display
 */

import { useEffect, FC } from 'react';
import { useRouteSimulatorStore, useMetricsState } from '../store';
import { styleTokens } from '../styles/tokens';

const MetricsGrid: FC = () => {
  const { updateMetrics } = useRouteSimulatorStore();
  const { p50, p95, memoryMB, cpuPercent, requestsPerSecond, lastUpdated } = useMetricsState();

  // Update metrics periodically
  useEffect(() => {
    updateMetrics();
    const interval = setInterval(updateMetrics, 5000);
    return () => clearInterval(interval);
  }, [updateMetrics]);

  const formatValue = (value: number, unit: string, decimals: number = 1): string => {
    if (value === 0) return '—';
    return `${value.toFixed(decimals)}${unit}`;
  };

  const formatTime = (ms: number): string => {
    if (ms === 0) return '—';
    if (ms < 1) return `${(ms * 1000).toFixed(0)}μs`;
    if (ms < 1000) return `${ms.toFixed(1)}ms`;
    return `${(ms / 1000).toFixed(2)}s`;
  };

  const getPerformanceLevel = (value: number, thresholds: [number, number]): 'good' | 'warning' | 'critical' => {
    if (value <= thresholds[0]) return 'good';
    if (value <= thresholds[1]) return 'warning';
    return 'critical';
  };

  const getPerformanceColor = (level: 'good' | 'warning' | 'critical'): string => {
    switch (level) {
      case 'good':
        return 'text-accent-success';
      case 'warning':
        return 'text-accent-warning';
      case 'critical':
        return 'text-accent-error';
    }
  };

  const metrics = [
    {
      id: 'p50',
      title: 'Response Time (P50)',
      value: formatTime(p50),
      rawValue: p50,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm1-13a1 1 0 10-2 0v4a1 1 0 00.293.707l2.828 2.829a1 1 0 101.415-1.415L11 9.586V5z" clipRule="evenodd" />
        </svg>
      ),
      description: 'Median response time',
      level: getPerformanceLevel(p50, [20, 50]),
    },
    {
      id: 'p95',
      title: 'Response Time (P95)',
      value: formatTime(p95),
      rawValue: p95,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm1-13a1 1 0 10-2 0v4a1 1 0 00.293.707l2.828 2.829a1 1 0 101.415-1.415L11 9.586V5z" clipRule="evenodd" />
        </svg>
      ),
      description: '95th percentile response time',
      level: getPerformanceLevel(p95, [50, 100]),
    },
    {
      id: 'memory',
      title: 'Memory Usage',
      value: formatValue(memoryMB, 'MB'),
      rawValue: memoryMB,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4zM3 10a1 1 0 011-1h6a1 1 0 011 1v6a1 1 0 01-1 1H4a1 1 0 01-1-1v-6zM14 9a1 1 0 00-1 1v6a1 1 0 001 1h2a1 1 0 001-1v-6a1 1 0 00-1-1h-2z" />
        </svg>
      ),
      description: 'Current memory consumption',
      level: getPerformanceLevel(memoryMB, [150, 250]),
    },
    {
      id: 'cpu',
      title: 'CPU Usage',
      value: formatValue(cpuPercent, '%'),
      rawValue: cpuPercent,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M3 3a1 1 0 000 2v8a2 2 0 002 2h2.586l-1.293 1.293a1 1 0 101.414 1.414L10 15.414l2.293 2.293a1 1 0 001.414-1.414L12.414 15H15a2 2 0 002-2V5a1 1 0 100-2H3zm11.707 4.707a1 1 0 00-1.414-1.414L10 9.586 8.707 8.293a1 1 0 00-1.414 0l-2 2a1 1 0 101.414 1.414L8 10.414l1.293 1.293a1 1 0 001.414 0l4-4z" clipRule="evenodd" />
        </svg>
      ),
      description: 'Current CPU utilization',
      level: getPerformanceLevel(cpuPercent, [50, 80]),
    },
    {
      id: 'rps',
      title: 'Requests/Second',
      value: formatValue(requestsPerSecond, '/s'),
      rawValue: requestsPerSecond,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M6 2a1 1 0 00-1 1v1H4a2 2 0 00-2 2v10a2 2 0 002 2h12a2 2 0 002-2V6a2 2 0 00-2-2h-1V3a1 1 0 10-2 0v1H7V3a1 1 0 00-1-1zm0 5a1 1 0 000 2h8a1 1 0 100-2H6z" clipRule="evenodd" />
        </svg>
      ),
      description: 'Current request throughput',
      level: 'good' as const, // RPS is generally good unless very high
    },
    {
      id: 'uptime',
      title: 'System Uptime',
      value: lastUpdated ? new Date(lastUpdated).toLocaleTimeString() : '—',
      rawValue: lastUpdated,
      icon: (
        <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zM4.332 8.027a6.012 6.012 0 011.912-2.706C6.512 5.73 6.974 6 7.5 6A1.5 1.5 0 019 7.5V8a2 2 0 004 0 2 2 0 011.523-1.943A5.977 5.977 0 0116 10c0 .34-.028.675-.083 1H15a2 2 0 00-2 2v2.197A5.973 5.973 0 0110 16v-2a2 2 0 00-2-2 2 2 0 01-2-2 2 2 0 00-1.668-1.973z" clipRule="evenodd" />
        </svg>
      ),
      description: 'Last updated',
      level: 'good' as const,
    },
  ];

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="text-center mb-8">
        <h2 className={`${styleTokens.text.heading} mb-2`}>Performance Metrics</h2>
        <p className={styleTokens.text.caption}>
          Real-time system performance and algorithm analytics
        </p>
      </div>

      {/* Metrics Grid */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        {metrics.map((metric) => (
          <div key={metric.id} className={`${styleTokens.card} hover:shadow-elevated transition-all duration-200`}>
            <div className="flex items-start justify-between mb-4">
              <div className={`p-3 rounded-xl ${
                metric.level === 'good' ? 'bg-accent-success/20 text-accent-success' :
                metric.level === 'warning' ? 'bg-accent-warning/20 text-accent-warning' :
                'bg-accent-error/20 text-accent-error'
              }`}>
                {metric.icon}
              </div>

              {/* Status Indicator */}
              <div className={`w-2 h-2 rounded-full ${
                metric.level === 'good' ? 'bg-accent-success' :
                metric.level === 'warning' ? 'bg-accent-warning' :
                'bg-accent-error'
              }`} />
            </div>

            <div>
              <h3 className="font-semibold text-text-primary mb-1">{metric.title}</h3>
              <div className={`text-2xl font-bold mb-2 ${getPerformanceColor(metric.level)}`}>
                {metric.value}
              </div>
              <p className="text-sm text-text-secondary">{metric.description}</p>
            </div>

            {/* Progress Bar for percentage metrics */}
            {(metric.id === 'cpu' || metric.id === 'memory') && metric.rawValue > 0 && (
              <div className="mt-4">
                <div className="w-full bg-bg-elevated rounded-full h-2">
                  <div
                    className={`h-2 rounded-full transition-all duration-500 ${
                      metric.level === 'good' ? 'bg-accent-success' :
                      metric.level === 'warning' ? 'bg-accent-warning' :
                      'bg-accent-error'
                    }`}
                    style={{ 
                      width: `${Math.min(
                        metric.id === 'cpu' ? metric.rawValue : (metric.rawValue / 500) * 100, 
                        100
                      )}%` 
                    }}
                  />
                </div>
              </div>
            )}
          </div>
        ))}
      </div>

      {/* Summary Stats */}
      <div className={`${styleTokens.card} bg-gradient-to-r from-bg-surface to-bg-elevated`}>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <div className="text-center">
            <div className="text-2xl font-bold text-accent-success mb-1">
              {getPerformanceLevel(p50, [20, 50]) === 'good' ? 'Excellent' : 
               getPerformanceLevel(p50, [20, 50]) === 'warning' ? 'Good' : 'Poor'}
            </div>
            <div className="text-sm text-text-secondary">Overall Performance</div>
          </div>

          <div className="text-center">
            <div className="text-2xl font-bold text-accent-primary mb-1">
              {requestsPerSecond > 0 ? `${requestsPerSecond.toFixed(1)}/s` : '0/s'}
            </div>
            <div className="text-sm text-text-secondary">Current Throughput</div>
          </div>

          <div className="text-center">
            <div className="text-2xl font-bold text-accent-warning mb-1">
              {lastUpdated ? Math.floor((Date.now() - lastUpdated) / 1000) : 0}s
            </div>
            <div className="text-sm text-text-secondary">Last Update</div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default MetricsGrid;