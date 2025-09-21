import { FC, useMemo } from 'react';
import { useGraphMetrics, useGraphState, useRouteNodes, useRouteCost, useRouteExecutionTime, useRouteExpanded, useRouteAlgorithm, useRouteRelaxations } from '../store';

const GraphStats: FC = () => {
  const gm = useGraphMetrics();
  const graph = useGraphState();
  const routeNodes = useRouteNodes();
  const routeCost = useRouteCost();
  const routeTime = useRouteExecutionTime();
  const expanded = useRouteExpanded();
  const algo = useRouteAlgorithm();
  const relaxations = useRouteRelaxations();

  const pathAvgCost = useMemo(() => routeNodes.length > 0 ? routeCost / routeNodes.length : 0, [routeNodes, routeCost]);

  const fmt = (n: number, d = 2) => Number.isFinite(n) ? n.toFixed(d) : '—';

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
      <h3 className="text-lg sm:text-xl font-semibold text-white mb-3">Estadísticas</h3>

      <div className="grid grid-cols-2 gap-3 sm:gap-4">
        <div>
          <div className="text-xs text-gray-400">Nodos</div>
          <div className="text-white font-semibold">{gm.nodes || graph.size}</div>
        </div>
        <div>
          <div className="text-xs text-gray-400">Aristas</div>
          <div className="text-white font-semibold">{gm.edges}</div>
        </div>
        <div>
          <div className="text-xs text-gray-400">Grado medio</div>
          <div className="text-white font-semibold">{fmt(gm.avgDegree, 2)}</div>
        </div>
        <div>
          <div className="text-xs text-gray-400">Densidad</div>
          <div className="text-white font-semibold">{fmt(gm.density, 3)}</div>
        </div>
        <div>
          <div className="text-xs text-gray-400">Peso medio</div>
          <div className="text-white font-semibold">{fmt(gm.avgWeight, 1)}</div>
        </div>
        <div>
          <div className="text-xs text-gray-400">Peso (min–max)</div>
          <div className="text-white font-semibold">{fmt(gm.minWeight, 1)} – {fmt(gm.maxWeight, 1)}</div>
        </div>
      </div>

      {routeNodes.length > 0 && (
        <div className="mt-4 border-t border-gray-700 pt-4">
          <div className="grid grid-cols-2 gap-3 sm:gap-4">
            <div>
              <div className="text-xs text-gray-400">Ruta nodos</div>
              <div className="text-white font-semibold">{routeNodes.length}</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Costo total</div>
              <div className="text-white font-semibold">{fmt(routeCost, 1)}</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Expandidos</div>
              <div className="text-white font-semibold">{expanded}</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Tiempo</div>
              <div className="text-white font-semibold">{fmt(routeTime, 2)} ms</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Relajaciones</div>
              <div className="text-white font-semibold">{relaxations}</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Prom. costo/nodo</div>
              <div className="text-white font-semibold">{fmt(pathAvgCost, 2)}</div>
            </div>
            <div>
              <div className="text-xs text-gray-400">Algoritmo</div>
              <div className="text-white font-semibold">{algo?.toUpperCase()}</div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default GraphStats;
