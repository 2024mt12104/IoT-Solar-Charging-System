/**
 * src/App.jsx
 * -----------
 * Root component.  Polls the backend every 5 seconds and distributes
 * data to all dashboard panels.
 */

import React, { useState, useEffect, useCallback } from 'react';
import { fetchLatest, fetchHistory, fetchStatus, fetchSession } from './utils/api';
import VoltageChart  from './components/VoltageChart';
import CurrentChart  from './components/CurrentChart';
import PowerGauge    from './components/PowerGauge';
import EnergyMeter   from './components/EnergyMeter';
import StatusPanel   from './components/StatusPanel';

const POLL_INTERVAL_MS = 5000;
const HISTORY_LIMIT    = 60;

const styles = {
  wrapper: {
    minHeight: '100vh',
    background: '#0f1117',
    color: '#e2e8f0',
    fontFamily: "'Segoe UI', system-ui, sans-serif",
  },
  header: {
    background: '#1a1f2e',
    borderBottom: '1px solid #2d3748',
    padding: '14px 28px',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
  },
  headerTitle: {
    fontSize: 20,
    fontWeight: 700,
    color: '#90cdf4',
    letterSpacing: '0.01em',
  },
  headerSub: {
    fontSize: 13,
    color: '#718096',
    marginTop: 2,
  },
  main: {
    maxWidth: 1400,
    margin: '0 auto',
    padding: '24px 20px',
  },
  grid2: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(480px, 1fr))',
    gap: 20,
    marginBottom: 20,
  },
  grid3: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))',
    gap: 20,
  },
  error: {
    background: '#742a2a',
    color: '#feb2b2',
    borderRadius: 8,
    padding: '10px 16px',
    marginBottom: 16,
    fontSize: 14,
  },
};

export default function App() {
  const [latest,  setLatest]  = useState(null);
  const [history, setHistory] = useState([]);
  const [status,  setStatus]  = useState(null);
  const [session, setSession] = useState(null);
  const [error,   setError]   = useState(null);
  const [lastPoll, setLastPoll] = useState(null);

  const poll = useCallback(async () => {
    try {
      const [lat, hist, stat, sess] = await Promise.all([
        fetchLatest(),
        fetchHistory(null, HISTORY_LIMIT),
        fetchStatus(),
        fetchSession(),
      ]);
      setLatest(lat);
      setHistory(hist.data || []);
      setStatus(stat);
      setSession(sess);
      setError(null);
      setLastPoll(new Date().toLocaleTimeString());
    } catch (err) {
      setError(err?.response?.data?.error || err.message || 'Connection error');
    }
  }, []);

  useEffect(() => {
    poll();
    const id = setInterval(poll, POLL_INTERVAL_MS);
    return () => clearInterval(id);
  }, [poll]);

  return (
    <div style={styles.wrapper}>
      {/* Header */}
      <header style={styles.header}>
        <div>
          <div style={styles.headerTitle}>
            ☀️ IoT Solar Charging System – Dashboard
          </div>
          <div style={styles.headerSub}>
            Real-time monitoring · STM32F446RE + ESP8266 · BITS Pilani WILP
          </div>
        </div>
        <div style={{ fontSize: 13, color: '#718096', textAlign: 'right' }}>
          Auto-refresh every {POLL_INTERVAL_MS / 1000} s<br />
          {lastPoll && <span>Last update: {lastPoll}</span>}
        </div>
      </header>

      {/* Main content */}
      <main style={styles.main}>
        {error && (
          <div style={styles.error}>
            ⚠️ {error} – Retrying in {POLL_INTERVAL_MS / 1000} s…
          </div>
        )}

        {/* Row 1: Voltage + Current charts */}
        <div style={styles.grid2}>
          <VoltageChart history={history} />
          <CurrentChart history={history} />
        </div>

        {/* Row 2: Power + Energy + Status */}
        <div style={styles.grid3}>
          <PowerGauge latest={latest} history={history} />
          <EnergyMeter latest={latest} />
          <StatusPanel latest={latest} status={status} session={session} />
        </div>
      </main>
    </div>
  );
}
