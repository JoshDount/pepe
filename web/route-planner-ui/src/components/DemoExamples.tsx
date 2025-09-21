/**
 * Demo Examples Component
 * Predefined examples for classroom demonstration
 */

import { FC } from 'react';

interface DemoExample {
  id: string;
  name: string;
  description: string;
  sourceId: number;
  targetId: number;
  difficulty: 'easy' | 'medium' | 'hard';
  learningObjective: string;
}

interface DemoExamplesProps {
  onSelectExample: (sourceId: number, targetId: number, description: string) => void;
  availableNodes: Array<{id: number, lat: number, lon: number}>;
}

const DemoExamples: FC<DemoExamplesProps> = ({ onSelectExample, availableNodes }) => {
  // Generate examples based on available nodes
  const generateExamples = (): DemoExample[] => {
    if (availableNodes.length < 2) return [];

    const examples: DemoExample[] = [];
    
    // Easy example: close nodes
    if (availableNodes.length >= 2) {
      const closeNodes = availableNodes
        .map(node => ({
          ...node,
          distance: Math.sqrt(
            Math.pow(node.lat - 24.7841, 2) + Math.pow(node.lon - (-107.3866), 2)
          )
        }))
        .sort((a, b) => a.distance - b.distance)
        .slice(0, 10);

      if (closeNodes.length >= 2) {
        examples.push({
          id: 'easy-1',
          name: '🏫 Ruta Corta (Fácil)',
          description: 'Ruta entre dos puntos cercanos - ideal para ver el algoritmo paso a paso',
          sourceId: closeNodes[0].id,
          targetId: closeNodes[1].id,
          difficulty: 'easy',
          learningObjective: 'Entender la exploración básica del algoritmo'
        });
      }
    }

    // Medium example: medium distance
    if (availableNodes.length >= 4) {
      const sortedNodes = availableNodes
        .map(node => ({
          ...node,
          distance: Math.sqrt(
            Math.pow(node.lat - 24.7841, 2) + Math.pow(node.lon - (-107.3866), 2)
          )
        }))
        .sort((a, b) => a.distance - b.distance);

      const midIndex = Math.floor(sortedNodes.length / 2);
      if (midIndex >= 1 && midIndex + 1 < sortedNodes.length) {
        examples.push({
          id: 'medium-1',
          name: '🏢 Ruta Media (Intermedio)',
          description: 'Ruta de distancia media - muestra la eficiencia del algoritmo',
          sourceId: sortedNodes[0].id,
          targetId: sortedNodes[midIndex].id,
          difficulty: 'medium',
          learningObjective: 'Analizar la complejidad y eficiencia'
        });
      }
    }

    // Hard example: far nodes
    if (availableNodes.length >= 2) {
      const farNodes = availableNodes
        .map(node => ({
          ...node,
          distance: Math.sqrt(
            Math.pow(node.lat - 24.7841, 2) + Math.pow(node.lon - (-107.3866), 2)
          )
        }))
        .sort((a, b) => b.distance - a.distance)
        .slice(0, 5);

      if (farNodes.length >= 2) {
        examples.push({
          id: 'hard-1',
          name: '🌆 Ruta Larga (Avanzado)',
          description: 'Ruta entre puntos distantes - demuestra la escalabilidad del algoritmo',
          sourceId: farNodes[0].id,
          targetId: farNodes[1].id,
          difficulty: 'hard',
          learningObjective: 'Evaluar el rendimiento en casos complejos'
        });
      }
    }

    // Special educational examples
    examples.push({
      id: 'demo-1',
      name: '🎓 Demostración Clase',
      description: 'Ejemplo optimizado para demostración en clase - muestra conceptos clave',
      sourceId: availableNodes[0]?.id || 0,
      targetId: availableNodes[Math.min(5, availableNodes.length - 1)]?.id || 0,
      difficulty: 'easy',
      learningObjective: 'Demostrar conceptos fundamentales de pathfinding'
    });

    return examples.filter(ex => ex.sourceId !== ex.targetId);
  };

  const examples = generateExamples();

  const getDifficultyColor = (difficulty: string) => {
    switch (difficulty) {
      case 'easy': return 'bg-green-600 hover:bg-green-700';
      case 'medium': return 'bg-yellow-600 hover:bg-yellow-700';
      case 'hard': return 'bg-red-600 hover:bg-red-700';
      default: return 'bg-gray-600 hover:bg-gray-700';
    }
  };

  const getDifficultyIcon = (difficulty: string) => {
    switch (difficulty) {
      case 'easy': return '🟢';
      case 'medium': return '🟡';
      case 'hard': return '🔴';
      default: return '⚪';
    }
  };

  return (
    <div className="bg-gray-800/95 backdrop-blur-sm rounded-xl border border-gray-600 shadow-xl p-6">
      <div className="flex items-center justify-between mb-6">
        <h3 className="text-xl font-bold text-white">🎯 Ejemplos para Demostración</h3>
        <div className="text-sm text-gray-400">
          {examples.length} ejemplos disponibles
        </div>
      </div>

      <div className="space-y-4">
        {examples.map((example) => (
          <div
            key={example.id}
            className="bg-gray-700 rounded-lg p-4 border border-gray-600 hover:border-gray-500 transition-colors"
          >
            <div className="flex items-start justify-between mb-3">
              <div className="flex-1">
                <div className="flex items-center gap-2 mb-2">
                  <h4 className="text-lg font-semibold text-white">
                    {example.name}
                  </h4>
                  <span className="text-lg">{getDifficultyIcon(example.difficulty)}</span>
                </div>
                <p className="text-gray-300 text-sm mb-2">
                  {example.description}
                </p>
                <div className="bg-blue-900/30 border border-blue-600 rounded-lg p-2">
                  <div className="text-blue-300 text-xs font-medium mb-1">
                    🎓 Objetivo de Aprendizaje:
                  </div>
                  <div className="text-blue-200 text-xs">
                    {example.learningObjective}
                  </div>
                </div>
              </div>
            </div>

            <div className="flex items-center justify-between">
              <div className="text-xs text-gray-400">
                Origen: {example.sourceId} → Destino: {example.targetId}
              </div>
              <button
                onClick={() => onSelectExample(example.sourceId, example.targetId, example.description)}
                className={`px-4 py-2 rounded-lg text-sm font-medium text-white transition-colors ${getDifficultyColor(example.difficulty)}`}
              >
                Usar este Ejemplo
              </button>
            </div>
          </div>
        ))}
      </div>

      <div className="mt-6 p-4 bg-purple-900/30 border border-purple-600 rounded-lg">
        <h5 className="text-purple-300 font-semibold mb-2">💡 Consejos para la Demostración:</h5>
        <ul className="space-y-1 text-sm text-purple-200">
          <li>• Comienza con ejemplos fáciles para explicar conceptos básicos</li>
          <li>• Usa ejemplos medios para mostrar la eficiencia del algoritmo</li>
          <li>• Compara diferentes algoritmos con el mismo ejemplo</li>
          <li>• Observa cómo cambian las métricas con la complejidad</li>
          <li>• Explica la diferencia entre exploración y explotación</li>
        </ul>
      </div>
    </div>
  );
};

export default DemoExamples;
