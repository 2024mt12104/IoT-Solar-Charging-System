/**
 * src/components/EnergyMeter.jsx
 * --------------------------------
 * Shows cumulative input/output energy and a live efficiency gauge.
 */

import React from 'react';

const card = {
  background: '#1a1f2e',
  borderRadius: 12,
  padding: '16px 20px',
  border: '1px solid #2d3748',
};

function metricRow(label, value, unit, color) {
  return (
    <div key={label} style={{
      display: 'flex', justifyContent: 'space-between', alignItems: 'center',
      padding: '8px 0', borderBottom: '1px solid #2d3748',
    }}>
      <span style={{ color: '#a0aec0', fontSize: 14 }}>{label}</span>
      <span style={{ color, fontWeight: 700, fontFamily: 'monospace', fontSize: 18 }}>
        {value} <span style={{ fontSize: 12, color: '#718096' }}>{unit}</span>
      </span>
    </div>
  );
}

function EfficiencyBar({ pct }) {
  const clamp = Math.max(0, Math.min(100, pct ?? 0));
  const color = clamp >= 75 ? '#68d391' : clamp >= 55 ? '#f6ad55' : '#fc8181';
  return (
    <div style={{ marginTop: 14 }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: 4 }}>
        <span style={{ color: '#a0aec0', fontSize: 14 }}>System Efficiency</span>
        <span style={{ color, fontWeight: 700 }}>{clamp} %</span>
      </div>
      <div style={{ background: '#2d3748', borderRadius: 99, height: 10 }}>
        <div style={{
          width: `${clamp}%`,
          height: '100%',
          background: color,
          borderRadius: 99,
          transition: 'width 0.5s ease',
        }} />
      </div>
    </div>
  );
}

export default function EnergyMeter({ latest }) {
  const eIn   = latest?.e_input  != null ? (latest.e_input  / 1000).toFixed(1) : '—';
  const eOut  = latest?.e_output != null ? (latest.e_output / 1000).toFixed(1) : '—';
  const scE   = latest?.sc_energy != null ? (latest.sc_energy / 1000).toFixed(1) : '—';
  const eff   = latest?.efficiency ?? null;

  return (
    <div style={card}>
      <h2 style={{ marginBottom: 12, fontSize: 16, color: '#90cdf4' }}>
        Energy Meter
      </h2>
      {metricRow('Input Energy',      eIn,  'J',  '#f6ad55')}
      {metricRow('Output Energy',     eOut, 'J',  '#68d391')}
      {metricRow('Supercap Energy',   scE,  'J',  '#76e4f7')}
      <EfficiencyBar pct={eff} />
    </div>
  );
}
