/**
 * API Service for Route Planning Interface
 * Unified service layer with standardized /api endpoints
 */

import {
  RouteParams,
  RouteResult,
  GraphLoadResult,
  GraphCreateParams,
  NodeCoordinatesResult,
  SystemStatus,
  PerformanceMetrics,
  SimulationParams,
  SimulationStatus,
  ApiResponse,
  ApiError,
} from '../types/api';

// Configuration
const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080/api';
const REQUEST_TIMEOUT = 30000; // 30 seconds

class ApiService {
  private baseUrl: string;
  private abortController: AbortController | null = null;

  constructor(baseUrl: string = API_BASE_URL) {
    this.baseUrl = baseUrl;
  }

  /**
   * Generic fetch wrapper with error handling and timeout
   */
  private async fetchWithTimeout<T>(
    endpoint: string,
    options: RequestInit = {},
    timeout: number = REQUEST_TIMEOUT
  ): Promise<T> {
    // Cancel previous request if exists
    if (this.abortController) {
      this.abortController.abort();
    }

    this.abortController = new AbortController();
    const timeoutId = setTimeout(() => this.abortController?.abort(), timeout);

    try {
      const response = await fetch(`${this.baseUrl}${endpoint}`, {
        ...options,
        signal: this.abortController.signal,
        headers: {
          'Content-Type': 'application/json',
          ...options.headers,
        },
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        const errorData = await response.json().catch(() => ({}));
        throw new Error(
          errorData.message || `HTTP ${response.status}: ${response.statusText}`
        );
      }

      return await response.json();
    } catch (error) {
      clearTimeout(timeoutId);
      
      if (error instanceof Error) {
        if (error.name === 'AbortError') {
          throw new Error('Request was cancelled or timed out');
        }
        throw error;
      }
      
      throw new Error('Unknown error occurred');
    }
  }

  /**
   * Input validation helpers
   */
  private validateRouteParams(params: RouteParams): void {
    if (!Number.isInteger(params.source) || params.source <= 0) {
      throw new Error('Source must be a positive integer');
    }
    if (!Number.isInteger(params.target) || params.target <= 0) {
      throw new Error('Target must be a positive integer');
    }
    if (!['astar', 'dijkstra'].includes(params.algo)) {
      throw new Error('Algorithm must be either \"astar\" or \"dijkstra\"');
    }
    if (!['time', 'distance', 'cost'].includes(params.metric)) {
      throw new Error('Metric must be one of: time, distance, cost');
    }
  }

  private validateNodeIds(nodeIds: number[]): void {
    if (nodeIds.length === 0) {
      throw new Error('Node IDs array cannot be empty');
    }
    if (nodeIds.length > 1000) {
      throw new Error('Too many node IDs requested (max 1000)');
    }
    if (!nodeIds.every(id => Number.isInteger(id) && id > 0)) {
      throw new Error('All node IDs must be positive integers');
    }
  }

  // ============================================================================
  // GRAPH OPERATIONS
  // ============================================================================

  /**
   * Load graph with specified parameters
   */
  async loadGraph(params: GraphCreateParams): Promise<GraphLoadResult> {
    if (params.nodes <= 0 || params.edges < 0) {
      throw new Error('Invalid graph parameters');
    }

    return this.fetchWithTimeout<GraphLoadResult>('/graph/load', {
      method: 'POST',
      body: JSON.stringify(params),
    });
  }

  /**
   * Get node coordinates for specified node IDs
   */
  async getNodeCoordinates(nodeIds: number[]): Promise<NodeCoordinatesResult> {
    this.validateNodeIds(nodeIds);
    
    const idsParam = nodeIds.join(',');
    return this.fetchWithTimeout<NodeCoordinatesResult>(
      `/nodes/coords?ids=${idsParam}`
    );
  }

  // ============================================================================
  // ROUTE OPERATIONS
  // ============================================================================

  /**
   * Calculate route between source and target
   */
  async calculateRoute(params: RouteParams): Promise<RouteResult> {
    this.validateRouteParams(params);

    return this.fetchWithTimeout<RouteResult>('/route', {
      method: 'POST',
      body: JSON.stringify(params),
    });
  }

  /**
   * Cancel ongoing route calculation
   */
  cancelRoute(): void {
    if (this.abortController) {
      this.abortController.abort();
      this.abortController = null;
    }
  }

  // ============================================================================
  // SIMULATION OPERATIONS
  // ============================================================================

  /**
   * Start traffic simulation
   */
  async startSimulation(params: SimulationParams): Promise<SimulationStatus> {
    if (params.duration <= 0) {
      throw new Error('Simulation duration must be positive');
    }

    return this.fetchWithTimeout<SimulationStatus>('/simulate/start', {
      method: 'POST',
      body: JSON.stringify(params),
    });
  }

  /**
   * Stop traffic simulation
   */
  async stopSimulation(): Promise<SimulationStatus> {
    return this.fetchWithTimeout<SimulationStatus>('/simulate/stop', {
      method: 'POST',
    });
  }

  /**
   * Get simulation status
   */
  async getSimulationStatus(): Promise<SimulationStatus> {
    return this.fetchWithTimeout<SimulationStatus>('/simulate/status');
  }

  // ============================================================================
  // SYSTEM OPERATIONS
  // ============================================================================

  /**
   * Get system status
   */
  async getSystemStatus(): Promise<SystemStatus> {
    return this.fetchWithTimeout<SystemStatus>('/status');
  }

  /**
   * Get performance metrics
   */
  async getMetrics(): Promise<PerformanceMetrics> {
    return this.fetchWithTimeout<PerformanceMetrics>('/metrics');
  }

  // ============================================================================
  // STREAMING OPERATIONS
  // ============================================================================

  /**
   * Create Server-Sent Events connection for route streaming
   */
  createRouteStream(params: RouteParams): EventSource {
    this.validateRouteParams(params);
    
    const searchParams = new URLSearchParams({
      source: params.source.toString(),
      target: params.target.toString(),
      algo: params.algo,
      metric: params.metric,
      debug: 'true',
    });

    const url = `${this.baseUrl}/stream/route?${searchParams.toString()}`;
    return new EventSource(url);
  }

  /**
   * Create WebSocket connection for simulation events
   */
  createSimulationWebSocket(): WebSocket {
    const wsUrl = this.baseUrl.replace(/^http/, 'ws') + '/ws/sim';
    return new WebSocket(wsUrl);
  }

  // ============================================================================
  // UTILITY METHODS
  // ============================================================================

  /**
   * Health check endpoint
   */
  async healthCheck(): Promise<boolean> {
    try {
      await this.fetchWithTimeout('/health', {}, 5000);
      return true;
    } catch {
      return false;
    }
  }

  /**
   * Get API base URL
   */
  getBaseUrl(): string {
    return this.baseUrl;
  }

  /**
   * Update API base URL
   */
  setBaseUrl(url: string): void {
    this.baseUrl = url;
  }
}

// Create singleton instance
export const apiService = new ApiService();

// Export default
export default apiService;

// Export class for testing
export { ApiService };