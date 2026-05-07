/**
 * src/utils/api.js
 * ----------------
 * Centralised API helper using axios.
 * All calls are relative so they use the proxy defined in package.json
 * during development, and the production base URL in deployment.
 */

import axios from 'axios';

const BASE = process.env.REACT_APP_API_URL || '';

const api = axios.create({
  baseURL: BASE,
  timeout: 8000,
  headers: { 'Content-Type': 'application/json' },
});

/** Fetch the most recent sensor reading */
export const fetchLatest = (deviceId) => {
  const params = deviceId ? { device_id: deviceId } : {};
  return api.get('/api/data/latest', { params }).then(r => r.data);
};

/** Fetch paginated history */
export const fetchHistory = (deviceId, limit = 60, offset = 0) => {
  const params = { limit, offset, ...(deviceId ? { device_id: deviceId } : {}) };
  return api.get('/api/data/history', { params }).then(r => r.data);
};

/** Fetch system status */
export const fetchStatus = () =>
  api.get('/api/status').then(r => r.data);

/** Fetch session summary */
export const fetchSession = (deviceId) => {
  const params = deviceId ? { device_id: deviceId } : {};
  return api.get('/api/session', { params }).then(r => r.data);
};

export default api;
