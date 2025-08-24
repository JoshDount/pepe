/**
 * Hero Section Component
 * Main title section with gradient background and professional styling
 */

import { FC } from 'react';
import { useGraphState, useCurrentRoute } from '../store';
import { styleTokens } from '../styles/tokens';

interface HeroSectionProps {
  onGetStarted?: () => void;
}

const HeroSection: FC<HeroSectionProps> = ({ onGetStarted }) => {
  const { loaded, size } = useGraphState();
  const { nodes, cost, algorithm } = useCurrentRoute();

  const formatCost = (cost: number): string => {
    if (cost === 0) return '0';
    if (cost < 1000) return cost.toFixed(2);
    if (cost < 1000000) return `${(cost / 1000).toFixed(1)}k`;
    return `${(cost / 1000000).toFixed(1)}M`;
  };

  return (
    <section className="relative pt-24 pb-16 overflow-hidden">
      {/* Background Gradient Effects */}
      <div className="absolute inset-0 pointer-events-none">
        {/* Primary radial gradient */}
        <div 
          className="absolute top-1/4 left-1/4 w-96 h-96 opacity-10"
          style={{
            background: 'radial-gradient(circle, #B91C1C 0%, transparent 70%)',
            filter: 'blur(60px)',
          }}
        />
        
        {/* Secondary radial gradient */}
        <div 
          className="absolute top-1/3 right-1/4 w-80 h-80 opacity-8"
          style={{
            background: 'radial-gradient(circle, #EF4444 0%, transparent 70%)',
            filter: 'blur(80px)',
          }}
        />
        
        {/* Subtle grid pattern */}
        <div 
          className="absolute inset-0 opacity-5"
          style={{
            backgroundImage: `
              linear-gradient(rgba(185, 28, 28, 0.1) 1px, transparent 1px),
              linear-gradient(90deg, rgba(185, 28, 28, 0.1) 1px, transparent 1px)
            `,
            backgroundSize: '40px 40px',
          }}
        />
      </div>

      <div className={`${styleTokens.container} relative z-10`}>
        <div className="text-center max-w-4xl mx-auto">
          {/* Main Title */}
          <h1 className="text-4xl md:text-5xl lg:text-6xl font-extrabold text-text-primary leading-tight mb-6">
            Advanced Route
            <span className="block text-gradient bg-gradient-to-r from-text-primary to-accent-primary bg-clip-text text-transparent">
              Planning & Analysis
            </span>
          </h1>

          {/* Subtitle */}
          <p className="text-lg md:text-xl text-text-secondary max-w-2xl mx-auto mb-8 leading-relaxed">
            Powerful pathfinding algorithms with real-time visualization. 
            Compare A* and Dijkstra performance on complex transit networks.
          </p>

          {/* Feature Highlights */}
          <div className="flex flex-wrap justify-center gap-4 mb-10">
            <div className="flex items-center space-x-2 bg-bg-surface/50 backdrop-blur-sm px-4 py-2 rounded-full border border-border-muted">
              <div className="w-2 h-2 bg-accent-success rounded-full" />
              <span className="text-sm text-text-secondary font-medium">Real-time Algorithm Visualization</span>
            </div>
            
            <div className="flex items-center space-x-2 bg-bg-surface/50 backdrop-blur-sm px-4 py-2 rounded-full border border-border-muted">
              <div className="w-2 h-2 bg-accent-primary rounded-full" />
              <span className="text-sm text-text-secondary font-medium">Performance Metrics & Analysis</span>
            </div>
            
            <div className="flex items-center space-x-2 bg-bg-surface/50 backdrop-blur-sm px-4 py-2 rounded-full border border-border-muted">
              <div className="w-2 h-2 bg-accent-warning rounded-full" />
              <span className="text-sm text-text-secondary font-medium">Interactive Route Planning</span>
            </div>
          </div>

          {/* Status Cards */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-10 max-w-3xl mx-auto">
            {/* Graph Status */}
            <div className="bg-bg-surface/30 backdrop-blur-sm border border-border-muted rounded-xl p-6">
              <div className="text-2xl font-bold text-text-primary mb-1">
                {loaded ? size.toLocaleString() : '—'}
              </div>
              <div className="text-sm text-text-secondary">
                {loaded ? 'Nodes Loaded' : 'No Graph Loaded'}
              </div>
              <div className={`w-full h-1 rounded-full mt-2 ${
                loaded ? 'bg-accent-success' : 'bg-text-muted'
              }`} />
            </div>

            {/* Route Status */}
            <div className="bg-bg-surface/30 backdrop-blur-sm border border-border-muted rounded-xl p-6">
              <div className="text-2xl font-bold text-text-primary mb-1">
                {nodes.length > 0 ? nodes.length : '—'}
              </div>
              <div className="text-sm text-text-secondary">
                {nodes.length > 0 ? 'Route Nodes' : 'No Route Calculated'}
              </div>
              <div className={`w-full h-1 rounded-full mt-2 ${
                nodes.length > 0 ? 'bg-accent-primary' : 'bg-text-muted'
              }`} />
            </div>

            {/* Algorithm Status */}
            <div className="bg-bg-surface/30 backdrop-blur-sm border border-border-muted rounded-xl p-6">
              <div className="text-2xl font-bold text-text-primary mb-1">
                {cost > 0 ? formatCost(cost) : '—'}
              </div>
              <div className="text-sm text-text-secondary">
                {cost > 0 ? `${algorithm.toUpperCase()} Cost` : 'No Cost Data'}
              </div>
              <div className={`w-full h-1 rounded-full mt-2 ${
                cost > 0 ? 'bg-accent-warning' : 'bg-text-muted'
              }`} />
            </div>
          </div>

          {/* Call to Action */}
          {onGetStarted && (
            <div className="flex flex-col sm:flex-row gap-4 justify-center items-center">
              <button
                onClick={onGetStarted}
                className={`${styleTokens.button.primary} px-8 py-4 text-lg`}
              >
                Get Started
              </button>
              
              <button className={`${styleTokens.button.secondary} px-6 py-4`}>
                View Documentation
              </button>
            </div>
          )}

          {/* Quick Stats */}
          <div className="mt-12 pt-8 border-t border-border-muted">
            <div className="grid grid-cols-2 md:grid-cols-4 gap-6 text-center">
              <div>
                <div className="text-xl font-bold text-accent-primary mb-1">A* & Dijkstra</div>
                <div className="text-sm text-text-muted">Algorithms</div>
              </div>
              
              <div>
                <div className="text-xl font-bold text-accent-success mb-1">Real-time</div>
                <div className="text-sm text-text-muted">Visualization</div>
              </div>
              
              <div>
                <div className="text-xl font-bold text-accent-warning mb-1">Performance</div>
                <div className="text-sm text-text-muted">Analytics</div>
              </div>
              
              <div>
                <div className="text-xl font-bold text-text-primary mb-1">Interactive</div>
                <div className="text-sm text-text-muted">Mapping</div>
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* Bottom fade effect */}
      <div className="absolute bottom-0 left-0 right-0 h-32 bg-gradient-to-t from-bg-primary to-transparent pointer-events-none" />
    </section>
  );
};

export default HeroSection;