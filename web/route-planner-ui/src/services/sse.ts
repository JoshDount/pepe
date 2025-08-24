/**
 * Server-Sent Events (SSE) Integration
 * Real-time streaming service for algorithm timeline updates
 */

import { apiService } from './api';
import type { RouteParams, ExpansionStep, SSEEvent } from '../types/api';

class SSEService {
  private eventSource: EventSource | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 1000; // Start with 1 second
  private isManuallyDisconnected = false;

  /**
   * Start streaming algorithm timeline for a route calculation
   */
  public startRouteStreaming(
    params: RouteParams,
    onStep: (step: ExpansionStep) => void,
    onComplete: (data: { totalSteps: number; finalCost: number }) => void,
    onError: (error: string) => void,
    onConnectionChange?: (connected: boolean) => void
  ): void {
    // Close existing connection
    this.stopStreaming();
    
    this.isManuallyDisconnected = false;
    
    try {
      // Create EventSource connection
      this.eventSource = apiService.createRouteStream(params);
      
      // Connection opened
      this.eventSource.onopen = () => {
        console.log('SSE connection established');
        this.reconnectAttempts = 0;
        this.reconnectDelay = 1000;
        onConnectionChange?.(true);
      };
      
      // Handle messages
      this.eventSource.onmessage = (event) => {
        try {
          const data: SSEEvent = JSON.parse(event.data);
          
          switch (data.type) {
            case 'expansion':
              onStep(data.data as ExpansionStep);
              break;
              
            case 'complete':
              onComplete(data.data as { totalSteps: number; finalCost: number });
              this.stopStreaming();
              break;
              
            case 'error':
              const errorData = data.data as { message: string; code: number };
              onError(errorData.message);
              this.stopStreaming();
              break;
              
            default:
              console.warn('Unknown SSE event type:', (data as any).type);
          }
        } catch (parseError) {
          console.error('Failed to parse SSE message:', parseError);
          onError('Failed to parse server message');
        }
      };
      
      // Handle errors
      this.eventSource.onerror = (event) => {
        console.error('SSE connection error:', event);
        onConnectionChange?.(false);
        
        if (!this.isManuallyDisconnected) {
          this.handleReconnection(params, onStep, onComplete, onError, onConnectionChange);
        }
      };
      
    } catch (error) {
      console.error('Failed to create SSE connection:', error);
      onError('Failed to establish streaming connection');
      onConnectionChange?.(false);
    }
  }
  
  /**
   * Stop streaming and close connection
   */
  public stopStreaming(): void {
    this.isManuallyDisconnected = true;
    
    if (this.eventSource) {
      this.eventSource.close();
      this.eventSource = null;
    }
    
    this.reconnectAttempts = 0;
  }
  
  /**
   * Check if streaming is active
   */
  public isStreaming(): boolean {
    return this.eventSource !== null && this.eventSource.readyState === EventSource.OPEN;
  }
  
  /**
   * Get connection state
   */
  public getConnectionState(): 'connecting' | 'open' | 'closed' {
    if (!this.eventSource) return 'closed';
    
    switch (this.eventSource.readyState) {
      case EventSource.CONNECTING:
        return 'connecting';
      case EventSource.OPEN:
        return 'open';
      case EventSource.CLOSED:
      default:
        return 'closed';
    }
  }
  
  /**
   * Handle reconnection with exponential backoff
   */
  private handleReconnection(
    params: RouteParams,
    onStep: (step: ExpansionStep) => void,
    onComplete: (data: { totalSteps: number; finalCost: number }) => void,
    onError: (error: string) => void,
    onConnectionChange?: (connected: boolean) => void
  ): void {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('Max reconnection attempts reached');
      onError('Connection lost - maximum retry attempts exceeded');
      return;
    }
    
    this.reconnectAttempts++;
    
    console.log(`Attempting to reconnect (${this.reconnectAttempts}/${this.maxReconnectAttempts}) in ${this.reconnectDelay}ms`);
    
    setTimeout(() => {
      if (!this.isManuallyDisconnected) {
        this.startRouteStreaming(params, onStep, onComplete, onError, onConnectionChange);
      }
    }, this.reconnectDelay);
    
    // Exponential backoff with jitter
    this.reconnectDelay = Math.min(this.reconnectDelay * 2, 30000) + Math.random() * 1000;
  }
}

// Create singleton instance
export const sseService = new SSEService();

/**
 * React Hook for SSE Route Streaming
 * Provides easy integration with React components
 */
export const useRouteStreaming = () => {
  const startStreaming = (
    params: RouteParams,
    callbacks: {
      onStep: (step: ExpansionStep) => void;
      onComplete: (data: { totalSteps: number; finalCost: number }) => void;
      onError: (error: string) => void;
      onConnectionChange?: (connected: boolean) => void;
    }
  ) => {
    sseService.startRouteStreaming(
      params,
      callbacks.onStep,
      callbacks.onComplete,
      callbacks.onError,
      callbacks.onConnectionChange
    );
  };
  
  const stopStreaming = () => {
    sseService.stopStreaming();
  };
  
  const isStreaming = () => {
    return sseService.isStreaming();
  };
  
  const getConnectionState = () => {
    return sseService.getConnectionState();
  };
  
  return {
    startStreaming,
    stopStreaming,
    isStreaming,
    getConnectionState,
  };
};

export default sseService;