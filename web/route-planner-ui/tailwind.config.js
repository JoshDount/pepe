/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        // Background colors - Professional dark theme
        'bg': {
          'default': '#0B0F14',  // Base background
          'surface': '#12171F',  // Surface background
          'elevated': '#1A1F28', // Elevated surfaces
          'muted': '#0F1419',    // Muted backgrounds
        },
        
        // Text colors
        'text': {
          'primary': '#E5E7EB',   // Primary text
          'secondary': '#9CA3AF', // Secondary text
          'muted': '#6B7280',     // Muted text
        },
        
        // Accent colors
        'accent': {
          'primary': '#B91C1C',   // Primary red accent
          'hover': '#EF4444',     // Hover state
          'success': '#10B981',   // Success green
          'warning': '#F59E0B',   // Warning amber
          'error': '#EF4444',     // Error red
        },
        
        // Border colors
        'border': {
          'default': 'rgba(127, 29, 29, 0.2)', // Subtle red border
          'muted': 'rgba(156, 163, 175, 0.1)',  // Very subtle border
          'focus': '#B91C1C',                    // Focus border
        },
      },
      fontFamily: {
        'sans': ['Inter', 'system-ui', 'sans-serif'],
      },
      fontSize: {
        'sm': '14px',     // Small text
        'base': '16px',   // Base text
        'lg': '20px',     // Large text
        'xl': '32px',     // Extra large
        '2xl': '48px',    // Display
        '3xl': '60px',    // Hero
      },
      fontWeight: {
        'medium': '500',    // Medium weight
        'semibold': '600',  // Semibold
        'bold': '800',      // Bold
      },
      borderRadius: {
        'DEFAULT': '12px', // Standard radius
        'lg': '16px',      // Large radius
      },
      boxShadow: {
        'soft': '0 4px 16px rgba(0, 0, 0, 0.2)',      // Soft shadow
        'elevated': '0 8px 32px rgba(0, 0, 0, 0.25)', // Elevated shadow
        'glow': '0 0 20px rgba(185, 28, 28, 0.3)',    // Red glow
      },
      maxWidth: {
        'container': '1280px', // Container width as specified
      },
      animation: {
        'pulse-slow': 'pulse 2s infinite',
        'fade-in': 'fadeIn 0.3s ease-in-out',
      },
      keyframes: {
        fadeIn: {
          '0%': { opacity: '0', transform: 'translateY(10px)' },
          '100%': { opacity: '1', transform: 'translateY(0)' },
        },
      },
    },
  },
  plugins: [],
}