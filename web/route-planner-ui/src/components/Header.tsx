/**
 * Header Component
 * Main navigation and status bar
 */

import { FC } from 'react';
import { useSystemState, useIsComputing } from '../store';

export type TabType = 'original' | 'city' | 'performance';

interface HeaderProps {
  onActionClick?: () => void;
  activeTab?: TabType;
  onTabChange?: (tab: TabType) => void;
}

const Header: FC<HeaderProps> = ({ onActionClick, activeTab, onTabChange }) => {
  const { status, connections } = useSystemState();
  const isComputing = useIsComputing();

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'online':
        return 'text-green-500';
      case 'computing':
        return 'text-yellow-500';
      case 'offline':
        return 'text-gray-500';
      default:
        return 'text-gray-400';
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

  const tabs = [
    {
      id: 'original' as TabType,
      label: 'Simulador Original',
      description: 'Algoritmos de búsqueda con grafos sintéticos',
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path d="M13 6a3 3 0 11-6 0 3 3 0 016 0zM18 8a2 2 0 11-4 0 2 2 0 014 0zM14 15a4 4 0 00-8 0v3h8v-3z" />
        </svg>
      )
    },
    {
      id: 'city' as TabType,
      label: 'Simulador de Ciudad',
      description: 'Planificación de rutas con datos reales de tu ciudad',
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path fillRule="evenodd" d="M5.05 4.05a7 7 0 119.9 9.9L10 18.9l-4.95-4.95a7 7 0 010-9.9zM10 11a2 2 0 100-4 2 2 0 000 4z" clipRule="evenodd" />
        </svg>
      )
    },
    {
      id: 'performance' as TabType,
      label: 'Rendimiento',
      description: 'Métricas del sistema y análisis de rendimiento',
      icon: (
        <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
          <path d="M2 11a1 1 0 011-1h2a1 1 0 011 1v5a1 1 0 01-1 1H3a1 1 0 01-1-1v-5zM8 7a1 1 0 011-1h2a1 1 0 011 1v9a1 1 0 01-1 1H9a1 1 0 01-1-1V7zM14 4a1 1 0 011-1h2a1 1 0 011 1v12a1 1 0 01-1 1h-2a1 1 0 01-1-1V4z" />
        </svg>
      )
    }
  ];

  return (
    <header className="fixed top-0 left-0 right-0 z-50 bg-gray-800/95 backdrop-blur-sm border-b border-gray-700">
      {/* Main Header */}
      <div className="max-w-7xl mx-auto px-4 sm:px-6 py-3 sm:py-4">
        <div className="flex items-center justify-between">
          <div className="flex items-center space-x-2 sm:space-x-3 min-w-0">
            <div className="w-8 h-8 sm:w-10 sm:h-10 bg-red-600 rounded-lg flex items-center justify-center flex-shrink-0">
              <svg className="w-4 h-4 sm:w-6 sm:h-6 text-white" fill="currentColor" viewBox="0 0 24 24">
                <path d="M12 2L13.09 8.26L22 9L13.09 9.74L12 16L10.91 9.74L2 9L10.91 8.26L12 2Z" />
              </svg>
            </div>
            <div className="min-w-0">
              <h1 className="text-sm sm:text-lg lg:text-xl font-bold text-white whitespace-normal break-words">Dijkstra — Planificador de Rutas</h1>
              <p className="text-xs sm:text-sm text-gray-400 whitespace-normal break-words">Interfaz educativa y concisa</p>
            </div>
          </div>

          <div className="flex items-center space-x-3 sm:space-x-4">
            <div className="flex items-center space-x-2">
              <div className={`w-2 h-2 rounded-full ${isComputing ? 'bg-yellow-500 animate-pulse' : status === 'online' ? 'bg-green-500' : 'bg-gray-500'}`} />
              <span className={`text-xs sm:text-sm font-medium ${getStatusColor(status)} hidden sm:inline`}>
                {getStatusText(status)}
              </span>
            </div>
            {onActionClick && (
              <button onClick={onActionClick} className="bg-red-600 hover:bg-red-700 text-white font-semibold px-3 py-1.5 rounded-lg transition-colors text-xs sm:text-sm">
                Ir a Controles
              </button>
            )}
          </div>
        </div>
      </div>

      {/* Navigation Tabs */}
      {activeTab && onTabChange && (
        <div className="border-t border-gray-600">
          <div className="max-w-7xl mx-auto px-4 sm:px-6">
            <nav className="flex space-x-1 py-3">
              {tabs.map((tab) => (
                <button
                  key={tab.id}
                  onClick={() => onTabChange(tab.id)}
                  className={`
                    flex items-center space-x-2 px-3 py-2 rounded-lg font-medium transition-all duration-200 text-sm
                    ${activeTab === tab.id
                      ? 'bg-blue-600 text-white shadow-lg'
                      : 'text-gray-400 hover:text-white hover:bg-gray-700/50'
                    }
                  `}
                >
                  <div className={`
                    ${activeTab === tab.id ? 'text-white' : 'text-gray-500'}
                  `}>
                    {tab.icon}
                  </div>
                  <div className="text-left">
                    <div className="font-semibold">{tab.label}</div>
                    <div className="text-xs opacity-75 hidden sm:block">{tab.description}</div>
                  </div>
                </button>
              ))}
            </nav>
          </div>
        </div>
      )}
    </header>
  );
};

export default Header;
