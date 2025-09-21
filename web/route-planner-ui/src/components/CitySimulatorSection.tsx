/**
 * City Simulator Section Component
 * Wrapper for the city simulator with proper layout
 */

import { FC } from 'react';
import CitySimulator from './CitySimulator';

const CitySimulatorSection: FC = () => {
  const handleRouteCalculated = (result: any) => {
    console.log('Ruta calculada:', result);
  };

  return (
    <div className="px-4 sm:px-6 lg:px-8">
      {/* Header */}
      <div className="text-center mb-6">
        <h2 className="text-3xl sm:text-4xl md:text-5xl lg:text-6xl font-extrabold text-white mb-3 sm:mb-4">
          Simulador de Ciudad Realista
        </h2>
        <p className="text-xs sm:text-sm font-medium text-gray-400 max-w-2xl mx-auto px-4">
          Planificación de rutas usando datos reales de tu ciudad con el algoritmo de Dijkstra
        </p>
      </div>

      {/* City Simulator Content */}
      <CitySimulator onRouteCalculated={handleRouteCalculated} />
    </div>
  );
};

export default CitySimulatorSection;
