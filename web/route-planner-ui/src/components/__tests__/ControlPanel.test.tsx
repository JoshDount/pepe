import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import ControlPanel from '../ControlPanel';
import { useRouteActions, useIsComputing, useGraphState } from '../../store';

// Mock the store
vi.mock('../../store', () => ({
  useRouteStore: vi.fn(),
  useRouteActions: vi.fn()
}));

// Mock API service
vi.mock('../../services/api', () => ({
  api: {
    findRoute: vi.fn()
  }
}));

const mockUseRouteStore = vi.mocked(useRouteStore);

describe('ControlPanel Component', () => {
  const mockExecuteRoute = vi.fn();

  beforeEach(() => {
    vi.clearAllMocks();
    
    mockUseRouteStore.mockReturnValue({
      graph: { loaded: true, nodes: 100, edges: 250 },
      route: { running: false, result: null },
      executeRoute: mockExecuteRoute
    });
  });

  it('should render all form fields', () => {
    render(<ControlPanel />);

    expect(screen.getByLabelText('Start Node')).toBeInTheDocument();
    expect(screen.getByLabelText('End Node')).toBeInTheDocument();
    expect(screen.getByLabelText('Algorithm')).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /find route/i })).toBeInTheDocument();
  });

  it('should have A* selected by default', () => {
    render(<ControlPanel />);

    const algorithmSelect = screen.getByLabelText('Algorithm') as HTMLSelectElement;
    expect(algorithmSelect.value).toBe('A_STAR');
  });

  it('should allow changing algorithm selection', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const algorithmSelect = screen.getByLabelText('Algorithm');
    await user.selectOptions(algorithmSelect, 'DIJKSTRA');

    expect((algorithmSelect as HTMLSelectElement).value).toBe('DIJKSTRA');
  });

  it('should validate node inputs', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const findButton = screen.getByRole('button', { name: /find route/i });

    // Test negative values
    await user.type(startNodeInput, '-1');
    await user.type(endNodeInput, '50');
    await user.click(findButton);

    await waitFor(() => {
      expect(screen.getByText('Start node must be between 0 and 99')).toBeInTheDocument();
    });
  });

  it('should validate nodes are within graph range', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const findButton = screen.getByRole('button', { name: /find route/i });

    // Test values beyond graph size
    await user.type(startNodeInput, '150');
    await user.type(endNodeInput, '200');
    await user.click(findButton);

    await waitFor(() => {
      expect(screen.getByText('Start node must be between 0 and 99')).toBeInTheDocument();
      expect(screen.getByText('End node must be between 0 and 99')).toBeInTheDocument();
    });
  });

  it('should validate that start and end nodes are different', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const findButton = screen.getByRole('button', { name: /find route/i });

    await user.type(startNodeInput, '25');
    await user.type(endNodeInput, '25');
    await user.click(findButton);

    await waitFor(() => {
      expect(screen.getByText('Start and end nodes must be different')).toBeInTheDocument();
    });
  });

  it('should execute route with valid inputs', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const algorithmSelect = screen.getByLabelText('Algorithm');
    const findButton = screen.getByRole('button', { name: /find route/i });

    await user.type(startNodeInput, '10');
    await user.type(endNodeInput, '90');
    await user.selectOptions(algorithmSelect, 'DIJKSTRA');
    await user.click(findButton);

    await waitFor(() => {
      expect(mockExecuteRoute).toHaveBeenCalledWith({
        startNode: 10,
        endNode: 90,
        algorithm: 'DIJKSTRA'
      });
    });
  });

  it('should disable button when route is running', () => {
    mockUseRouteStore.mockReturnValue({
      graph: { loaded: true, nodes: 100, edges: 250 },
      route: { running: true, result: null },
      executeRoute: mockExecuteRoute
    });

    render(<ControlPanel />);

    const findButton = screen.getByRole('button', { name: /finding route/i });
    expect(findButton).toBeDisabled();
    expect(findButton).toHaveTextContent('Finding Route...');
  });

  it('should disable button when graph is not loaded', () => {
    mockUseRouteStore.mockReturnValue({
      graph: { loaded: false, nodes: 0, edges: 0 },
      route: { running: false, result: null },
      executeRoute: mockExecuteRoute
    });

    render(<ControlPanel />);

    const findButton = screen.getByRole('button', { name: /find route/i });
    expect(findButton).toBeDisabled();
  });

  it('should clear validation errors when inputs change', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const findButton = screen.getByRole('button', { name: /find route/i });

    // First, create an error
    await user.type(startNodeInput, '25');
    await user.type(endNodeInput, '25');
    await user.click(findButton);

    await waitFor(() => {
      expect(screen.getByText('Start and end nodes must be different')).toBeInTheDocument();
    });

    // Then change input to clear error
    await user.clear(endNodeInput);
    await user.type(endNodeInput, '50');

    await waitFor(() => {
      expect(screen.queryByText('Start and end nodes must be different')).not.toBeInTheDocument();
    });
  });

  it('should have proper accessibility attributes', () => {
    render(<ControlPanel />);

    const form = screen.getByRole('form');
    expect(form).toBeInTheDocument();

    const startNodeInput = screen.getByLabelText('Start Node');
    expect(startNodeInput).toHaveAttribute('aria-describedby');

    const endNodeInput = screen.getByLabelText('End Node');
    expect(endNodeInput).toHaveAttribute('aria-describedby');

    const algorithmSelect = screen.getByLabelText('Algorithm');
    expect(algorithmSelect).toHaveAttribute('aria-describedby');
  });

  it('should display algorithm descriptions', () => {
    render(<ControlPanel />);

    expect(screen.getByText(/optimal pathfinding with heuristic/i)).toBeInTheDocument();
    expect(screen.getByText(/guaranteed shortest path/i)).toBeInTheDocument();
  });

  it('should handle keyboard navigation', async () => {
    const user = userEvent.setup();
    render(<ControlPanel />);

    const startNodeInput = screen.getByLabelText('Start Node');
    const endNodeInput = screen.getByLabelText('End Node');
    const algorithmSelect = screen.getByLabelText('Algorithm');

    // Tab through inputs
    await user.tab();
    expect(startNodeInput).toHaveFocus();

    await user.tab();
    expect(endNodeInput).toHaveFocus();

    await user.tab();
    expect(algorithmSelect).toHaveFocus();
  });
});