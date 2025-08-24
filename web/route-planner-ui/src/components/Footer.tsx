/**
 * Footer Component
 * Simple footer with credits and information
 */

import { FC } from 'react';
import { styleTokens } from '../styles/tokens';

const Footer: FC = () => {
  const currentYear = new Date().getFullYear();

  return (
    <footer className="bg-bg-surface border-t border-border-default mt-16">
      <div className={`${styleTokens.container} py-8`}>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-8">
          {/* Brand */}
          <div>
            <div className="flex items-center space-x-2 mb-4">
              <div className="w-6 h-6 bg-accent-primary rounded flex items-center justify-center">
                <svg className="w-4 h-4 text-white" fill="currentColor" viewBox="0 0 24 24">
                  <path d="M12 2L13.09 8.26L22 9L13.09 9.74L12 16L10.91 9.74L2 9L10.91 8.26L12 2Z" />
                </svg>
              </div>
              <span className="font-semibold text-text-primary">Route Transit Simulator</span>
            </div>
            <p className={`${styleTokens.text.caption} max-w-xs`}>
              Advanced pathfinding algorithms with real-time visualization and performance analysis.
            </p>
          </div>

          {/* Features */}
          <div>
            <h4 className="font-semibold text-text-primary mb-4">Features</h4>
            <ul className="space-y-2 text-sm text-text-secondary">
              <li>• A* & Dijkstra Algorithms</li>
              <li>• Real-time Visualization</li>
              <li>• Performance Metrics</li>
              <li>• Interactive Mapping</li>
            </ul>
          </div>

          {/* Technology */}
          <div>
            <h4 className="font-semibold text-text-primary mb-4">Technology</h4>
            <ul className="space-y-2 text-sm text-text-secondary">
              <li>• React + TypeScript</li>
              <li>• TailwindCSS</li>
              <li>• Zustand State Management</li>
              <li>• C++ Backend Engine</li>
            </ul>
          </div>
        </div>

        {/* Bottom Section */}
        <div className="border-t border-border-muted mt-8 pt-8 flex flex-col md:flex-row justify-between items-center">
          <p className={`${styleTokens.text.caption} mb-4 md:mb-0`}>
            © {currentYear} Route Transit Simulator. Built for educational and research purposes.
          </p>
          
          <div className="flex items-center space-x-6">
            <a 
              href="#" 
              className="text-text-secondary hover:text-accent-primary transition-colors text-sm"
            >
              Documentation
            </a>
            <a 
              href="#" 
              className="text-text-secondary hover:text-accent-primary transition-colors text-sm"
            >
              Source Code
            </a>
            <a 
              href="#" 
              className="text-text-secondary hover:text-accent-primary transition-colors text-sm"
            >
              Report Issues
            </a>
          </div>
        </div>
      </div>
    </footer>
  );
};

export default Footer;