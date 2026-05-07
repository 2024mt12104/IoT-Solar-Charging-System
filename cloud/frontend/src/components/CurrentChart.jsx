/**
 * src/components/CurrentChart.jsx
 * ---------------------------------
 * Line chart for input and output current over time.
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
    tooltip: {
      callbacks: {
        label: ctx => `${ctx.dataset.label}: ${ctx.raw} mA`,
      },
    },
  },
  scales: {
    x: {
      ticks: { color: '#718096', maxTicksLimit: 8, maxRotation: 0 },
      grid:  { color: '#2d3748' },
    },
    y: {
      title: { display: true, text: 'Current (mA)', color: '#a0aec0' },
      ticks: { color: '#718096' },
      grid:  { color: '#2d3748' },
    },
  },
};

export default function CurrentChart({ history }) {
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
      dataset('i_input',  'Input Current',  '#f687b3'),
      dataset('i_output', 'Output Current', '#9f7aea'),
    ],
  };

  return (
    <div style={card}>
      <h2 style={{ marginBottom: 12, fontSize: 16, color: '#90cdf4' }}>
        Current (mA)
      </h2>
      <Line options={options} data={data} />
    </div>
  );
}
