/**
 * Original Simulator Component
 * Contains the original graph algorithms simulator
 */

import { FC } from 'react';
import MapContainer from './MapContainer';
import UnifiedControlPanel from './UnifiedControlPanel';
import UnifiedResultsPanel from './UnifiedResultsPanel';

const OriginalSimulator: FC = () => {
  return (
    <div className="space-y-6 px-4 sm:px-6 lg:px-8">
      {/* Header */}
      <div className="text-center mb-8">
        <h2 className="text-3xl sm:text-4xl md:text-5xl lg:text-6xl font-extrabold text-white mb-3 sm:mb-4">
          Simulador de Algoritmos de Búsqueda
        </h2>
        <p className="text-xs sm:text-sm font-medium text-gray-400 max-w-2xl mx-auto px-4">
          Explora y compara algoritmos de búsqueda de rutas con grafos sintéticos
        </p>
      </div>

      {/* Main Content - 3 Layer Layout */}
      <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 min-h-[calc(100vh-200px)]">
        {/* Layer 1: Controls (Left) */}
        <div className="lg:col-span-3">
          <UnifiedControlPanel />
        </div>

        {/* Layer 2: Graph (Center) - Main Focus */}
        <div className="lg:col-span-6">
          <div className="h-full">
            <MapContainer />
          </div>
        </div>

        {/* Layer 3: Results (Right) */}
        <div className="lg:col-span-3">
          <UnifiedResultsPanel />
        </div>
      </div>
    </div>
  );
};

export default OriginalSimulator;

