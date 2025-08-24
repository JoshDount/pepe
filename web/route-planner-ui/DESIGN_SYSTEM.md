# Route Transit Simulator - Design System

## Overview

This document outlines the professional design system implemented for the Route Transit Simulator frontend. The system emphasizes professionalism, clarity, and accessibility with a cohesive dark theme.

## Color Palette

### Background Colors
- **Base**: `#0B0F14` - Primary background color
- **Surface**: `#12171F` - Cards, panels, and elevated surfaces  
- **Elevated**: `#1A1F28` - Modal overlays and highest elevation
- **Muted**: `#0F1419` - Subtle background variations

### Text Colors
- **Primary**: `#E5E7EB` - Main content text
- **Secondary**: `#9CA3AF` - Supporting text and labels
- **Muted**: `#6B7280` - Captions and less important text

### Accent Colors
- **Primary**: `#B91C1C` - Main brand color, CTAs, active states
- **Hover**: `#EF4444` - Hover states for primary elements
- **Success**: `#10B981` - Success indicators, positive actions
- **Warning**: `#F59E0B` - Warnings, alerts, caution states
- **Error**: `#EF4444` - Error states and destructive actions

### Border Colors
- **Default**: `rgba(127, 29, 29, 0.2)` - Subtle red-tinted borders
- **Muted**: `rgba(156, 163, 175, 0.1)` - Very subtle neutral borders
- **Focus**: `#B91C1C` - Focus indicators and active borders

## Typography

### Font Family
- **Primary**: Inter, system-ui, sans-serif

### Font Sizes
- **Small**: 14px - Labels, captions, secondary information
- **Base**: 16px - Body text, standard content
- **Large**: 20px - Subheadings, important text
- **Extra Large**: 32px - Section headings
- **Display**: 48px - Page titles, hero text
- **Hero**: 60px - Main headings, landing page titles

### Font Weights
- **Medium**: 500 - Standard text weight
- **Semibold**: 600 - Emphasized text, headings
- **Bold**: 800 - Strong emphasis, hero text

## Spacing & Layout

### Container Width
- **Max Width**: 1280px - Maximum content width
- **Padding**: 24px horizontal - Standard container padding

### Border Radius
- **Standard**: 12px - Cards, buttons, form elements
- **Large**: 16px - Larger components, modals

### Shadows
- **Soft**: `0 4px 16px rgba(0, 0, 0, 0.2)` - Standard card shadow
- **Elevated**: `0 8px 32px rgba(0, 0, 0, 0.25)` - Modal and dropdown shadows
- **Glow**: `0 0 20px rgba(185, 28, 28, 0.3)` - Focus and accent shadows

## Component Styles

### Buttons
```css
/* Primary Button */
.btn-primary {
  background: #B91C1C;
  color: white;
  border: 1px solid rgba(127, 29, 29, 0.2);
  border-radius: 12px;
  font-weight: 600;
  transition: background-color 0.2s ease;
}

.btn-primary:hover {
  background: #EF4444;
}

/* Secondary Button */
.btn-secondary {
  background: transparent;
  color: #E5E7EB;
  border: 1px solid rgba(127, 29, 29, 0.2);
  border-radius: 12px;
  font-weight: 500;
  transition: background-color 0.2s ease;
}

.btn-secondary:hover {
  background: #1A1F28;
}
```

### Cards
```css
.card {
  background: #12171F;
  border: 1px solid rgba(127, 29, 29, 0.2);
  border-radius: 12px;
  padding: 24px;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.2);
}

.card-hover {
  transition: all 0.2s ease;
}

.card-hover:hover {
  transform: translateY(-4px);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
}
```

### Form Elements
```css
.input-field {
  background: #12171F;
  border: 1px solid rgba(127, 29, 29, 0.2);
  border-radius: 12px;
  color: #E5E7EB;
  padding: 8px 12px;
}

.input-field:focus {
  border-color: #B91C1C;
  box-shadow: 0 0 0 2px rgba(185, 28, 28, 0.5);
  outline: none;
}

.input-field::placeholder {
  color: #9CA3AF;
}
```

## Status Indicators

### System Status
- **Online**: Green dot with success color
- **Computing**: Animated red dot with pulse effect
- **Warning**: Amber dot for attention states
- **Offline**: Gray dot for inactive states

### Visual Examples
```tsx
// Online Status
<div className="flex items-center space-x-2 text-accent-success">
  <div className="w-2 h-2 bg-accent-success rounded-full"></div>
  <span>Online</span>
</div>

// Computing Status  
<div className="flex items-center space-x-2 text-accent-primary">
  <div className="w-2 h-2 bg-accent-primary rounded-full animate-pulse"></div>
  <span>Computing</span>
</div>
```

## Professional Icons

All emojis have been replaced with professional SVG icons:

### Common Icons
- **Server Status**: Database/server icon
- **Route Planning**: Location/navigation icon
- **Performance**: Bar chart/analytics icon
- **Configuration**: Settings/gear icon
- **Success**: Check circle icon
- **Warning**: Exclamation triangle icon
- **Error**: X circle icon
- **Information**: Info circle icon

### Icon Implementation
```tsx
// Example: Server Status Icon
<div className="w-12 h-12 bg-accent-success/20 rounded-lg flex items-center justify-center">
  <svg className="w-6 h-6 text-accent-success" fill="currentColor" viewBox="0 0 20 20">
    <path fillRule="evenodd" d="M3 4a1 1 0 011-1h12a1 1 0 011 1v2a1 1 0 01-1 1H4a1 1 0 01-1-1V4z..." />
  </svg>
</div>
```

## Usage Guidelines

### Design Tokens
Import and use the centralized design tokens:

```tsx
import { styleTokens } from '../styles/tokens';

// Typography
<h1 className={styleTokens.text.title}>Page Title</h1>
<p className={styleTokens.text.body}>Body content</p>

// Components
<div className={styleTokens.card}>Card content</div>
<button className={styleTokens.button.primary}>Action</button>

// Layout
<div className={styleTokens.container}>Page content</div>
```

### Tailwind Configuration
The design system is fully integrated with Tailwind CSS:

```tsx
// Background colors
<div className="bg-bg-surface">...</div>

// Text colors  
<span className="text-text-primary">...</span>

// Accent colors
<button className="bg-accent-primary hover:bg-accent-hover">...</button>

// Borders
<div className="border border-border-default">...</div>
```

## Accessibility

### Focus States
All interactive elements include proper focus indicators:
- Visible focus rings using the accent color
- Sufficient color contrast ratios
- Keyboard navigation support

### Color Contrast
All color combinations meet WCAG AA standards:
- Text on background: 4.5:1+ contrast ratio
- Interactive elements: 3:1+ contrast ratio
- Focus indicators: clearly visible

### Semantic HTML
Components use proper semantic HTML elements and ARIA attributes where needed.

## File Structure

```
src/
├── styles/
│   └── tokens.ts          # Centralized design tokens
├── components/
│   ├── DesignShowcase.tsx # Design system demonstration
│   ├── Header.tsx         # Updated with new design system
│   ├── MetricsGrid.tsx    # Professional metric displays
│   └── ...
└── tailwind.config.js     # Tailwind configuration
```

## Migration Notes

### Changes Made
1. **Color Palette**: Refined to exact specifications
2. **Typography**: Simplified to essential weights and sizes
3. **Icons**: Replaced all emojis with professional SVG icons
4. **Components**: Updated all components to use new tokens
5. **Documentation**: Added comprehensive design system docs

### Breaking Changes
- Some utility class names have changed (old `bg-bg-primary` → new `bg-bg-default`)
- Typography scale has been simplified
- Border radius values have been adjusted

### Migration Steps
1. Update import statements to use new design tokens
2. Replace emoji references with professional icons
3. Update class names to match new Tailwind configuration
4. Test all components for visual consistency

This design system provides a solid foundation for building professional, accessible, and maintainable user interfaces for the Route Transit Simulator.