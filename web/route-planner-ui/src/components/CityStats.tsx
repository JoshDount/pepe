/**
 * City Stats Component
 * Display statistics about the loaded city data
 */

import { FC } from 'react';

interface CityNode {
  id: number;
  lat: number;
  lon: number;
  elevation?: number | null;
}

interface CityStatsProps {
  nodes: CityNode[];
}

const CityStats: FC<CityStatsProps> = ({ nodes }) => {
  if (nodes.length === 0) {
    return (
      <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
        <h3 className="text-xl sm:text-2xl font-semibold text-white mb-4">Estadísticas de la Ciudad</h3>
        <div className="text-center py-8">
          <div className="text-gray-400">No hay datos de ciudad cargados</div>
        </div>
      </div>
    );
  }

  // Calculate statistics
  const lats = nodes.map(node => node.lat);
  const lons = nodes.map(node => node.lon);
  const elevations = nodes.filter(node => node.elevation !== null && node.elevation !== undefined).map(node => node.elevation!);

  const minLat = Math.min(...lats);
  const maxLat = Math.max(...lats);
  const minLon = Math.min(...lons);
  const maxLon = Math.max(...lons);
  const centerLat = (minLat + maxLat) / 2;
  const centerLon = (minLon + maxLon) / 2;

  // Calculate approximate area in km²
  const latRange = maxLat - minLat;
  const lonRange = maxLon - minLon;
  const avgLat = centerLat;
  const latKm = latRange * 111.32; // 1 degree latitude ≈ 111.32 km
  const lonKm = lonRange * 111.32 * Math.cos(avgLat * Math.PI / 180);
  const areaKm2 = latKm * lonKm;

  // Calculate density
  const density = nodes.length / areaKm2;

  // Elevation statistics
  const minElevation = elevations.length > 0 ? Math.min(...elevations) : null;
  const maxElevation = elevations.length > 0 ? Math.max(...elevations) : null;
  const avgElevation = elevations.length > 0 ? elevations.reduce((a, b) => a + b, 0) / elevations.length : null;

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-4 h-fit">
      <h3 className="text-lg font-semibold text-white mb-4">Estadísticas de la Ciudad</h3>
      
      <div className="space-y-4">
        {/* Basic Stats */}
        <div className="space-y-3">
          <h4 className="text-base font-medium text-white">Información General</h4>
          <div className="space-y-2 text-sm">
            <div className="flex justify-between">
              <span className="text-gray-400">Total de nodos:</span>
              <span className="text-white font-medium">{nodes.length.toLocaleString()}</span>
            </div>
            <div className="flex justify-between">
              <span className="text-gray-400">Área aproximada:</span>
              <span className="text-white font-medium">{areaKm2.toFixed(2)} km²</span>
            </div>
            <div className="flex justify-between">
              <span className="text-gray-400">Densidad:</span>
              <span className="text-white font-medium">{density.toFixed(1)} nodos/km²</span>
            </div>
          </div>
        </div>

        {/* Coordinates */}
        <div className="space-y-3">
          <h4 className="text-lg font-medium text-white">Coordenadas</h4>
          <div className="space-y-2 text-sm">
            <div className="flex justify-between">
              <span className="text-gray-400">Centro:</span>
              <span className="text-white font-medium text-xs">
                {centerLat.toFixed(6)}, {centerLon.toFixed(6)}
              </span>
            </div>
            <div className="flex justify-between">
              <span className="text-gray-400">Latitud:</span>
              <span className="text-white font-medium text-xs">
                {minLat.toFixed(6)} - {maxLat.toFixed(6)}
              </span>
            </div>
            <div className="flex justify-between">
              <span className="text-gray-400">Longitud:</span>
              <span className="text-white font-medium text-xs">
                {minLon.toFixed(6)} - {maxLon.toFixed(6)}
              </span>
            </div>
          </div>
        </div>

        {/* Elevation */}
        {elevations.length > 0 && (
          <div className="space-y-3">
            <h4 className="text-base font-medium text-white">Elevación</h4>
            <div className="space-y-2 text-sm">
              <div className="flex justify-between">
                <span className="text-gray-400">Mínima:</span>
                <span className="text-white font-medium">{minElevation?.toFixed(1)}m</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Máxima:</span>
                <span className="text-white font-medium">{maxElevation?.toFixed(1)}m</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Promedio:</span>
                <span className="text-white font-medium">{avgElevation?.toFixed(1)}m</span>
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Map Preview */}
      <div className="mt-4">
        <h4 className="text-base font-medium text-white mb-3">Vista Previa del Área</h4>
        <div className="bg-gray-700 rounded-lg p-4">
          <div className="text-sm text-gray-400 mb-2">
            Área cubierta: {latRange.toFixed(4)}° × {lonRange.toFixed(4)}°
          </div>
          <div className="text-xs text-gray-500">
            Dimensiones aproximadas: {latKm.toFixed(1)} km × {lonKm.toFixed(1)} km
          </div>
        </div>
      </div>

      {/* Sample Nodes */}
      <div className="mt-4">
        <h4 className="text-base font-medium text-white mb-3">Nodos de Ejemplo</h4>
        <div className="bg-gray-700 rounded-lg p-4 max-h-32 overflow-y-auto">
          <div className="space-y-1 text-xs">
            {nodes.slice(0, 5).map((node) => (
              <div key={node.id} className="flex justify-between text-gray-300">
                <span>ID: {node.id}</span>
                <span>{node.lat.toFixed(6)}, {node.lon.toFixed(6)}</span>
              </div>
            ))}
            {nodes.length > 5 && (
              <div className="text-gray-500 text-center">
                ... y {nodes.length - 5} nodos más
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};

export default CityStats;
