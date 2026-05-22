/*
 * =============================================================================
 * Project Title:
 *   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
 *   SUPERCAPACITOR ENERGY BUFFERING
 *
 * Author: Ajeesh Kumar R | BITS ID: 2024MT12104
 * Programme: M.Tech in Software Systems - Specialization in IoT
 * Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division
 *
 * Academic Purpose Notice:
 *   This source code, dataset, documentation, and associated project files are
 *   developed solely for academic learning, research, experimentation, and project
 *   evaluation purposes under the M.Tech in Software Systems - Specialization in
 *   IoT programme at BITS Pilani WILP.
 *
 * Ownership Declaration:
 *   The complete project work, including source code, datasets, documentation,
 *   design files, reports, diagrams, and related resources, are the intellectual
 *   work of the above-mentioned author unless otherwise referenced.
 *
 * Usage Restriction:
 *   Unauthorized copying, redistribution, commercial usage, or modification of
 *   this project material without prior permission from the author is discouraged.
 *   This project is intended strictly for educational and research-oriented use.
 * =============================================================================
 */

/**
 * src/components/PowerGauge.jsx
 * -------------------------------
 * Displays instantaneous input/output power as large metric cards
 * and a bar chart comparing them.
 */

import React from 'react';
import {
  Chart as ChartJS,
  CategoryScale, LinearScale,
  BarElement, Title, Tooltip, Legend,
} from 'chart.js';
import { Bar } from 'react-chartjs-2';

ChartJS.register(
  CategoryScale, LinearScale,
  BarElement, Title, Tooltip, Legend
);

const outerCard = {
  background: '#1a1f2e',
  borderRadius: 12,
  padding: '16px 20px',
  border: '1px solid #2d3748',
};

const metricBox = (accent) => ({
  flex: 1,
  background: '#0f1117',
  borderRadius: 8,
  padding: '14px 18px',
  borderLeft: `4px solid ${accent}`,
  textAlign: 'center',
});

const options = {
  responsive: true,
  animation: false,
  plugins: {
    legend: { display: false },
    tooltip: {
      callbacks: { label: ctx => `${ctx.raw} mW` },
    },
  },
  scales: {
    x: { ticks: { color: '#718096' }, grid: { color: '#2d3748' } },
    y: {
      title: { display: true, text: 'Power (mW)', color: '#a0aec0' },
      ticks: { color: '#718096' },
      grid:  { color: '#2d3748' },
    },
  },
};

export default function PowerGauge({ latest, history }) {
  const pIn  = latest?.p_input  ?? 0;
  const pOut = latest?.p_output ?? 0;

  const labels = (history || []).map(r =>
    new Date(r.timestamp + 'Z').toLocaleTimeString()
  ).reverse();

  const chartData = {
    labels,
    datasets: [
      {
        label: 'Input Power (mW)',
        data: [...(history || [])].reverse().map(r => r.p_input),
        backgroundColor: '#f6ad5599',
        borderColor:     '#f6ad55',
        borderWidth: 1,
      },
      {
        label: 'Output Power (mW)',
        data: [...(history || [])].reverse().map(r => r.p_output),
        backgroundColor: '#68d39199',
        borderColor:     '#68d391',
        borderWidth: 1,
      },
    ],
  };

  return (
    <div style={outerCard}>
      <h2 style={{ marginBottom: 12, fontSize: 16, color: '#90cdf4' }}>
        Power (mW)
      </h2>
      <div style={{ display: 'flex', gap: 12, marginBottom: 16 }}>
        <div style={metricBox('#f6ad55')}>
          <div style={{ fontSize: 12, color: '#a0aec0' }}>Input Power</div>
          <div style={{ fontSize: 28, fontWeight: 700, color: '#f6ad55' }}>{pIn}</div>
          <div style={{ fontSize: 11, color: '#718096' }}>mW</div>
        </div>
        <div style={metricBox('#68d391')}>
          <div style={{ fontSize: 12, color: '#a0aec0' }}>Output Power</div>
          <div style={{ fontSize: 28, fontWeight: 700, color: '#68d391' }}>{pOut}</div>
          <div style={{ fontSize: 11, color: '#718096' }}>mW</div>
        </div>
      </div>
      <Bar options={options} data={chartData} />
    </div>
  );
}
