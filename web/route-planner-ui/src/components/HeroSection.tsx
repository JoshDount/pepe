/**
 * Minimal Hero/Intro Section for Dijkstra
 */
import { FC } from 'react';

interface HeroSectionProps {
  onGetStarted?: () => void;
}

const HeroSection: FC<HeroSectionProps> = ({ onGetStarted }) => {
  return (
    <section className="pt-16 pb-6">
      <div className="max-w-7xl mx-auto px-4 sm:px-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-2xl sm:text-3xl font-bold">Dijkstra (camino mínimo)</h1>
            <p className="text-sm text-gray-400 mt-1">Interfaz enfocada en calcular rutas con Dijkstra.</p>
          </div>
          {onGetStarted && (
            <button onClick={onGetStarted} className="bg-red-600 hover:bg-red-700 text-white font-semibold px-4 py-2 rounded-lg text-sm">
              Ir a controles
            </button>
          )}
        </div>
      </div>
    </section>
  );
};

export default HeroSection;
