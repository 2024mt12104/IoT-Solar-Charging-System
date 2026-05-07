/**
 * src/components/StatusPanel.jsx
 * --------------------------------
 * Displays system status indicators: connection, device info, session timer.
 */

import React from 'react';

const card = {
  background: '#1a1f2e',
  borderRadius: 12,
  padding: '16px 20px',
  border: '1px solid #2d3748',
};

const badge = (color) => ({
  display: 'inline-block',
  padding: '2px 10px',
  borderRadius: 99,
  background: color,
  fontWeight: 700,
  fontSize: 12,
  marginLeft: 8,
});

function fmtDuration(secs) {
  if (!secs && secs !== 0) return '—';
  const h = Math.floor(secs / 3600);
  const m = Math.floor((secs % 3600) / 60);
  const s = secs % 60;
  return [h, m, s].map(n => String(n).padStart(2, '0')).join(':');
}

export default function StatusPanel({ latest, status, session }) {
  const connected = !!latest;
  const efficiency = latest?.efficiency ?? null;

  let effColor = '#48bb78';           // green
  if (efficiency !== null && efficiency < 60) effColor = '#fc8181';  // red
  else if (efficiency !== null && efficiency < 75) effColor = '#f6ad55'; // orange

  return (
    <div style={card}>
      <h2 style={{ marginBottom: 12, fontSize: 16, color: '#90cdf4' }}>
        System Status
      </h2>
      <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: 14 }}>
        <tbody>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Connection</td>
            <td>
              <span style={badge(connected ? '#276749' : '#742a2a')}>
                {connected ? '● ONLINE' : '● OFFLINE'}
              </span>
            </td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Device ID</td>
            <td style={{ fontFamily: 'monospace' }}>
              {latest?.device_id ?? '—'}
            </td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Location</td>
            <td>{latest?.location ?? '—'}</td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Last Update</td>
            <td style={{ fontFamily: 'monospace' }}>
              {latest?.timestamp
                ? new Date(latest.timestamp + 'Z').toLocaleTimeString()
                : '—'}
            </td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Session Time</td>
            <td style={{ fontFamily: 'monospace' }}>
              {fmtDuration(latest?.session_s)}
            </td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Efficiency</td>
            <td>
              <span style={badge(effColor)}>
                {efficiency !== null ? `${efficiency} %` : '—'}
              </span>
            </td>
          </tr>
          <tr>
            <td style={{ padding: '5px 0', color: '#a0aec0' }}>Supercap SoC</td>
            <td>
              <span style={{ fontFamily: 'monospace' }}>
                {latest?.sc_soc != null ? `${latest.sc_soc} %` : '—'}
              </span>
            </td>
          </tr>
          {session && (
            <>
              <tr>
                <td style={{ padding: '5px 0', color: '#a0aec0' }}>Total Readings</td>
                <td>{session.total_readings ?? '—'}</td>
              </tr>
              <tr>
                <td style={{ padding: '5px 0', color: '#a0aec0' }}>Avg Efficiency</td>
                <td>{session.avg_efficiency_pct != null
                  ? `${Number(session.avg_efficiency_pct).toFixed(1)} %`
                  : '—'}</td>
              </tr>
            </>
          )}
          {status && (
            <tr>
              <td style={{ padding: '5px 0', color: '#a0aec0' }}>Server Uptime</td>
              <td style={{ fontFamily: 'monospace' }}>
                {fmtDuration(status.uptime_s)}
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
}
