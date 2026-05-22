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

# Cloud Backend – Flask REST API

## Overview

Python / Flask REST API that receives sensor data from the ESP8266 IoT gateway
and serves it to the React frontend dashboard.

### API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/api/data` | Receive sensor reading from ESP8266 |
| GET | `/api/data/latest` | Fetch most recent reading |
| GET | `/api/data/history` | Paginated historical data |
| GET | `/api/status` | System health + uptime |
| GET | `/api/session` | Charging session statistics |
| DELETE | `/api/data` | Clear all data (requires `X-Admin-Token` header) |

### Data Flow Architecture

![DataFlow Cloud Dashboard Architecture](../../documentation/images/DataFlow_Cloud_Dashboard_Architecture_Diagram.png)

---

## Setup

### 1. Install dependencies

```bash
cd cloud/backend
python -m venv .venv
source .venv/bin/activate       # Windows: .venv\Scripts\activate
pip install -r requirements.txt
```

### 2. Configure environment (optional)

```bash
export DATABASE_PATH=solar_charger.db   # SQLite file path
export ADMIN_TOKEN=my-secret-token      # Protects DELETE /api/data
export FLASK_DEBUG=1                    # Enable hot-reload for development
export PORT=5000
```

### 3. Run – development

```bash
python app.py
```

### 4. Run – production

```bash
gunicorn -w 4 -b 0.0.0.0:5000 app:app
```

---

## Example Requests

### POST sensor data (from ESP8266 or curl)

```bash
curl -X POST http://localhost:5000/api/data \
     -H "Content-Type: application/json" \
     -d '{
       "device_id":  "solar-charger-001",
       "location":   "Lab-A",
       "v_solar":    12450,
       "v_battery":  13200,
       "v_supercap": 8500,
       "i_input":    1200,
       "i_output":   950,
       "p_input":    14940,
       "p_output":   12540,
       "e_input":    45200,
       "e_output":   37900,
       "efficiency": 83,
       "sc_soc":     72,
       "sc_energy":  18170,
       "session_s":  38
     }'
```

Response `201`:
```json
{"status": "ok", "id": 1}
```

### GET latest reading

```bash
curl http://localhost:5000/api/data/latest
curl http://localhost:5000/api/data/latest?device_id=solar-charger-001
```

### GET history (paginated)

```bash
curl "http://localhost:5000/api/data/history?limit=50&offset=0"
```

### GET system status

```bash
curl http://localhost:5000/api/status
```

### GET session summary

```bash
curl http://localhost:5000/api/session
```

---

## Project Structure

```
cloud/backend/
├── app.py              # Flask application factory + entry point
├── requirements.txt    # Python dependencies
├── solar_charger.db    # SQLite database (auto-created on first run)
├── database/
│   ├── __init__.py
│   └── db.py           # SQLite init, connection helpers
├── models/
│   ├── __init__.py
│   └── sensor_data.py  # SensorReading dataclass + SensorReadingModel CRUD
└── routes/
    ├── __init__.py
    └── data_routes.py  # Flask Blueprint with all API endpoints
```

---

## Database Schema

```sql
CREATE TABLE sensor_readings (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id   TEXT,
    location    TEXT,
    timestamp   DATETIME DEFAULT CURRENT_TIMESTAMP,
    v_solar     INTEGER,   -- Solar voltage [mV]
    v_battery   INTEGER,   -- Battery voltage [mV]
    v_supercap  INTEGER,   -- Supercap voltage [mV]
    i_input     INTEGER,   -- Input current [mA]
    i_output    INTEGER,   -- Output current [mA]
    p_input     INTEGER,   -- Input power [mW]
    p_output    INTEGER,   -- Output power [mW]
    e_input     INTEGER,   -- Cumulative input energy [mJ]
    e_output    INTEGER,   -- Cumulative output energy [mJ]
    efficiency  INTEGER,   -- System efficiency [%]
    sc_soc      INTEGER,   -- Supercap state-of-charge [%]
    sc_energy   INTEGER,   -- Supercap stored energy [mJ]
    session_s   INTEGER    -- Session duration [s]
);
```

---

*Last updated: May 2026*
