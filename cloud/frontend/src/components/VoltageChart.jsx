/**
 * src/components/VoltageChart.jsx
 * --------------------------------
 * Line chart showing solar, battery, and supercapacitor voltages over time.
 */

import React from 'react';
import {
  Chart as ChartJS,
  CategoryScale, LinearScale,
  PointElement, LineElement,
  Title, Tooltip, Legend,
} from 'chart.js';
import { Line } from 'react-chartjs-2';

ChartJS.register(
  CategoryScale, LinearScale,
  PointElement, LineElement,
  Title, Tooltip, Legend
);

const card = {
  background: '#1a1f2e',
  borderRadius: 12,
  padding: '16px 20px',
  border: '1px solid #2d3748',
};

const options = {
  responsive: true,
  maintainAspectRatio: true,
  animation: false,
  plugins: {
    legend: { labels: { color: '#a0aec0' } },
    title:  { display: false },
    tooltip: {
      callbacks: {
        label: ctx => `${ctx.dataset.label}: ${(ctx.raw / 1000).toFixed(3)} V`,
      },
    },
  },
  scales: {
    x: {
      ticks: { color: '#718096', maxTicksLimit: 8, maxRotation: 0 },
      grid:  { color: '#2d3748' },
    },
    y: {
      title: { display: true, text: 'Voltage (mV)', color: '#a0aec0' },
      ticks: { color: '#718096' },
      grid:  { color: '#2d3748' },
    },
  },
};

export default function VoltageChart({ history }) {
  const labels = (history || []).map(r =>
    new Date(r.timestamp + 'Z').toLocaleTimeString()
  ).reverse();

  const dataset = (key, label, color) => ({
    label,
    data: [...(history || [])].reverse().map(r => r[key]),
    borderColor:     color,
    backgroundColor: color + '33',
    borderWidth: 2,
    pointRadius: 2,
    tension: 0.3,
  });

  const data = {
    labels,
    datasets: [
      dataset('v_solar',    'Solar Voltage',    '#f6ad55'),
      dataset('v_battery',  'Battery Voltage',  '#68d391'),
      dataset('v_supercap', 'Supercap Voltage', '#76e4f7'),
    ],
  };

  return (
    <div style={card}>
      <h2 style={{ marginBottom: 12, fontSize: 16, color: '#90cdf4' }}>
        Voltage (mV)
      </h2>
      <Line options={options} data={data} />
    </div>
  );
}
