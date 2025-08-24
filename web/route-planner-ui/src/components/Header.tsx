/**
 * Header Component
 * Main navigation and status bar
 */

import { FC } from 'react';
import { useSystemState, useIsComputing } from '../store';
import { styleTokens } from '../styles/tokens';

interface HeaderProps {
  onActionClick?: () => void;
}

const Header: FC<HeaderProps> = ({ onActionClick }) => {
  const { status, connections } = useSystemState();
  const isComputing = useIsComputing();

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'online':
        return 'text-accent-success';
      case 'computing':
        return 'text-accent-warning';
      case 'offline':
        return 'text-text-muted';
      default:
        return 'text-text-secondary';
    }
  };

  const getStatusText = (status: string) => {
    if (isComputing) return 'Computing';
    switch (status) {
      case 'online':
        return 'Online';
      case 'offline':
        return 'Offline';
      default:
        return 'Unknown';
    }
  };

  return (
    <header className="fixed top-0 left-0 right-0 z-50 bg-bg-surface/95 backdrop-blur-sm border-b border-border-default">
      <div className={`${styleTokens.container} py-4`}>
        <div className="flex items-center justify-between">
          {/* Logo and Title */}
          <div className="flex items-center space-x-3">
            <div className="w-10 h-10 bg-accent-primary rounded-lg flex items-center justify-center">
              <svg className="w-6 h-6 text-white" fill="currentColor" viewBox="0 0 24 24">
                <path d="M12 2L13.09 8.26L22 9L13.09 9.74L12 16L10.91 9.74L2 9L10.91 8.26L12 2Z" />
              </svg>
            </div>
            <div>
              <h1 className="text-xl font-bold text-text-primary">Route Transit Simulator</h1>
              <p className="text-sm text-text-secondary">Advanced Pathfinding Analysis</p>
            </div>
          </div>

          {/* Status and Navigation */}
          <div className="flex items-center space-x-6">
            {/* System Status */}
            <div className="flex items-center space-x-4">
              <div className="flex items-center space-x-2">
                <div className={`w-2 h-2 rounded-full ${
                  isComputing ? 'bg-accent-warning animate-pulse' :
                  status === 'online' ? 'bg-accent-success' :
                  'bg-text-muted'
                }`} />
                <span className={`text-sm font-medium ${getStatusColor(status)}`}>
                  {getStatusText(status)}
                </span>
              </div>

              <div className="hidden md:flex items-center space-x-4 text-sm text-text-secondary">
                <div className="flex items-center space-x-1">
                  <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
                    <path d="M13 6a3 3 0 11-6 0 3 3 0 016 0zM18 8a2 2 0 11-4 0 2 2 0 014 0zM14 15a4 4 0 00-8 0v3h8v-3z" />
                  </svg>
                  <span>{connections} connections</span>
                </div>
              </div>
            </div>

            {/* Action Button */}
            {onActionClick && (
              <button
                onClick={onActionClick}
                className={styleTokens.button.primary}
              >
                Get Started
              </button>
            )}

            {/* Menu Button */}
            <button className="p-2 hover:bg-bg-elevated rounded-lg transition-colors">
              <svg className="w-5 h-5 text-text-secondary" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M3 5a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zM3 10a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zM3 15a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1z" clipRule="evenodd" />
              </svg>
            </button>
          </div>
        </div>
      </div>
    </header>
  );
};

export default Header;