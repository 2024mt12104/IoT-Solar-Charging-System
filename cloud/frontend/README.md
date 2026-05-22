<!---
Project Title: IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING SUPERCAPACITOR ENERGY BUFFERING
Author: Ajeesh Kumar R | BITS ID: 2024MT12104
Programme: M.Tech in Software Systems - Specialization in IoT
Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division

Academic Purpose Notice:
  This document is developed solely for academic learning, research, experimentation,
  and project evaluation purposes under the M.Tech in Software Systems - Specialization
  in IoT programme at BITS Pilani WILP.

Ownership: All project work is the intellectual work of the above-mentioned author unless otherwise referenced.
Usage Restriction: Unauthorized copying, redistribution, or commercial usage without prior permission is discouraged.
--->

# Cloud Frontend – React Dashboard

## Overview

Real-time monitoring dashboard built with **React 18** and **Chart.js**.
Polls the Flask backend every 5 seconds and displays:

| Panel | Component | Data shown |
|-------|-----------|-----------|
| Voltage Chart | `VoltageChart.jsx` | Solar, battery, supercap voltages over time |
| Current Chart | `CurrentChart.jsx` | Input/output current over time |
| Power Gauge | `PowerGauge.jsx` | Instantaneous input/output power + bar chart |
| Energy Meter | `EnergyMeter.jsx` | Cumulative energy, efficiency progress bar |
| Status Panel | `StatusPanel.jsx` | Connection, device info, session timer |

---

## Prerequisites

| Tool | Version |
|------|---------|
| Node.js | ≥ 18.x |
| npm | ≥ 9.x |

---

## Setup & Run

```bash
cd cloud/frontend

# Install dependencies
npm install

# Start development server (requires backend running on port 5000)
npm start
```

The app opens at **http://localhost:3000** and proxies API calls to `http://localhost:5000` (configured in `package.json`).

---

## Production Build

```bash
npm run build
```

Outputs static files to `build/`.  Serve with nginx or any static file server:

```bash
npx serve -s build -l 3000
```

---

## Configuration

Set the backend URL via environment variable:

```bash
# .env.production
REACT_APP_API_URL=https://your-backend.example.com
```

---

## Project Structure

```
cloud/frontend/
├── package.json
├── public/
│   └── index.html          # HTML shell
└── src/
    ├── index.jsx           # React root
    ├── App.jsx             # Root component, polling loop
    ├── utils/
    │   └── api.js          # axios helpers (fetchLatest, fetchHistory, …)
    └── components/
        ├── VoltageChart.jsx
        ├── CurrentChart.jsx
        ├── PowerGauge.jsx
        ├── EnergyMeter.jsx
        └── StatusPanel.jsx
```

---

## Dashboard Screenshot (layout)

```
┌────────────────────────────────────────────────────────────────────┐
│  ☀️ IoT Solar Charging System – Dashboard                          │
│  Real-time monitoring · STM32F446RE + ESP8266 · BITS Pilani WILP   │
├──────────────────────────────┬─────────────────────────────────────┤
│  Voltage Chart (mV)          │  Current Chart (mA)                 │
│  [Solar/Battery/Supercap]    │  [Input/Output]                     │
├──────────────────┬───────────┴──────────────┬───────────────────── ┤
│  Power Gauge     │  Energy Meter            │  Status Panel        │
│  [Input/Output]  │  [Cumul. E, Efficiency]  │  [Device, Session]   │
└──────────────────┴──────────────────────────┴─────────────────────-┘
```

---

*Last updated: May 2026*
