import { FC, useEffect, useState } from 'react';
import { useGraphState, useGraphK, useSetGraphK, useRegenerateGraph, useMapHeight, useSetMapHeight } from '../store';

const GraphControls: FC = () => {
  const graph = useGraphState();
  const graphK = useGraphK();
  const setGraphK = useSetGraphK();
  const regenerate = useRegenerateGraph();
  const mapHeight = useMapHeight();
  const setMapHeight = useSetMapHeight();

  const [nodes, setNodes] = useState<number>(graph.size || 80);
  const [kLocal, setKLocal] = useState<number>(graphK);
  const [heightLocal, setHeightLocal] = useState<number>(mapHeight);

  useEffect(() => { setNodes(graph.size || 80); }, [graph.size]);
  useEffect(() => { setKLocal(graphK); }, [graphK]);
  useEffect(() => { setHeightLocal(mapHeight); }, [mapHeight]);

  const onRegenerate = async () => {
    setGraphK(kLocal);
    await regenerate(nodes);
  };

  return (
    <div className="bg-gray-800 rounded-lg p-4 sm:p-6 border border-gray-600 shadow-lg">
      <h3 className="text-lg sm:text-xl font-semibold text-white mb-3">Configuración de Grafo</h3>

      <div className="space-y-4">
        <div>
          <div className="flex items-center justify-between">
            <label className="text-sm text-gray-300">Nodos: <span className="font-semibold text-white">{nodes}</span></label>
            <span className="text-xs text-gray-400">[50–300]</span>
          </div>
          <input type="range" min={50} max={300} step={1} value={nodes} onChange={(e) => setNodes(parseInt(e.target.value))} className="w-full" />
        </div>

        <div>
          <div className="flex items-center justify-between">
            <label className="text-sm text-gray-300">Conectividad k: <span className="font-semibold text-white">{kLocal}</span></label>
            <span className="text-xs text-gray-400">[2–10]</span>
          </div>
          <input type="range" min={2} max={10} step={1} value={kLocal} onChange={(e) => setKLocal(parseInt(e.target.value))} className="w-full" />
        </div>

        <div>
          <div className="flex items-center justify-between">
            <label className="text-sm text-gray-300">Altura del mapa: <span className="font-semibold text-white">{heightLocal}px</span></label>
            <span className="text-xs text-gray-400">[400–1200]</span>
          </div>
          <input type="range" min={400} max={1200} step={20} value={heightLocal} onChange={(e) => { const v = parseInt(e.target.value); setHeightLocal(v); setMapHeight(v); }} className="w-full" />
        </div>

        <div className="flex gap-3">
          <button onClick={onRegenerate} className="bg-red-600 hover:bg-red-700 text-white font-semibold py-2 px-4 rounded-lg text-sm">Regenerar grafo</button>
          <button onClick={() => { setNodes(graph.size || 80); setKLocal(graphK); setHeightLocal(mapHeight); }} className="bg-gray-700 hover:bg-gray-600 text-white font-medium py-2 px-4 rounded-lg text-sm">Restablecer</button>
        </div>
      </div>
    </div>
  );
};

export default GraphControls;

