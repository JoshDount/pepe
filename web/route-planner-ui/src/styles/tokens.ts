/**
 * Design Tokens for Route Planning Interface
 * Centralized styling constants following the dark theme requirements
 */

export const colors = {
  // Background colors
  bg: {
    default: '#0B0F14',  // Base background
    surface: '#12171F',  // Surface background
    elevated: '#1A1F28', // Elevated surfaces
    muted: '#0F1419',    // Muted backgrounds
  },
  
  // Text colors
  text: {
    primary: '#E5E7EB',   // Primary text
    secondary: '#9CA3AF', // Secondary text
    muted: '#6B7280',     // Muted text
    caption: '#9CA3AF',   // Caption text
  },
  
  // Accent colors
  accent: {
    primary: '#B91C1C',   // Primary red accent
    hover: '#EF4444',     // Hover state
    success: '#10B981',   // Success green
    warning: '#F59E0B',   // Warning amber
    error: '#EF4444',     // Error red
  },
  
  // Border colors
  border: {
    default: 'rgba(127, 29, 29, 0.2)', // Subtle red border
    muted: 'rgba(156, 163, 175, 0.1)',  // Very subtle border
    focus: '#B91C1C',                    // Focus border
  },
  
  // Status colors
  status: {
    online: '#10B981',    // Success green
    offline: '#6B7280',   // Muted gray
    computing: '#B91C1C', // Primary red
    warning: '#F59E0B',   // Warning amber
  },
} as const;

export const typography = {
  fontFamily: {
    primary: "'Inter', system-ui, sans-serif",
  },
  
  fontSize: {
    sm: '14px',     // Small text
    base: '16px',   // Base text
    lg: '20px',     // Large text
    xl: '32px',     // Extra large
    '2xl': '48px',  // Display
    '3xl': '60px',  // Hero
  },
  
  fontWeight: {
    medium: '500',    // Medium weight
    semibold: '600',  // Semibold
    bold: '800',      // Bold
  },
  
  lineHeight: {
    tight: '1.25',
    normal: '1.5',
    relaxed: '1.625',
  },
} as const;

export const spacing = {
  xs: '4px',
  sm: '8px',
  md: '16px',
  lg: '24px',
  xl: '32px',
  '2xl': '48px',
  '3xl': '64px',
  '4xl': '96px',
} as const;

export const borderRadius = {
  md: '12px',    // Standard radius
  lg: '16px',    // Large radius
  full: '9999px', // Fully rounded
} as const;

export const shadows = {
  soft: '0 4px 16px rgba(0, 0, 0, 0.2)',      // Soft shadow
  elevated: '0 8px 32px rgba(0, 0, 0, 0.25)', // Elevated shadow
  glow: '0 0 20px rgba(185, 28, 28, 0.3)',    // Red glow
} as const;

export const breakpoints = {
  sm: '640px',
  md: '768px',
  lg: '1024px',
  xl: '1280px',
  '2xl': '1536px',
} as const;

export const layout = {
  container: {
    maxWidth: '1280px',  // Container width as specified
    padding: '0 24px',
  },
  header: {
    height: '64px',
  },
  sidebar: {
    width: '280px',
  },
} as const;

export const animation = {
  duration: {
    fast: '150ms',
    normal: '250ms',
    slow: '350ms',
  },
  easing: {
    ease: 'ease',
    easeIn: 'ease-in',
    easeOut: 'ease-out',
    easeInOut: 'ease-in-out',
  },
} as const;

// Compound style tokens for common UI patterns
export const styleTokens = {
  // Container styles
  container: 'max-w-[1280px] mx-auto px-6',
  
  // Card styles
  card: 'bg-[#12171F] rounded-lg p-6 border border-[rgba(127,29,29,0.2)] shadow-[0_4px_16px_rgba(0,0,0,0.2)]',
  cardHover: 'bg-[#12171F] rounded-lg p-6 border border-[rgba(127,29,29,0.2)] shadow-[0_4px_16px_rgba(0,0,0,0.2)] hover:shadow-[0_8px_32px_rgba(0,0,0,0.25)] transition-all duration-200 hover:-translate-y-1',
  
  // Button styles
  button: {
    primary: 'bg-[#B91C1C] hover:bg-[#EF4444] text-white font-semibold px-4 py-2 rounded-lg border border-[rgba(127,29,29,0.2)] transition-colors',
    secondary: 'bg-transparent hover:bg-[#1A1F28] text-[#E5E7EB] font-medium px-4 py-2 rounded-lg border border-[rgba(127,29,29,0.2)] transition-colors',
  },
  
  // Input styles
  input: 'bg-[#12171F] border border-[rgba(127,29,29,0.2)] rounded-lg px-3 py-2 text-[#E5E7EB] placeholder:text-[#9CA3AF] focus:border-[#B91C1C] focus:ring-2 focus:ring-[#B91C1C] focus:ring-opacity-50',
  
  // Text styles
  text: {
    title: 'text-[60px] font-[800] text-[#E5E7EB] leading-tight',
    subtitle: 'text-[48px] font-[800] text-[#E5E7EB] leading-tight',
    heading: 'text-[32px] font-[600] text-[#E5E7EB]',
    body: 'text-[16px] font-[500] text-[#E5E7EB]',
    caption: 'text-[14px] font-[500] text-[#9CA3AF]',
    muted: 'text-[14px] font-[500] text-[#6B7280]',
  },
  
  // Status indicator styles
  status: {
    online: 'text-[#10B981]',
    offline: 'text-[#6B7280]',
    computing: 'text-[#B91C1C] animate-pulse',
    warning: 'text-[#F59E0B]',
  },
  
  // Layout styles
  section: 'py-12',
  grid: {
    '2col': 'grid grid-cols-1 md:grid-cols-2 gap-6',
    '3col': 'grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 gap-6',
    '4col': 'grid grid-cols-1 md:grid-cols-2 xl:grid-cols-4 gap-6',
  },
  
  // Focus styles for accessibility
  focus: 'focus:outline-none focus:ring-2 focus:ring-[#B91C1C] focus:ring-opacity-50',
} as const;

// Type definitions for TypeScript support
export type ColorToken = keyof typeof colors;
export type TypographyToken = keyof typeof typography;
export type SpacingToken = keyof typeof spacing;
export type StyleToken = keyof typeof styleTokens;

// Helper function to get nested color values
export const getColor = (path: string): string => {
  const parts = path.split('.');
  let value: any = colors;
  
  for (const part of parts) {
    value = value[part];
    if (value === undefined) {
      console.warn(`Color token '${path}' not found`);
      return colors.text.primary;
    }
  }
  
  return value;
};

// Export default theme object
export const theme = {
  colors,
  typography,
  spacing,
  borderRadius,
  shadows,
  breakpoints,
  layout,
  animation,
  styleTokens,
} as const;

export default theme;