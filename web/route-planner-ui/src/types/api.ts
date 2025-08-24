/**
 * API Type Definitions for Route Planning Interface
 * Stable data contracts with the C++ backend
 */

// Core coordinate type
export interface Coordinates {
  longitude: number;
  latitude: number;
}

// Node coordinates resolution
export interface NodeCoordinatesResult {
  coords: Record<string, [number, number]>; // nodeId -> [lon, lat]
}

// Graph data structure
export interface GraphLoadResult {
  nodes: number;
  edges: number;
  directed: boolean;
  bounds: {
    minLon: number;
    maxLon: number;
    minLat: number;
    maxLat: number;
  };
}

// Route calculation parameters
export interface RouteParams {
  source: number;            // Must be positive integer
  target: number;            // Must be positive integer
  algo: 'astar' | 'dijkstra';
  metric: 'time' | 'distance' | 'cost';
  debug?: boolean;           // Include timeline steps
}

// Route calculation result
export interface RouteResult {
  cost: number;
  nodes: number[];           // Path as node IDs
  edges: number[];           // Edge IDs in path
  expanded: number;          // Nodes expanded during search
  timeline?: ExpansionStep[]; // Only if debug=true
  execution_time: number;    // Milliseconds
}

// Algorithm timeline step
export interface ExpansionStep {
  stepId: number;
  currentNode: number;
  openSet: number[];
  closedSet: number[];
  gScore: number;
  fScore?: number;           // Only for A*
  timestamp: number;
}

// Performance metrics
export interface PerformanceMetrics {
  p50: number;               // 50th percentile response time
  p95: number;               // 95th percentile response time
  memoryMB: number;
  cpuPercent: number;
  requestsPerSecond: number;
}

// System status
export interface SystemStatus {
  status: 'online' | 'offline' | 'computing';
  version: string;
  connections: number;
  uptime: number;
}

// Graph creation parameters
export interface GraphCreateParams {
  nodes: number;
  edges: number;
  directed: boolean;
}

// Simulation parameters
export interface SimulationParams {
  duration: number;          // Duration in seconds
  events?: string[];         // Event types to simulate
}

// Simulation status
export interface SimulationStatus {
  running: boolean;
  duration: number;
  elapsed: number;
  events: number;
}

// API Error response
export interface ApiError {
  error: string;
  code: number;
  message: string;
  timestamp: number;
}

// Standard API response wrapper
export interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: ApiError;
  timestamp: number;
}

// SSE Event types
export interface SSEExpansionEvent {
  type: 'expansion';
  data: ExpansionStep;
}

export interface SSECompleteEvent {
  type: 'complete';
  data: {
    totalSteps: number;
    finalCost: number;
  };
}

export interface SSEErrorEvent {
  type: 'error';
  data: {
    message: string;
    code: number;
  };
}

export type SSEEvent = SSEExpansionEvent | SSECompleteEvent | SSEErrorEvent;

// WebSocket message types
export interface WSSimulationUpdate {
  type: 'simulation_update';
  data: {
    elapsed: number;
    events: number;
    performance: Partial<PerformanceMetrics>;
  };
}

export interface WSStatusUpdate {
  type: 'status_update';
  data: SystemStatus;
}

export type WSMessage = WSSimulationUpdate | WSStatusUpdate;