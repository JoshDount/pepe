/**
 * Design Showcase Component
 * Demonstrates the improved professional design system
 */

import { FC } from 'react';
import { styleTokens } from '../styles/tokens';

const DesignShowcase: FC = () => {
  return (
    <div className="min-h-screen bg-bg-default p-6">
      <div className={styleTokens.container}>
        {/* Header Section */}
        <div className="mb-12">
          <h1 className={styleTokens.text.title}>
            Professional Design System
          </h1>
          <p className={styleTokens.text.body}>
            Refined color palette and typography for Route Transit Simulator
          </p>
        </div>

        {/* Color Palette */}
        <div className={styleTokens.card}>
          <h2 className={styleTokens.text.heading}>Color Palette</h2>
          
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mt-6">
            {/* Background Colors */}
            <div>
              <h3 className={styleTokens.text.caption}>Backgrounds</h3>
              <div className="space-y-2 mt-2">
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-bg-default rounded border border-border-default"></div>
                  <span className="text-sm text-text-secondary">Base (#0B0F14)</span>
                </div>
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-bg-surface rounded border border-border-default"></div>
                  <span className="text-sm text-text-secondary">Surface (#12171F)</span>
                </div>
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-bg-elevated rounded border border-border-default"></div>
                  <span className="text-sm text-text-secondary">Elevated (#1A1F28)</span>
                </div>
              </div>
            </div>

            {/* Text Colors */}
            <div>
              <h3 className={styleTokens.text.caption}>Text</h3>
              <div className="space-y-2 mt-2">
                <div className="text-text-primary">Primary (#E5E7EB)</div>
                <div className="text-text-secondary">Secondary (#9CA3AF)</div>
                <div className="text-text-muted">Muted (#6B7280)</div>
              </div>
            </div>

            {/* Accent Colors */}
            <div>
              <h3 className={styleTokens.text.caption}>Accents</h3>
              <div className="space-y-2 mt-2">
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-accent-primary rounded"></div>
                  <span className="text-sm text-text-secondary">Primary (#B91C1C)</span>
                </div>
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-accent-hover rounded"></div>
                  <span className="text-sm text-text-secondary">Hover (#EF4444)</span>
                </div>
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-accent-success rounded"></div>
                  <span className="text-sm text-text-secondary">Success (#10B981)</span>
                </div>
                <div className="flex items-center space-x-3">
                  <div className="w-8 h-8 bg-accent-warning rounded"></div>
                  <span className="text-sm text-text-secondary">Warning (#F59E0B)</span>
                </div>
              </div>
            </div>

            {/* Status Colors */}
            <div>
              <h3 className={styleTokens.text.caption}>Status Indicators</h3>
              <div className="space-y-2 mt-2">
                <div className={`${styleTokens.status.online} flex items-center space-x-2`}>
                  <div className="w-2 h-2 bg-accent-success rounded-full"></div>
                  <span>Online</span>
                </div>
                <div className={`${styleTokens.status.computing} flex items-center space-x-2`}>
                  <div className="w-2 h-2 bg-accent-primary rounded-full"></div>
                  <span>Computing</span>
                </div>
                <div className={`${styleTokens.status.warning} flex items-center space-x-2`}>
                  <div className="w-2 h-2 bg-accent-warning rounded-full"></div>
                  <span>Warning</span>
                </div>
                <div className={`${styleTokens.status.offline} flex items-center space-x-2`}>
                  <div className="w-2 h-2 bg-text-muted rounded-full"></div>
                  <span>Offline</span>
                </div>
              </div>
            </div>
          </div>
        </div>

        {/* Typography */}
        <div className={`${styleTokens.card} mt-8`}>
          <h2 className={styleTokens.text.heading}>Typography Scale</h2>
          
          <div className="space-y-6 mt-6">
            <div>
              <div className={styleTokens.text.title}>Hero Text (60px, 800 weight)</div>
              <span className="text-sm text-text-muted">Used for main headings and hero sections</span>
            </div>
            
            <div>
              <div className={styleTokens.text.subtitle}>Display Text (48px, 800 weight)</div>
              <span className="text-sm text-text-muted">Used for section titles and important headings</span>
            </div>
            
            <div>
              <div className={styleTokens.text.heading}>Heading Text (32px, 600 weight)</div>
              <span className="text-sm text-text-muted">Used for component headings and subsections</span>
            </div>
            
            <div>
              <div className={styleTokens.text.body}>Body Text (16px, 500 weight)</div>
              <span className="text-sm text-text-muted">Used for main content and general text</span>
            </div>
            
            <div>
              <div className={styleTokens.text.caption}>Caption Text (14px, 500 weight)</div>
              <span className="text-sm text-text-muted">Used for labels, captions, and secondary information</span>
            </div>
          </div>
        </div>

        {/* Interactive Elements */}
        <div className={`${styleTokens.card} mt-8`}>
          <h2 className={styleTokens.text.heading}>Interactive Elements</h2>
          
          <div className="grid grid-cols-1 md:grid-cols-2 gap-8 mt-6">
            {/* Buttons */}
            <div>
              <h3 className={styleTokens.text.caption}>Buttons</h3>
              <div className="space-y-4 mt-4">
                <button className={styleTokens.button.primary}>
                  Primary Action
                </button>
                <button className={styleTokens.button.secondary}>
                  Secondary Action
                </button>
                <button className={`${styleTokens.button.primary} opacity-50 cursor-not-allowed`} disabled>
                  Disabled State
                </button>
              </div>
            </div>

            {/* Form Elements */}
            <div>
              <h3 className={styleTokens.text.caption}>Form Elements</h3>
              <div className="space-y-4 mt-4">
                <input 
                  type="text" 
                  placeholder="Enter text here..."
                  className={styleTokens.input}
                />
                <select className={styleTokens.input}>
                  <option>Select an option</option>
                  <option>Dijkstra Algorithm</option>
                  <option>A* Search</option>
                </select>
              </div>
            </div>
          </div>
        </div>

        {/* Professional Icons */}
        <div className={`${styleTokens.card} mt-8`}>
          <h2 className={styleTokens.text.heading}>Professional Icons</h2>
          <p className={styleTokens.text.caption}>Replacing emojis with clean, professional SVG icons</p>
          
          <div className="grid grid-cols-2 md:grid-cols-4 gap-6 mt-6">
            {/* Server Status Icon */}
            <div className="flex flex-col items-center space-y-2">
              <div className="w-12 h-12 bg-accent-success/20 rounded-lg flex items-center justify-center">
                <svg className="w-6 h-6 text-accent-success" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4zM3 10a1 1 0 011-1h6a1 1 0 011 1v6a1 1 0 01-1 1H4a1 1 0 01-1-1v-6zM14 9a1 1 0 00-1 1v6a1 1 0 001 1h2a1 1 0 001-1v-6a1 1 0 00-1-1h-2z" clipRule="evenodd" />
                </svg>
              </div>
              <span className="text-sm text-text-secondary">Server Status</span>
            </div>

            {/* Route Planning Icon */}
            <div className="flex flex-col items-center space-y-2">
              <div className="w-12 h-12 bg-accent-primary/20 rounded-lg flex items-center justify-center">
                <svg className="w-6 h-6 text-accent-primary" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M5.05 4.05a7 7 0 119.9 9.9L10 18.9l-4.95-4.95a7 7 0 010-9.9zM10 11a2 2 0 100-4 2 2 0 000 4z" clipRule="evenodd" />
                </svg>
              </div>
              <span className="text-sm text-text-secondary">Route Planning</span>
            </div>

            {/* Performance Icon */}
            <div className="flex flex-col items-center space-y-2">
              <div className="w-12 h-12 bg-accent-warning/20 rounded-lg flex items-center justify-center">
                <svg className="w-6 h-6 text-accent-warning" fill="currentColor" viewBox="0 0 20 20">
                  <path d="M2 11a1 1 0 011-1h2a1 1 0 011 1v5a1 1 0 01-1 1H3a1 1 0 01-1-1v-5zM8 7a1 1 0 011-1h2a1 1 0 011 1v9a1 1 0 01-1 1H9a1 1 0 01-1-1V7zM14 4a1 1 0 011-1h2a1 1 0 011 1v12a1 1 0 01-1 1h-2a1 1 0 01-1-1V4z" />
                </svg>
              </div>
              <span className="text-sm text-text-secondary">Performance</span>
            </div>

            {/* Settings Icon */}
            <div className="flex flex-col items-center space-y-2">
              <div className="w-12 h-12 bg-text-muted/20 rounded-lg flex items-center justify-center">
                <svg className="w-6 h-6 text-text-muted" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M11.49 3.17c-.38-1.56-2.6-1.56-2.98 0a1.532 1.532 0 01-2.286.948c-1.372-.836-2.942.734-2.106 2.106.54.886.061 2.042-.947 2.287-1.561.379-1.561 2.6 0 2.978a1.532 1.532 0 01.947 2.287c-.836 1.372.734 2.942 2.106 2.106a1.532 1.532 0 012.287.947c.379 1.561 2.6 1.561 2.978 0a1.533 1.533 0 012.287-.947c1.372.836 2.942-.734 2.106-2.106a1.533 1.533 0 01.947-2.287c1.561-.379 1.561-2.6 0-2.978a1.532 1.532 0 01-.947-2.287c.836-1.372-.734-2.942-2.106-2.106a1.532 1.532 0 01-2.287-.947zM10 13a3 3 0 100-6 3 3 0 000 6z" clipRule="evenodd" />
                </svg>
              </div>
              <span className="text-sm text-text-secondary">Configuration</span>
            </div>
          </div>
        </div>

        {/* Layout Examples */}
        <div className={`${styleTokens.card} mt-8`}>
          <h2 className={styleTokens.text.heading}>Layout Components</h2>
          
          <div className="space-y-6 mt-6">
            {/* Card Examples */}
            <div>
              <h3 className={styleTokens.text.caption}>Card Components</h3>
              <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mt-4">
                <div className={styleTokens.card}>
                  <h4 className={styleTokens.text.heading}>Metric Card</h4>
                  <div className="text-2xl font-bold text-accent-primary mt-2">42ms</div>
                  <p className={styleTokens.text.caption}>Execution Time</p>
                </div>
                
                <div className={styleTokens.cardHover}>
                  <h4 className={styleTokens.text.heading}>Interactive Card</h4>
                  <div className="text-2xl font-bold text-accent-success mt-2">1,247</div>
                  <p className={styleTokens.text.caption}>Nodes Processed</p>
                </div>
                
                <div className={styleTokens.card}>
                  <h4 className={styleTokens.text.heading}>Status Card</h4>
                  <div className={`${styleTokens.status.online} flex items-center space-x-2 mt-2`}>
                    <div className="w-3 h-3 bg-accent-success rounded-full"></div>
                    <span>System Online</span>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default DesignShowcase;