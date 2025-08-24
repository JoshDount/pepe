import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen } from '@testing-library/react';
import MetricsGrid from '../MetricsGrid';
import { useCurrentRoute, useSystemState } from '../../store';

// Mock the store
vi.mock('../../store', () => ({
  useRouteStore: vi.fn()
}));

const mockUseRouteStore = vi.mocked(useRouteStore);

describe('MetricsGrid Component', () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it('should render all metric cards', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('Route Cost')).toBeInTheDocument();
    expect(screen.getByText('Nodes Expanded')).toBeInTheDocument();
    expect(screen.getByText('Execution Time')).toBeInTheDocument();
    expect(screen.getByText('Algorithm Efficiency')).toBeInTheDocument();
  });

  it('should display default values when no route result', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('--')).toBeInTheDocument();
    expect(screen.getByText('0')).toBeInTheDocument();
    expect(screen.getByText('0.000s')).toBeInTheDocument();
    expect(screen.getByText('N/A')).toBeInTheDocument();
  });

  it('should display route metrics when result is available', () => {
    mockUseRouteStore.mockReturnValue({
      route: {
        result: {
          path: [1, 2, 3, 4, 5],
          cost: 15.75,
          nodesExpanded: 42,
          executionTime: 0.125,
          status: 'success'
        },
        running: false
      },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('15.75')).toBeInTheDocument();
    expect(screen.getByText('42')).toBeInTheDocument();
    expect(screen.getByText('0.125s')).toBeInTheDocument();
    
    // Algorithm efficiency = path length / nodes expanded * 100
    const efficiency = (5 / 42 * 100).toFixed(1);
    expect(screen.getByText(`${efficiency}%`)).toBeInTheDocument();
  });

  it('should format execution time correctly', () => {
    mockUseRouteStore.mockReturnValue({
      route: {
        result: {
          path: [1, 2, 3],
          cost: 10.5,
          nodesExpanded: 25,
          executionTime: 0.001234,
          status: 'success'
        },
        running: false
      },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('0.001s')).toBeInTheDocument();
  });

  it('should display loading state when route is running', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: true },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    const loadingElements = screen.getAllByText('...');
    expect(loadingElements).toHaveLength(4); // One for each metric card
  });

  it('should handle zero nodes expanded correctly', () => {
    mockUseRouteStore.mockReturnValue({
      route: {
        result: {
          path: [1, 2],
          cost: 5.0,
          nodesExpanded: 0,
          executionTime: 0.001,
          status: 'success'
        },
        running: false
      },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('0')).toBeInTheDocument();
    expect(screen.getByText('N/A')).toBeInTheDocument(); // Efficiency when division by zero
  });

  it('should display proper icons for each metric', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    const { container } = render(<MetricsGrid />);

    // Check for SVG icons in each metric card
    const svgElements = container.querySelectorAll('svg');
    expect(svgElements).toHaveLength(4); // One icon per metric
  });

  it('should have proper styling and responsive layout', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    const { container } = render(<MetricsGrid />);

    const grid = container.querySelector('.grid');
    expect(grid).toHaveClass('grid-cols-1', 'md:grid-cols-2', 'lg:grid-cols-4');
    expect(grid).toHaveClass('gap-6');

    const cards = container.querySelectorAll('[class*="bg-bg-surface"]');
    expect(cards).toHaveLength(4);
  });

  it('should display metric descriptions', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('Total path distance')).toBeInTheDocument();
    expect(screen.getByText('Nodes explored')).toBeInTheDocument();
    expect(screen.getByText('Algorithm runtime')).toBeInTheDocument();
    expect(screen.getByText('Path optimality ratio')).toBeInTheDocument();
  });

  it('should handle very large numbers correctly', () => {
    mockUseRouteStore.mockReturnValue({
      route: {
        result: {
          path: new Array(1000).fill(0).map((_, i) => i),
          cost: 999999.99,
          nodesExpanded: 50000,
          executionTime: 10.555,
          status: 'success'
        },
        running: false
      },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    expect(screen.getByText('999999.99')).toBeInTheDocument();
    expect(screen.getByText('50000')).toBeInTheDocument();
    expect(screen.getByText('10.555s')).toBeInTheDocument();
    
    // Efficiency = 1000 / 50000 * 100 = 2.0%
    expect(screen.getByText('2.0%')).toBeInTheDocument();
  });

  it('should have proper accessibility attributes', () => {
    mockUseRouteStore.mockReturnValue({
      route: { result: null, running: false },
      system: { status: 'online' }
    });

    render(<MetricsGrid />);

    const metricsSection = screen.getByRole('region', { name: /performance metrics/i });
    expect(metricsSection).toBeInTheDocument();

    // Check for metric cards having proper ARIA attributes
    const costMetric = screen.getByLabelText(/route cost metric/i);
    expect(costMetric).toBeInTheDocument();
  });
});