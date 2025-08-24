import { describe, it, expect, beforeEach, vi } from 'vitest';
import api from '../api';
import type { RouteParams } from '../../types/api';

// Mock fetch globally
const mockFetch = vi.fn();
global.fetch = mockFetch;

describe('API Service', () => {
  beforeEach(() => {
    mockFetch.mockClear();
  });

  describe('findRoute', () => {
    it('should find route with valid parameters', async () => {
      const mockResponse = {
        ok: true,
        json: async () => ({
          path: [1, 2, 3, 4],
          cost: 10.5,
          nodesExpanded: 15,
          executionTime: 0.025,
          status: 'success'
        })
      };
      mockFetch.mockResolvedValue(mockResponse);

      const params: RouteParams = {
        source: 1,
        target: 4,
        algo: 'astar',
        metric: 'distance'
      };

      const result = await api.findRoute(params);

      expect(mockFetch).toHaveBeenCalledWith('/api/route/find', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(params),
        signal: expect.any(AbortSignal)
      });
      expect(result.path).toEqual([1, 2, 3, 4]);
      expect(result.cost).toBe(10.5);
    });

    it('should validate route parameters', async () => {
      const invalidParams = {
        source: -1,
        target: 4,
        algo: 'astar',
        metric: 'distance'
      } as RouteParams;

      await expect(api.findRoute(invalidParams)).rejects.toThrow('Invalid route parameters');
    });
  });

  describe('getSystemStatus', () => {
    it('should get system status', async () => {
      const mockResponse = {
        ok: true,
        json: async () => ({
          status: 'online',
          version: '1.0.0',
          uptime: 3600,
          lastUpdate: new Date().toISOString()
        })
      };
      mockFetch.mockResolvedValue(mockResponse);

      const result = await api.getSystemStatus();

      expect(mockFetch).toHaveBeenCalledWith('/api/system/status', {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        },
        signal: expect.any(AbortSignal)
      });
      expect(result.status).toBe('online');
    });

    it('should handle server errors', async () => {
      const mockResponse = {
        ok: false,
        status: 500,
        text: async () => 'Internal Server Error'
      };
      mockFetch.mockResolvedValue(mockResponse);

      await expect(api.getSystemStatus()).rejects.toThrow('API Error: 500 - Internal Server Error');
    });
  });
});