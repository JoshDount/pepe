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

export interface GraphNode { id: number; x: number; y: number }
export interface GraphEdge { from: number; to: number; weight: number }

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
  relaxations: number;
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

interface GraphMetrics {
  nodes: number;
  edges: number; // undirected unique edges count
  avgDegree: number;
  density: number; // 0..1 for undirected graph
  avgWeight: number;
  minWeight: number;
  maxWeight: number;
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
  graphNodes: GraphNode[];
  graphEdges: GraphEdge[];
  graphK: number;
  route: RouteState;
  timeline: TimelineState;
  metrics: MetricsState;
  system: SystemState;
  coordinates: Record<string, [number, number]>;
  graphMetrics: GraphMetrics;
  mapHeight: number; // px
  
  // Actions
  loadGraph: (params: GraphCreateParams) => Promise<void>;
  regenerateGraph: (nodes: number) => Promise<void>;
  calculateRoute: (params: RouteParams) => Promise<void>;
  clearRoute: () => void;
  buildAdjacency: () => Map<number, GraphEdge[]>;
  addExpansionStep: (step: ExpansionStep) => void;
  clearTimeline: () => void;
  startStreaming: () => void;
  stopStreaming: () => void;
  getNodeCoordinates: (nodeIds: number[]) => Promise<void>;
  updateSystemStatus: () => Promise<void>;
  updateMetrics: () => Promise<void>;
  setGraphK: (k: number) => void;
  setMapHeight: (h: number) => void;
}

// Default state values
const defaultGraphState: GraphState = {
  loaded: false,
  size: 0,
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
  relaxations: 0,
};

const defaultTimelineState: TimelineState = {
  streaming: false,
  steps: [],
  stepCount: 0,
  currentStep: 0,
  maxSteps: 5000,
};

const defaultMetricsState: MetricsState = {
  p50: 45.2,
  p95: 120.8,
  memoryMB: 128.5,
  cpuPercent: 23.4,
  requestsPerSecond: 12.7,
  lastUpdated: Date.now(),
};

const defaultSystemState: SystemState = {
  status: 'online',
  version: '1.0.0',
  connections: 3,
  uptime: 3600000, // 1 hour in milliseconds
  lastChecked: Date.now(),
};

const defaultGraphMetrics: GraphMetrics = {
  nodes: 0,
  edges: 0,
  avgDegree: 0,
  density: 0,
  avgWeight: 0,
  minWeight: 0,
  maxWeight: 0,
};

// Create the main store
export const useRouteSimulatorStore = create<RouteSimulatorStore>()(
  (set, get) => ({
    // Initial state
    graph: defaultGraphState,
    graphNodes: [],
    graphEdges: [],
    graphK: 6,
    route: defaultRouteState,
    timeline: defaultTimelineState,
    metrics: defaultMetricsState,
    system: defaultSystemState,
    coordinates: {},
    graphMetrics: defaultGraphMetrics,
    mapHeight: 780,

    // Graph actions
    loadGraph: async (params: GraphCreateParams) => {
      try {
        // Generate demo graph with k-nearest edges
        const n = Math.max(50, Math.min(300, params.nodes || 80));
        const width = 900, height = 500, margin = 50;
        const nodes: GraphNode[] = Array.from({ length: n }, (_, i) => ({
          id: i + 1,
          x: margin + Math.random() * (width - 2 * margin),
          y: margin + Math.random() * (height - 2 * margin),
        }));

        // Compute k-nearest neighbors for each node
        const kConf = Math.max(2, Math.min(10, get().graphK));
        const k = Math.min(kConf, n - 1);
        const edgesSet = new Set<string>();
        const edges: GraphEdge[] = [];
        const dist = (a: GraphNode, b: GraphNode) => Math.hypot(a.x - b.x, a.y - b.y);
        for (const a of nodes) {
          const neighbors = nodes
            .filter(b => b.id !== a.id)
            .map(b => ({ b, d: dist(a, b) }))
            .sort((u, v) => u.d - v.d)
            .slice(0, k);
          for (const { b, d } of neighbors) {
            const key1 = `${a.id}-${b.id}`;
            const key2 = `${b.id}-${a.id}`;
            if (edgesSet.has(key1) || edgesSet.has(key2)) continue;
            edgesSet.add(key1);
            edges.push({ from: a.id, to: b.id, weight: d });
          }
        }

        // Graph metrics (undirected)
        const E = edges.length;
        const avgDegree = (2 * E) / n;
        const density = n > 1 ? (2 * E) / (n * (n - 1)) : 0;
        const weights = edges.map(e => e.weight);
        const avgWeight = weights.reduce((a, b) => a + b, 0) / Math.max(1, weights.length);
        const minWeight = weights.length ? Math.min(...weights) : 0;
        const maxWeight = weights.length ? Math.max(...weights) : 0;

        set(() => ({
          graph: { ...defaultGraphState, loaded: true, size: n, bounds: defaultGraphState.bounds },
          graphNodes: nodes,
          graphEdges: edges,
          graphMetrics: { nodes: n, edges: E, avgDegree, density, avgWeight, minWeight, maxWeight },
        }));
        console.log('Graph generated:', { nodes: n, edges: edges.length });
      } catch (error) {
        console.error('Failed to load graph:', error);
      }
    },

    regenerateGraph: async (nodes: number) => {
      await get().loadGraph({ nodes, edges: 0, directed: false });
    },

    // Route actions
    calculateRoute: async (params: RouteParams) => {
      try {
        set((state) => ({
          route: {
            ...state.route,
            computing: true,
            algorithm: 'dijkstra',
            metric: params.metric,
            error: null,
            nodes: [],
            cost: 0,
            expanded: 0,
            executionTime: 0,
          }
        }));

        const t0 = performance.now();
        // Build adjacency
        const adj = get().buildAdjacency();
        const N = get().graph.size;
        const src = Math.max(1, Math.min(N, params.source));
        const dst = Math.max(1, Math.min(N, params.target));

        const INF = Number.POSITIVE_INFINITY;
        const dist: number[] = Array(N + 1).fill(INF);
        const parent: number[] = Array(N + 1).fill(-1);
        const visited: boolean[] = Array(N + 1).fill(false);
        dist[src] = 0;

        // Min-heap priority queue
        type Entry = { d: number; u: number };
        const heap: Entry[] = [];
        const swap = (i: number, j: number) => { const t = heap[i]; heap[i] = heap[j]; heap[j] = t; };
        const up = (i: number) => { while (i > 0) { const p = (i - 1) >> 1; if (heap[p].d <= heap[i].d) break; swap(i, p); i = p; } };
        const down = (i: number) => { for (;;) { const l = i * 2 + 1, r = l + 1; let m = i; if (l < heap.length && heap[l].d < heap[m].d) m = l; if (r < heap.length && heap[r].d < heap[m].d) m = r; if (m === i) break; swap(i, m); i = m; } };
        const push = (e: Entry) => { heap.push(e); up(heap.length - 1); };
        const pop = (): Entry | undefined => { if (heap.length === 0) return; const top = heap[0]; const last = heap.pop()!; if (heap.length) { heap[0] = last; down(0); } return top; };

        push({ d: 0, u: src });
        let expanded = 0;
        let relaxations = 0;
        while (heap.length) {
          const cur = pop()!;
          const { d, u } = cur;
          if (visited[u]) continue;
          if (d !== dist[u]) continue;
          visited[u] = true;
          expanded++;
          if (u === dst) break;

          const edges = adj.get(u) || [];
          for (const e of edges) {
            const v = e.to;
            const w = e.weight;
            if (dist[u] + w < dist[v]) {
              dist[v] = dist[u] + w;
              parent[v] = u;
              push({ d: dist[v], u: v });
              relaxations++;
            }
          }
        }

        // Reconstruct path
        const path: number[] = [];
        if (dist[dst] < INF) {
          let cur = dst;
          while (cur !== -1) {
            path.push(cur);
            cur = parent[cur];
          }
          path.reverse();
        }
        const t1 = performance.now();

        set((state) => ({
          route: {
            ...state.route,
            computing: false,
            nodes: path,
            cost: dist[dst] < INF ? dist[dst] : 0,
            expanded,
            executionTime: t1 - t0,
            relaxations,
          }
        }));
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

    buildAdjacency: () => {
      const edges = get().graphEdges;
      const adj = new Map<number, GraphEdge[]>();
      for (const e of edges) {
        if (!adj.has(e.from)) adj.set(e.from, []);
        if (!adj.has(e.to)) adj.set(e.to, []);
        adj.get(e.from)!.push({ ...e });
        adj.get(e.to)!.push({ from: e.to, to: e.from, weight: e.weight });
      }
      return adj;
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
        const state = get();
        const currentTime = Date.now();
        
        // Generate realistic system status based on activity
        const isComputing = state.route.computing;
        const hasActiveRoute = state.route.nodes.length > 0;
        
        // Connections vary based on activity
        const baseConnections = 2;
        const activityConnections = hasActiveRoute ? 2 : 0;
        const connections = baseConnections + activityConnections + Math.floor(Math.random() * 3);
        
        // Uptime increases over time
        const uptime = state.system.uptime + (currentTime - state.system.lastChecked);
        
        set(() => ({
          system: {
            status: isComputing ? 'computing' : 'online',
            version: '1.0.0',
            connections: Math.max(1, connections),
            uptime: uptime,
            lastChecked: currentTime,
          }
        }));
      } catch (error) {
        console.error('Failed to update system status:', error);
      }
    },

    updateMetrics: async () => {
      try {
        const state = get();
        const currentTime = Date.now();
        
        // Generate realistic metrics based on system activity
        const baseLoad = state.route.computing ? 0.7 : 0.3; // Higher load when computing routes
        const graphSize = state.graph.size;
        const routeComplexity = state.route.nodes.length;
        
        // Latency metrics (affected by route complexity)
        const baseLatency = 25 + (routeComplexity * 2);
        const p50 = baseLatency + (Math.random() * 20 - 10);
        const p95 = baseLatency * 2.5 + (Math.random() * 30 - 15);
        
        // Memory usage (affected by graph size)
        const baseMemory = 80 + (graphSize * 0.5);
        const memoryMB = baseMemory + (Math.random() * 40 - 20);
        
        // CPU usage (affected by computing state and graph size)
        const baseCPU = 15 + (graphSize * 0.1);
        const cpuPercent = baseCPU + (baseLoad * 30) + (Math.random() * 20 - 10);
        
        // Requests per second (affected by system activity)
        const baseRPS = 8 + (state.system.connections * 0.5);
        const requestsPerSecond = baseRPS + (Math.random() * 10 - 5);
        
        const newMetrics = {
          p50: Math.max(10, p50),
          p95: Math.max(20, p95),
          memoryMB: Math.max(50, memoryMB),
          cpuPercent: Math.max(5, Math.min(95, cpuPercent)),
          requestsPerSecond: Math.max(1, requestsPerSecond),
          lastUpdated: currentTime,
        };
        
        set(() => ({
          metrics: newMetrics
        }));
      } catch (error) {
        console.error('Failed to update metrics:', error);
      }
    },

    setGraphK: (k: number) => set(() => ({ graphK: Math.max(2, Math.min(10, Math.floor(k))) })),
    setMapHeight: (h: number) => set(() => ({ mapHeight: Math.max(300, Math.min(2000, Math.floor(h))) })),
  })
);

// Selector hooks for components
export const useGraphState = () => useRouteSimulatorStore((state) => state.graph);
export const useGraphNodes = () => useRouteSimulatorStore((state) => state.graphNodes);
export const useGraphEdges = () => useRouteSimulatorStore((state) => state.graphEdges);
export const useTimelineState = () => useRouteSimulatorStore((state) => state.timeline);
export const useMetricsState = () => useRouteSimulatorStore((state) => state.metrics);
export const useSystemState = () => useRouteSimulatorStore((state) => state.system);
export const useGraphMetrics = () => useRouteSimulatorStore((state) => state.graphMetrics);

// Individual route selectors for better stability
export const useRouteNodes = () => useRouteSimulatorStore((state) => state.route.nodes);
export const useRouteCost = () => useRouteSimulatorStore((state) => state.route.cost);
export const useRouteAlgorithm = () => useRouteSimulatorStore((state) => state.route.algorithm);
export const useRouteExecutionTime = () => useRouteSimulatorStore((state) => state.route.executionTime);
export const useRouteExpanded = () => useRouteSimulatorStore((state) => state.route.expanded);
export const useRouteError = () => useRouteSimulatorStore((state) => state.route.error);
export const useRouteMetric = () => useRouteSimulatorStore((state) => state.route.metric);
export const useRouteRelaxations = () => useRouteSimulatorStore((state) => state.route.relaxations);

export const useIsComputing = () => useRouteSimulatorStore((state) => state.route.computing);

// Individual action selectors for better stability
export const useLoadGraph = () => useRouteSimulatorStore((state) => state.loadGraph);
export const useCalculateRoute = () => useRouteSimulatorStore((state) => state.calculateRoute);
export const useClearRoute = () => useRouteSimulatorStore((state) => state.clearRoute);
export const useUpdateSystemStatus = () => useRouteSimulatorStore((state) => state.updateSystemStatus);
export const useUpdateMetrics = () => useRouteSimulatorStore((state) => state.updateMetrics);
export const useRegenerateGraph = () => useRouteSimulatorStore((state) => state.regenerateGraph);
export const useGraphK = () => useRouteSimulatorStore((state) => state.graphK);
export const useSetGraphK = () => useRouteSimulatorStore((state) => state.setGraphK);
export const useMapHeight = () => useRouteSimulatorStore((state) => state.mapHeight);
export const useSetMapHeight = () => useRouteSimulatorStore((state) => state.setMapHeight);

// Timeline action selectors
export const useAddExpansionStep = () => useRouteSimulatorStore((state) => state.addExpansionStep);
export const useClearTimeline = () => useRouteSimulatorStore((state) => state.clearTimeline);
export const useStartStreaming = () => useRouteSimulatorStore((state) => state.startStreaming);
export const useStopStreaming = () => useRouteSimulatorStore((state) => state.stopStreaming);

// Coordinate selectors
export const useRouteCoordinates = () => useRouteSimulatorStore((state) => state.coordinates);
