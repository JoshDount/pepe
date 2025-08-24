/**
 * Test Utilities and Mock Data
 * Reusable helpers for testing components and services
 */

import { ReactElement } from 'react';
import { render, RenderOptions } from '@testing-library/react';
import { vi, expect } from 'vitest';
import type { RouteResult, ExpansionStep, SystemStatus, PerformanceMetrics } from '../types/api';

// Mock data for testing
export const mockRouteResult: RouteResult = {
  cost: 45.67,
  nodes: [1, 5, 12, 18, 25],
  edges: [1, 7, 15, 23],
  expanded: 42,
  execution_time: 11.23,
  timeline: [
    {
      stepId: 1,
      currentNode: 1,
      openSet: [5, 7],
      closedSet: [],
      gScore: 0,
      fScore: 15.5,
      timestamp: 0,
    },
    {
      stepId: 2,
      currentNode: 5,
      openSet: [7, 12],
      closedSet: [1],
      gScore: 8.2,
      fScore: 22.1,
      timestamp: 2,
    },
  ],
};

export const mockExpansionStep: ExpansionStep = {
  stepId: 1,
  currentNode: 5,
  openSet: [2, 8, 12],
  closedSet: [1],
  gScore: 8.5,
  fScore: 24.3,
  timestamp: 5,
};

export const mockSystemStatus: SystemStatus = {
  status: 'online',
  version: '1.0.0',
  connections: 3,
  uptime: 3600,
};

export const mockPerformanceMetrics: PerformanceMetrics = {
  p50: 12.5,
  p95: 45.2,
  memoryMB: 128.5,
  cpuPercent: 15.3,
  requestsPerSecond: 25.7,
};

// Mock Zustand store
export const createMockStore = (initialState: any = {}) => {
  const store = {
    // Default state
    graph: {
      size: 100,
      loaded: true,
      bounds: {
        minLon: -74.1,
        maxLon: -73.9,
        minLat: 40.6,
        maxLat: 40.8,
      },
    },
    route: {
      computing: false,
      nodes: [],
      edges: [],
      cost: 0,
      expanded: 0,
      algorithm: 'dijkstra' as const,
      metric: 'distance' as const,
      executionTime: 0,
      error: null,
    },
    timeline: {
      streaming: false,
      steps: [],
      stepCount: 0,
      currentStep: 0,
      maxSteps: 5000,
    },
    metrics: {
      p50: 0,
      p95: 0,
      memoryMB: 0,
      cpuPercent: 0,
      requestsPerSecond: 0,
      lastUpdated: 0,
    },
    system: {
      status: 'online' as const,
      version: '1.0.0',
      connections: 0,
      uptime: 0,
      lastChecked: 0,
    },
    coordinates: {},
    
    // Actions
    loadGraph: vi.fn(),
    calculateRoute: vi.fn(),
    clearRoute: vi.fn(),
    addExpansionStep: vi.fn(),
    clearTimeline: vi.fn(),
    startStreaming: vi.fn(),
    stopStreaming: vi.fn(),
    getNodeCoordinates: vi.fn(),
    updateSystemStatus: vi.fn(),
    updateMetrics: vi.fn(),
    
    // Override with provided initial state
    ...initialState,
  };
  
  return store;
};

// Mock API responses
export const mockApiResponses = {
  calculateRoute: {
    ok: true,
    json: () => Promise.resolve(mockRouteResult),
  },
  getSystemStatus: {
    ok: true,
    json: () => Promise.resolve(mockSystemStatus),
  },
  getMetrics: {
    ok: true,
    json: () => Promise.resolve(mockPerformanceMetrics),
  },
  error: {
    ok: false,
    status: 500,
    json: () => Promise.resolve({ error: 'Internal Server Error' }),
  },
};

// Custom render function with providers
interface CustomRenderOptions extends Omit<RenderOptions, 'wrapper'> {
  initialState?: any;
}

export const renderWithProviders = (
  ui: React.ReactElement,
  options: CustomRenderOptions = {}
) => {
  const { initialState, ...renderOptions } = options;
  
  // Mock the store hooks
  vi.doMock('../store', () => {
    const mockStore = createMockStore(initialState);
    return {
      useRouteSimulatorStore: () => mockStore,
      useGraphState: () => mockStore.graph,
      useRouteState: () => mockStore.route,
      useTimelineState: () => mockStore.timeline,
      useMetricsState: () => mockStore.metrics,
      useSystemState: () => mockStore.system,
      useCurrentRoute: () => ({
        nodes: mockStore.route.nodes,
        cost: mockStore.route.cost,
        algorithm: mockStore.route.algorithm,
        executionTime: mockStore.route.executionTime,
        expanded: mockStore.route.expanded,
        error: mockStore.route.error,
      }),
      useIsComputing: () => mockStore.route.computing,
      useRouteActions: () => ({
        loadGraph: mockStore.loadGraph,
        calculateRoute: mockStore.calculateRoute,
        clearRoute: mockStore.clearRoute,
        updateSystemStatus: mockStore.updateSystemStatus,
        getNodeCoordinates: mockStore.getNodeCoordinates,
      }),
      useTimelineActions: () => ({
        addExpansionStep: mockStore.addExpansionStep,
        clearTimeline: mockStore.clearTimeline,
        startStreaming: mockStore.startStreaming,
        stopStreaming: mockStore.stopStreaming,
      }),
    };
  });
  
  return render(ui, renderOptions);
};

// Assertion helpers
export const expectElementToBeVisible = (element: HTMLElement) => {
  expect(element).toBeInTheDocument();
  expect(element).toBeVisible();
};

export const expectButtonToBeEnabled = (button: HTMLElement) => {
  expect(button).toBeInTheDocument();
  expect(button).toBeEnabled();
};

export const expectButtonToBeDisabled = (button: HTMLElement) => {
  expect(button).toBeInTheDocument();
  expect(button).toBeDisabled();
};

// Wait for async operations
export const waitForApiCall = () => new Promise(resolve => setTimeout(resolve, 0));

// Mock fetch implementation
export const mockFetch = (response: any) => {
  global.fetch = vi.fn().mockResolvedValue({
    ok: true,
    json: () => Promise.resolve(response),
  });
};

export const mockFetchError = (status: number = 500, message: string = 'Server Error') => {
  global.fetch = vi.fn().mockResolvedValue({
    ok: false,
    status,
    json: () => Promise.resolve({ error: message }),
  });
};