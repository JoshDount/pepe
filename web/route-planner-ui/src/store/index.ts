/**
 * Zustand Store for Route Planning Interface
 * Centralized state management for the application
 */

import { create } from 'zustand';
import type { 
  RouteParams, 
  RouteResult, 
  ExpansionStep, 
  SystemStatus, 
  PerformanceMetrics,
  GraphCreateParams 
} from '../types/api';

// State interfaces
interface GraphState {
  loaded: boolean;
  size: number;
  bounds: {
    minLon: number;
    maxLon: number;
    minLat: number;
    maxLat: number;
  };
}

interface RouteState {
  computing: boolean;
  nodes: number[];
  edges: number[];
  cost: number;
  expanded: number;
  algorithm: 'astar' | 'dijkstra';
  metric: 'time' | 'distance' | 'cost';
  executionTime: number;
  error: string | null;
}

interface TimelineState {
  streaming: boolean;
  steps: ExpansionStep[];
  stepCount: number;
  currentStep: number;
  maxSteps: number;
}

interface MetricsState {
  p50: number;
  p95: number;
  memoryMB: number;
  cpuPercent: number;
  requestsPerSecond: number;
  lastUpdated: number;
}

interface SystemState {
  status: 'online' | 'offline' | 'computing';
  version: string;
  connections: number;
  uptime: number;
  lastChecked: number;
}

// Combined store interface
interface RouteSimulatorStore {
  // State
  graph: GraphState;
  route: RouteState;
  timeline: TimelineState;
  metrics: MetricsState;
  system: SystemState;
  coordinates: Record<string, [number, number]>;
  
  // Actions
  loadGraph: (params: GraphCreateParams) => Promise<void>;
  calculateRoute: (params: RouteParams) => Promise<void>;
  clearRoute: () => void;
  addExpansionStep: (step: ExpansionStep) => void;
  clearTimeline: () => void;
  startStreaming: () => void;
  stopStreaming: () => void;
  getNodeCoordinates: (nodeIds: number[]) => Promise<void>;
  updateSystemStatus: () => Promise<void>;
  updateMetrics: () => Promise<void>;
}

// Default state values
const defaultGraphState: GraphState = {
  loaded: true, // Set to true for demo
  size: 100,
  bounds: {
    minLon: -74.1,
    maxLon: -73.9,
    minLat: 40.6,
    maxLat: 40.8,
  },
};

const defaultRouteState: RouteState = {
  computing: false,
  nodes: [],
  edges: [],
  cost: 0,
  expanded: 0,
  algorithm: 'dijkstra',
  metric: 'distance',
  executionTime: 0,
  error: null,
};

const defaultTimelineState: TimelineState = {
  streaming: false,
  steps: [],
  stepCount: 0,
  currentStep: 0,
  maxSteps: 5000,
};

const defaultMetricsState: MetricsState = {
  p50: 0,
  p95: 0,
  memoryMB: 0,
  cpuPercent: 0,
  requestsPerSecond: 0,
  lastUpdated: 0,
};

const defaultSystemState: SystemState = {
  status: 'online',
  version: '1.0.0',
  connections: 0,
  uptime: 0,
  lastChecked: 0,
};

// Create the main store
export const useRouteSimulatorStore = create<RouteSimulatorStore>()(
    (set, get) => ({
      // Initial state
      graph: defaultGraphState,
      route: defaultRouteState,
      timeline: defaultTimelineState,
      metrics: defaultMetricsState,
      system: defaultSystemState,
      coordinates: {},

      // Graph actions
      loadGraph: async (params: GraphCreateParams) => {
        try {
          // Simulate loading
          set((state) => ({
            graph: {
              ...state.graph,
              loaded: true,
              size: params.nodes,
            }
          }));
          console.log('Graph loaded:', params);
        } catch (error) {
          console.error('Failed to load graph:', error);
        }
      },

      // Route actions
      calculateRoute: async (params: RouteParams) => {
        try {
          set((state) => ({
            route: {
              ...state.route,
              computing: true,
              algorithm: params.algo,
              metric: params.metric,
              error: null,
            }
          }));

          // Simulate route calculation
          setTimeout(() => {
            const mockNodes = [params.source, 5, 12, 23, params.target];
            set((state) => ({
              route: {
                ...state.route,
                computing: false,
                nodes: mockNodes,
                cost: Math.random() * 100 + 50,
                expanded: Math.floor(Math.random() * 20 + 10),
                executionTime: Math.random() * 100 + 10,
              }
            }));
          }, 2000);

          console.log('Route calculation started:', params);
        } catch (error) {
          set((state) => ({
            route: {
              ...state.route,
              computing: false,
              error: error instanceof Error ? error.message : 'Unknown error',
            }
          }));
        }
      },

      clearRoute: () => {
        set((state) => ({
          route: defaultRouteState,
          timeline: defaultTimelineState,
        }));
      },

      // Timeline actions
      addExpansionStep: (step: ExpansionStep) => {
        set((state) => ({
          timeline: {
            ...state.timeline,
            steps: [...state.timeline.steps, step],
            stepCount: state.timeline.stepCount + 1,
            currentStep: step.stepId,
          }
        }));
      },

      clearTimeline: () => {
        set((state) => ({
          timeline: defaultTimelineState,
        }));
      },

      startStreaming: () => {
        set((state) => ({
          timeline: {
            ...state.timeline,
            streaming: true,
          }
        }));
      },

      stopStreaming: () => {
        set((state) => ({
          timeline: {
            ...state.timeline,
            streaming: false,
          }
        }));
      },

      // Coordinate actions
      getNodeCoordinates: async (nodeIds: number[]) => {
        try {
          // Simulate coordinate fetching
          const coords: Record<string, [number, number]> = {};
          nodeIds.forEach(id => {
            coords[id.toString()] = [
              -74.0 + Math.random() * 0.1,
              40.7 + Math.random() * 0.1
            ];
          });
          
          set((state) => ({
            coordinates: { ...state.coordinates, ...coords }
          }));
        } catch (error) {
          console.error('Failed to get coordinates:', error);
        }
      },

      // System actions
      updateSystemStatus: async () => {
        try {
          set((state) => ({
            system: {
              ...state.system,
              lastChecked: Date.now(),
              uptime: Date.now() - 1000000, // Mock uptime
            }
          }));
        } catch (error) {
          console.error('Failed to update system status:', error);
        }
      },

      updateMetrics: async () => {
        try {
          set((state) => ({
            metrics: {
              p50: Math.random() * 50 + 10,
              p95: Math.random() * 100 + 50,
              memoryMB: Math.random() * 100 + 100,
              cpuPercent: Math.random() * 30 + 10,
              requestsPerSecond: Math.random() * 20 + 5,
              lastUpdated: Date.now(),
            }
          }));
        } catch (error) {
          console.error('Failed to update metrics:', error);
        }
      },
    })
);

// Selector hooks for components
export const useGraphState = () => useRouteSimulatorStore((state) => state.graph);
export const useRouteState = () => useRouteSimulatorStore((state) => state.route);
export const useTimelineState = () => useRouteSimulatorStore((state) => state.timeline);
export const useMetricsState = () => useRouteSimulatorStore((state) => state.metrics);
export const useSystemState = () => useRouteSimulatorStore((state) => state.system);

// Computed selectors
export const useCurrentRoute = () => useRouteSimulatorStore((state) => ({
  nodes: state.route.nodes,
  cost: state.route.cost,
  algorithm: state.route.algorithm,
  executionTime: state.route.executionTime,
  expanded: state.route.expanded,
  error: state.route.error,
  metric: state.route.metric,
}));

export const useIsComputing = () => useRouteSimulatorStore((state) => state.route.computing);

// Action selectors
export const useRouteActions = () => useRouteSimulatorStore((state) => ({
  loadGraph: state.loadGraph,
  calculateRoute: state.calculateRoute,
  clearRoute: state.clearRoute,
  updateSystemStatus: state.updateSystemStatus,
  getNodeCoordinates: state.getNodeCoordinates,
}));

export const useTimelineActions = () => useRouteSimulatorStore((state) => ({
  addExpansionStep: state.addExpansionStep,
  clearTimeline: state.clearTimeline,
  startStreaming: state.startStreaming,
  stopStreaming: state.stopStreaming,
}));

// Coordinate selectors
export const useRouteCoordinates = () => useRouteSimulatorStore((state) => state.coordinates);