/**
 * WebSocket Integration Service
 * Real-time communication for simulation events and live system updates
 */

import { apiService } from './api';
import type { WSMessage, SimulationParams, PerformanceMetrics, SystemStatus } from '../types/api';

type MessageHandler = (message: WSMessage) => void;
type ConnectionHandler = (connected: boolean) => void;
type ErrorHandler = (error: string) => void;

class WebSocketService {
  private ws: WebSocket | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 1000;
  private isManuallyDisconnected = false;
  private heartbeatInterval: NodeJS.Timeout | null = null;
  private heartbeatTimeout: NodeJS.Timeout | null = null;
  
  // Event handlers
  private messageHandlers: MessageHandler[] = [];
  private connectionHandlers: ConnectionHandler[] = [];
  private errorHandlers: ErrorHandler[] = [];
  
  /**
   * Connect to WebSocket server
   */
  public connect(): void {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      console.log('WebSocket already connected');
      return;
    }
    
    this.isManuallyDisconnected = false;
    
    try {
      this.ws = apiService.createSimulationWebSocket();
      
      this.ws.onopen = () => {
        console.log('WebSocket connection established');
        this.reconnectAttempts = 0;
        this.reconnectDelay = 1000;
        this.startHeartbeat();
        this.notifyConnectionHandlers(true);
      };
      
      this.ws.onmessage = (event) => {
        try {
          const message: WSMessage = JSON.parse(event.data);
          this.handleMessage(message);
        } catch (error) {
          console.error('Failed to parse WebSocket message:', error);
          this.notifyErrorHandlers('Failed to parse server message');
        }
      };
      
      this.ws.onclose = (event) => {
        console.log('WebSocket connection closed:', event.code, event.reason);
        this.stopHeartbeat();
        this.notifyConnectionHandlers(false);
        
        if (!this.isManuallyDisconnected && event.code !== 1000) {
          this.handleReconnection();
        }
      };
      
      this.ws.onerror = (event) => {
        console.error('WebSocket error:', event);
        this.notifyErrorHandlers('WebSocket connection error');
      };
      
    } catch (error) {
      console.error('Failed to create WebSocket connection:', error);
      this.notifyErrorHandlers('Failed to establish WebSocket connection');
    }
  }
  
  /**
   * Disconnect from WebSocket server
   */
  public disconnect(): void {
    this.isManuallyDisconnected = true;
    this.stopHeartbeat();
    
    if (this.ws) {
      this.ws.close(1000, 'Manual disconnect');
      this.ws = null;
    }
    
    this.reconnectAttempts = 0;
  }
  
  /**
   * Send message to server
   */
  public send(message: any): boolean {
    if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
      console.warn('WebSocket not connected, cannot send message');
      return false;
    }
    
    try {
      this.ws.send(JSON.stringify(message));
      return true;
    } catch (error) {
      console.error('Failed to send WebSocket message:', error);
      this.notifyErrorHandlers('Failed to send message');
      return false;
    }
  }
  
  /**
   * Check if WebSocket is connected
   */
  public isConnected(): boolean {
    return this.ws !== null && this.ws.readyState === WebSocket.OPEN;
  }
  
  /**
   * Get connection state
   */
  public getConnectionState(): 'connecting' | 'open' | 'closing' | 'closed' {
    if (!this.ws) return 'closed';
    
    switch (this.ws.readyState) {
      case WebSocket.CONNECTING:
        return 'connecting';
      case WebSocket.OPEN:
        return 'open';
      case WebSocket.CLOSING:
        return 'closing';
      case WebSocket.CLOSED:
      default:
        return 'closed';
    }
  }
  
  /**
   * Subscribe to messages
   */
  public onMessage(handler: MessageHandler): () => void {
    this.messageHandlers.push(handler);
    
    // Return unsubscribe function
    return () => {
      const index = this.messageHandlers.indexOf(handler);
      if (index > -1) {
        this.messageHandlers.splice(index, 1);
      }
    };
  }
  
  /**
   * Subscribe to connection changes
   */
  public onConnectionChange(handler: ConnectionHandler): () => void {
    this.connectionHandlers.push(handler);
    
    // Return unsubscribe function
    return () => {
      const index = this.connectionHandlers.indexOf(handler);
      if (index > -1) {
        this.connectionHandlers.splice(index, 1);
      }
    };
  }
  
  /**
   * Subscribe to errors
   */
  public onError(handler: ErrorHandler): () => void {
    this.errorHandlers.push(handler);
    
    // Return unsubscribe function
    return () => {
      const index = this.errorHandlers.indexOf(handler);
      if (index > -1) {
        this.errorHandlers.splice(index, 1);
      }
    };
  }
  
  /**
   * Start simulation
   */
  public startSimulation(params: SimulationParams): boolean {
    return this.send({
      type: 'start_simulation',
      data: params,
    });
  }
  
  /**
   * Stop simulation
   */
  public stopSimulation(): boolean {
    return this.send({
      type: 'stop_simulation',
      data: {},
    });
  }
  
  /**
   * Request system status
   */
  public requestStatus(): boolean {
    return this.send({
      type: 'get_status',
      data: {},
    });
  }
  
  /**
   * Request performance metrics
   */
  public requestMetrics(): boolean {
    return this.send({
      type: 'get_metrics',
      data: {},
    });
  }
  
  // Private methods
  
  private handleMessage(message: WSMessage): void {
    this.notifyMessageHandlers(message);
    
    // Handle heartbeat response
    if ((message as any).type === 'pong') {
      this.resetHeartbeatTimeout();
    }
  }
  
  private handleReconnection(): void {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('Max WebSocket reconnection attempts reached');
      this.notifyErrorHandlers('Connection lost - maximum retry attempts exceeded');
      return;
    }
    
    this.reconnectAttempts++;
    
    console.log(`Attempting WebSocket reconnection (${this.reconnectAttempts}/${this.maxReconnectAttempts}) in ${this.reconnectDelay}ms`);
    
    setTimeout(() => {
      if (!this.isManuallyDisconnected) {
        this.connect();
      }
    }, this.reconnectDelay);
    
    // Exponential backoff with jitter
    this.reconnectDelay = Math.min(this.reconnectDelay * 2, 30000) + Math.random() * 1000;
  }
  
  private startHeartbeat(): void {
    this.stopHeartbeat();
    
    // Send ping every 30 seconds
    this.heartbeatInterval = setInterval(() => {
      if (this.ws && this.ws.readyState === WebSocket.OPEN) {
        this.send({ type: 'ping', data: {} });
        
        // Set timeout for pong response
        this.heartbeatTimeout = setTimeout(() => {
          console.warn('WebSocket heartbeat timeout');
          this.ws?.close(1001, 'Heartbeat timeout');
        }, 10000); // 10 second timeout
      }
    }, 30000);
  }
  
  private stopHeartbeat(): void {
    if (this.heartbeatInterval) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
    }
    
    if (this.heartbeatTimeout) {
      clearTimeout(this.heartbeatTimeout);
      this.heartbeatTimeout = null;
    }
  }
  
  private resetHeartbeatTimeout(): void {
    if (this.heartbeatTimeout) {
      clearTimeout(this.heartbeatTimeout);
      this.heartbeatTimeout = null;
    }
  }
  
  private notifyMessageHandlers(message: WSMessage): void {
    this.messageHandlers.forEach(handler => {
      try {
        handler(message);
      } catch (error) {
        console.error('Error in WebSocket message handler:', error);
      }
    });
  }
  
  private notifyConnectionHandlers(connected: boolean): void {
    this.connectionHandlers.forEach(handler => {
      try {
        handler(connected);
      } catch (error) {
        console.error('Error in WebSocket connection handler:', error);
      }
    });
  }
  
  private notifyErrorHandlers(error: string): void {
    this.errorHandlers.forEach(handler => {
      try {
        handler(error);
      } catch (error) {
        console.error('Error in WebSocket error handler:', error);
      }
    });
  }
}

// Create singleton instance
export const wsService = new WebSocketService();

/**
 * React Hook for WebSocket Integration
 * Provides easy integration with React components
 */
export const useWebSocket = () => {
  const connect = () => {
    wsService.connect();
  };
  
  const disconnect = () => {
    wsService.disconnect();
  };
  
  const send = (message: any) => {
    return wsService.send(message);
  };
  
  const isConnected = () => {
    return wsService.isConnected();
  };
  
  const getConnectionState = () => {
    return wsService.getConnectionState();
  };
  
  const onMessage = (handler: MessageHandler) => {
    return wsService.onMessage(handler);
  };
  
  const onConnectionChange = (handler: ConnectionHandler) => {
    return wsService.onConnectionChange(handler);
  };
  
  const onError = (handler: ErrorHandler) => {
    return wsService.onError(handler);
  };
  
  const startSimulation = (params: SimulationParams) => {
    return wsService.startSimulation(params);
  };
  
  const stopSimulation = () => {
    return wsService.stopSimulation();
  };
  
  const requestStatus = () => {
    return wsService.requestStatus();
  };
  
  const requestMetrics = () => {
    return wsService.requestMetrics();
  };
  
  return {
    connect,
    disconnect,
    send,
    isConnected,
    getConnectionState,
    onMessage,
    onConnectionChange,
    onError,
    startSimulation,
    stopSimulation,
    requestStatus,
    requestMetrics,
  };
};

export default wsService;