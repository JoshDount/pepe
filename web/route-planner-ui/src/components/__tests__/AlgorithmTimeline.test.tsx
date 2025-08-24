import { describe, it, expect, beforeEach, vi, afterEach } from 'vitest';
import { render, screen, waitFor } from '@testing-library/react';
import AlgorithmTimeline from '../AlgorithmTimeline';
import { useTimelineState, useTimelineActions, useCurrentRoute } from '../../store';
import { sseService } from '../../services/sse';

// Mock the store
vi.mock('../../store', () => ({
  useTimelineState: vi.fn(),
  useTimelineActions: vi.fn(),
  useCurrentRoute: vi.fn()
}));

// Mock SSE service
vi.mock('../../services/sse', () => ({
  sseService: {
    connect: vi.fn(),
    disconnect: vi.fn(),
    isConnected: vi.fn(() => false),
    on: vi.fn(),
    off: vi.fn()
  }
}));

const mockUseTimelineState = vi.mocked(useTimelineState);
const mockUseTimelineActions = vi.mocked(useTimelineActions);
const mockUseCurrentRoute = vi.mocked(useCurrentRoute);
const mockSseService = vi.mocked(sseService);

describe('AlgorithmTimeline Component', () => {
  const mockAddTimelineStep = vi.fn();
  
  beforeEach(() => {
    vi.clearAllMocks();
    
    mockUseRouteStore.mockReturnValue({
      timeline: { steps: [], maxSteps: 5000 },
      route: { running: false, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    mockSSEService.getInstance.mockReturnValue({
      connect: vi.fn(),
      disconnect: vi.fn(),
      isConnected: vi.fn(() => false),
      on: vi.fn(),
      off: vi.fn()
    });
  });

  afterEach(() => {
    vi.clearAllTimers();
  });

  it('should render timeline header', () => {
    render(<AlgorithmTimeline />);

    expect(screen.getByText('Algorithm Timeline')).toBeInTheDocument();
    expect(screen.getByText('Real-time algorithm execution steps')).toBeInTheDocument();
  });

  it('should display empty state when no steps', () => {
    render(<AlgorithmTimeline />);

    expect(screen.getByText('No algorithm steps yet')).toBeInTheDocument();
    expect(screen.getByText('Timeline will appear when route planning starts')).toBeInTheDocument();
  });

  it('should display timeline steps', () => {
    const mockSteps = [
      {
        stepNumber: 1,
        action: 'EXPAND_NODE',
        nodeId: 5,
        cost: 2.5,
        heuristic: 3.0,
        timestamp: Date.now() - 2000
      },
      {
        stepNumber: 2,
        action: 'ADD_TO_OPEN_SET',
        nodeId: 7,
        cost: 4.2,
        heuristic: 2.8,
        timestamp: Date.now() - 1000
      },
      {
        stepNumber: 3,
        action: 'UPDATE_COST',
        nodeId: 3,
        cost: 1.8,
        heuristic: 4.1,
        timestamp: Date.now()
      }
    ];

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: true, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    render(<AlgorithmTimeline />);

    expect(screen.getByText('Step 1')).toBeInTheDocument();
    expect(screen.getByText('Step 2')).toBeInTheDocument();
    expect(screen.getByText('Step 3')).toBeInTheDocument();
    expect(screen.getByText('EXPAND_NODE')).toBeInTheDocument();
    expect(screen.getByText('ADD_TO_OPEN_SET')).toBeInTheDocument();
    expect(screen.getByText('UPDATE_COST')).toBeInTheDocument();
  });

  it('should display node information correctly', () => {
    const mockSteps = [
      {
        stepNumber: 1,
        action: 'EXPAND_NODE',
        nodeId: 42,
        cost: 15.75,
        heuristic: 8.25,
        timestamp: Date.now()
      }
    ];

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: true, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    render(<AlgorithmTimeline />);

    expect(screen.getByText('Node 42')).toBeInTheDocument();
    expect(screen.getByText('Cost: 15.75')).toBeInTheDocument();
    expect(screen.getByText('H: 8.25')).toBeInTheDocument();
  });

  it('should format timestamps correctly', () => {
    const now = Date.now();
    const mockSteps = [
      {
        stepNumber: 1,
        action: 'EXPAND_NODE',
        nodeId: 5,
        cost: 2.5,
        heuristic: 3.0,
        timestamp: now - 5000 // 5 seconds ago
      }
    ];

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: false, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    render(<AlgorithmTimeline />);

    expect(screen.getByText('5s ago')).toBeInTheDocument();
  });

  it('should show connection status', () => {
    mockSSEService.getInstance().isConnected.mockReturnValue(true);
    
    render(<AlgorithmTimeline />);

    expect(screen.getByText('Connected')).toBeInTheDocument();
    const statusIndicator = screen.getByTestId('sse-status');
    expect(statusIndicator).toHaveClass('bg-green-500');
  });

  it('should show disconnected status', () => {
    mockSSEService.getInstance().isConnected.mockReturnValue(false);
    
    render(<AlgorithmTimeline />);

    expect(screen.getByText('Disconnected')).toBeInTheDocument();
    const statusIndicator = screen.getByTestId('sse-status');
    expect(statusIndicator).toHaveClass('bg-red-500');
  });

  it('should connect to SSE when route starts running', () => {
    const mockConnect = vi.fn();
    mockSSEService.getInstance.mockReturnValue({
      connect: mockConnect,
      disconnect: vi.fn(),
      isConnected: vi.fn(() => false),
      on: vi.fn(),
      off: vi.fn()
    });

    // Initially not running
    mockUseRouteStore.mockReturnValue({
      timeline: { steps: [], maxSteps: 5000 },
      route: { running: false, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    const { rerender } = render(<AlgorithmTimeline />);

    // Change to running
    mockUseRouteStore.mockReturnValue({
      timeline: { steps: [], maxSteps: 5000 },
      route: { running: true, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    rerender(<AlgorithmTimeline />);

    expect(mockConnect).toHaveBeenCalledWith('/api/route/timeline');
  });

  it('should disconnect from SSE when component unmounts', () => {
    const mockDisconnect = vi.fn();
    mockSSEService.getInstance.mockReturnValue({
      connect: vi.fn(),
      disconnect: mockDisconnect,
      isConnected: vi.fn(() => true),
      on: vi.fn(),
      off: vi.fn()
    });

    const { unmount } = render(<AlgorithmTimeline />);
    unmount();

    expect(mockDisconnect).toHaveBeenCalled();
  });

  it('should auto-scroll to latest step', async () => {
    const mockSteps = Array.from({ length: 20 }, (_, i) => ({
      stepNumber: i + 1,
      action: 'EXPAND_NODE',
      nodeId: i,
      cost: i * 0.5,
      heuristic: i * 0.3,
      timestamp: Date.now() - (20 - i) * 1000
    }));

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: true, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    const { container } = render(<AlgorithmTimeline />);

    // Check that the timeline container has scroll behavior
    const timelineContainer = container.querySelector('[data-testid="timeline-container"]');
    expect(timelineContainer).toHaveClass('overflow-y-auto');
  });

  it('should display step action badges with correct colors', () => {
    const mockSteps = [
      {
        stepNumber: 1,
        action: 'EXPAND_NODE',
        nodeId: 1,
        cost: 1.0,
        heuristic: 2.0,
        timestamp: Date.now()
      },
      {
        stepNumber: 2,
        action: 'ADD_TO_OPEN_SET',
        nodeId: 2,
        cost: 2.0,
        heuristic: 1.5,
        timestamp: Date.now()
      },
      {
        stepNumber: 3,
        action: 'GOAL_REACHED',
        nodeId: 3,
        cost: 3.0,
        heuristic: 0.0,
        timestamp: Date.now()
      }
    ];

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: false, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    const { container } = render(<AlgorithmTimeline />);

    // Check for different action badge styles
    const expandBadge = screen.getByText('EXPAND_NODE').closest('span');
    const addBadge = screen.getByText('ADD_TO_OPEN_SET').closest('span');
    const goalBadge = screen.getByText('GOAL_REACHED').closest('span');

    expect(expandBadge).toHaveClass('bg-blue-600');
    expect(addBadge).toHaveClass('bg-green-600');
    expect(goalBadge).toHaveClass('bg-purple-600');
  });

  it('should limit displayed steps for performance', () => {
    // Create more steps than should be displayed
    const mockSteps = Array.from({ length: 200 }, (_, i) => ({
      stepNumber: i + 1,
      action: 'EXPAND_NODE',
      nodeId: i,
      cost: i * 0.5,
      heuristic: i * 0.3,
      timestamp: Date.now() - (200 - i) * 100
    }));

    mockUseRouteStore.mockReturnValue({
      timeline: { steps: mockSteps, maxSteps: 5000 },
      route: { running: false, result: null },
      addTimelineStep: mockAddTimelineStep
    });

    render(<AlgorithmTimeline />);

    // Should display only the latest 100 steps
    expect(screen.getByText('Step 101')).toBeInTheDocument();
    expect(screen.getByText('Step 200')).toBeInTheDocument();
    expect(screen.queryByText('Step 100')).not.toBeInTheDocument();
  });

  it('should have proper accessibility attributes', () => {
    render(<AlgorithmTimeline />);

    const timeline = screen.getByRole('region', { name: /algorithm timeline/i });
    expect(timeline).toBeInTheDocument();

    const statusIndicator = screen.getByLabelText(/sse connection status/i);
    expect(statusIndicator).toBeInTheDocument();
  });
});