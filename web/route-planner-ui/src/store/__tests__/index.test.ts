import { describe, it, expect, beforeEach, vi } from 'vitest';
import { useRouteSimulatorStore } from '../index';
import type { RouteResult, SystemStatus, ExpansionStep } from '../../types/api';

// Mock API service
vi.mock('../../services/api', () => ({
  default: {
    loadGraph: vi.fn(),
    findRoute: vi.fn(),
    getSystemStatus: vi.fn()
  }
}));

describe('Route Store', () => {
  beforeEach(() => {
    // Reset store state
    useRouteSimulatorStore.setState({
      graph: { loaded: false, nodes: 0, edges: 0 },
      route: { running: false, result: null },
      timeline: { steps: [], maxSteps: 5000 },
      system: { status: 'offline', connections: 0, uptime: 0, version: '1.0.0' },
      ui: { sidebarOpen: false }
    });
  });

  describe('graph state', () => {
    it('should update graph state when loading', () => {
      const { setGraphLoading } = useRouteStore.getState();
      
      setGraphLoading(true);
      
      const state = useRouteStore.getState();
      expect(state.graph.loaded).toBe(false);
    });

    it('should set graph data correctly', () => {
      const { setGraphData } = useRouteStore.getState();
      
      setGraphData({ nodes: 100, edges: 250 });
      
      const state = useRouteStore.getState();
      expect(state.graph.loaded).toBe(true);
      expect(state.graph.nodes).toBe(100);
      expect(state.graph.edges).toBe(250);
    });
  });

  describe('route state', () => {
    it('should set route running state', () => {
      const { setRouteRunning } = useRouteStore.getState();
      
      setRouteRunning(true);
      
      const state = useRouteStore.getState();
      expect(state.route.running).toBe(true);
    });

    it('should set route result', () => {
      const { setRouteResult } = useRouteStore.getState();
      const mockResult: RouteResult = {
        nodes: [1, 2, 3],
        edges: [10, 11, 12],
        cost: 10.5,
        expanded: 15,
        execution_time: 0.025
      };
      
      setRouteResult(mockResult);
      
      const state = useRouteStore.getState();
      expect(state.route.result).toEqual(mockResult);
    });
  });

  describe('timeline state with circular buffer', () => {
    it('should add timeline steps', () => {
      const { addTimelineStep } = useRouteStore.getState();
      const step: ExpansionStep = {
        stepId: 1,
        currentNode: 5,
        openSet: [1, 2],
        closedSet: [3, 4],
        gScore: 2.5,
        fScore: 3.0,
        timestamp: Date.now()
      };
      
      addTimelineStep(step);
      
      const state = useRouteStore.getState();
      expect(state.timeline.steps).toHaveLength(1);
      expect(state.timeline.steps[0]).toEqual(step);
    });

    it('should maintain circular buffer with max steps', () => {
      const { addTimelineStep } = useRouteStore.getState();
      
      // Add more than max steps
      for (let i = 0; i < 5005; i++) {
        const step: ExpansionStep = {
          stepId: i + 1,
          currentNode: i,
          openSet: [i],
          closedSet: [i - 1],
          gScore: i * 0.5,
          fScore: i * 0.3,
          timestamp: Date.now()
        };
        addTimelineStep(step);
      }
      
      const state = useRouteStore.getState();
      expect(state.timeline.steps).toHaveLength(5000);
      // Should contain the latest 5000 steps (starting from step 6)
      expect(state.timeline.steps[0].stepId).toBe(6);
      expect(state.timeline.steps[4999].stepId).toBe(5005);
    });

    it('should clear timeline', () => {
      const { addTimelineStep, clearTimeline } = useRouteStore.getState();
      
      // Add some steps
      for (let i = 0; i < 10; i++) {
        addTimelineStep({
          stepId: i + 1,
          currentNode: i,
          openSet: [i],
          closedSet: [i - 1],
          gScore: i * 0.5,
          fScore: i * 0.3,
          timestamp: Date.now()
        });
      }
      
      clearTimeline();
      
      const state = useRouteStore.getState();
      expect(state.timeline.steps).toHaveLength(0);
    });
  });

  describe('system state', () => {
    it('should set system status', () => {
      const { setSystemStatus } = useRouteStore.getState();
      const status: SystemStatus = {
        status: 'online',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      };
      
      setSystemStatus(status);
      
      const state = useRouteStore.getState();
      expect(state.system).toEqual(status);
    });
  });

  describe('UI state', () => {
    it('should toggle sidebar', () => {
      const { toggleSidebar } = useRouteStore.getState();
      
      toggleSidebar();
      
      let state = useRouteStore.getState();
      expect(state.ui.sidebarOpen).toBe(true);
      
      toggleSidebar();
      
      state = useRouteStore.getState();
      expect(state.ui.sidebarOpen).toBe(false);
    });
  });

  describe('memoized selectors', () => {
    it('should return correct graph state', () => {
      const { setGraphData } = useRouteStore.getState();
      setGraphData({ nodes: 100, edges: 250 });
      
      // This would be used in components via the selector
      const graphState = useRouteStore.getState().graph;
      expect(graphState.loaded).toBe(true);
      expect(graphState.nodes).toBe(100);
    });

    it('should return latest timeline steps', () => {
      const { addTimelineStep } = useRouteStore.getState();
      
      for (let i = 0; i < 10; i++) {
        addTimelineStep({
          stepNumber: i + 1,
          action: 'EXPAND_NODE',
          nodeId: i,
          cost: i * 0.5,
          heuristic: i * 0.3,
          timestamp: Date.now()
        });
      }
      
      const timelineState = useRouteStore.getState().timeline;
      expect(timelineState.steps).toHaveLength(10);
      expect(timelineState.steps[9].stepNumber).toBe(10);
    });
  });
});