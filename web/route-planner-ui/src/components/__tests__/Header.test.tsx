import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen } from '@testing-library/react';
import Header from '../Header';
import { useSystemState } from '../../store';

// Mock the store
vi.mock('../../store', () => ({
  useRouteStore: vi.fn()
}));

const mockUseRouteStore = vi.mocked(useRouteStore);

describe('Header Component', () => {
  beforeEach(() => {
    vi.clearAllMocks();
  });

  it('should render header with title and navigation', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    expect(screen.getByText('RouteOptimizer Pro')).toBeInTheDocument();
    expect(screen.getByText('Advanced Route Planning System')).toBeInTheDocument();
  });

  it('should display online system status', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    const statusIndicator = screen.getByTestId('system-status');
    expect(statusIndicator).toHaveClass('bg-green-500');
    expect(screen.getByText('System Online')).toBeInTheDocument();
  });

  it('should display offline system status', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'offline',
        version: '1.0.0',
        uptime: 0,
        lastUpdate: null
      }
    });

    render(<Header />);

    const statusIndicator = screen.getByTestId('system-status');
    expect(statusIndicator).toHaveClass('bg-red-500');
    expect(screen.getByText('System Offline')).toBeInTheDocument();
  });

  it('should display maintenance system status', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'maintenance',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    const statusIndicator = screen.getByTestId('system-status');
    expect(statusIndicator).toHaveClass('bg-yellow-500');
    expect(screen.getByText('System Maintenance')).toBeInTheDocument();
  });

  it('should display system version when available', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '2.1.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    expect(screen.getByText('v2.1.0')).toBeInTheDocument();
  });

  it('should format uptime correctly', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '1.0.0',
        uptime: 7322, // 2 hours, 2 minutes, 2 seconds
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    expect(screen.getByText('Uptime: 2h 2m')).toBeInTheDocument();
  });

  it('should handle missing system data gracefully', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'offline',
        version: null,
        uptime: 0,
        lastUpdate: null
      }
    });

    render(<Header />);

    expect(screen.getByText('System Offline')).toBeInTheDocument();
    expect(screen.queryByText(/v\d/)).not.toBeInTheDocument();
    expect(screen.queryByText(/Uptime/)).not.toBeInTheDocument();
  });

  it('should have proper accessibility attributes', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    render(<Header />);

    const header = screen.getByRole('banner');
    expect(header).toBeInTheDocument();

    const systemStatus = screen.getByLabelText('System status indicator');
    expect(systemStatus).toBeInTheDocument();
  });

  it('should be responsive and have proper styling', () => {
    mockUseRouteStore.mockReturnValue({
      system: {
        status: 'online',
        version: '1.0.0',
        uptime: 3600,
        lastUpdate: new Date().toISOString()
      }
    });

    const { container } = render(<Header />);

    const header = container.querySelector('header');
    expect(header).toHaveClass('fixed', 'top-0', 'left-0', 'right-0', 'z-50');
    expect(header).toHaveClass('bg-bg-primary/80', 'backdrop-blur-md', 'border-b', 'border-border-default');
  });
});