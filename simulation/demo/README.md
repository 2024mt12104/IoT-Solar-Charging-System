# Simulation Module – How to Run

## Overview

Software simulation of the IoT Solar Charging System.  Runs without any
physical hardware and produces data identical in format to the real STM32 output.

| Script | Purpose |
|--------|---------|
| `charging_model.py` | Physics model: solar panel, supercapacitor, battery |
| `stm32_simulator.py` | Firmware loop simulator; can POST live frames to the backend |
| `scenario_generator.py` | Batch scenarios (clear sky, cloudy, full cycle, buffered vs unbuffered) |

---

## Prerequisites

```bash
pip install matplotlib numpy     # optional – for plotting
```

No external dependencies required to run the core simulation.

---

## Quick Start

### 1. Generate simulated CSV data

```bash
cd simulation/demo
python charging_model.py
```

Outputs: `simulation/data/simulated_charging_data.csv` (3 601 rows, 1 h at 1 s/sample)

---

### 2. Run the STM32 simulator (print to terminal)

```bash
python stm32_simulator.py --duration 300 --speed 10
```

Options:

| Flag | Default | Description |
|------|---------|-------------|
| `--duration` | 3600 | Simulation duration [s] |
| `--dt` | 1.0 | Time step [s] |
| `--speed` | 1.0 | Playback speed (10 = 10× real-time) |
| `--post URL` | — | POST each frame to this API endpoint |
| `--device-id` | solar-charger-sim-001 | Device identifier |
| `--peak-hour` | 12.0 | Hour of peak irradiance |
| `--cloud-prob` | 0.05 | Cloud event probability |
| `--bat-soc` | 0.2 | Initial battery SoC |

---

### 3. Feed the simulator into the dashboard (full stack demo)

**Terminal 1** – start the backend:

```bash
cd cloud/backend
python app.py
```

**Terminal 2** – start the frontend:

```bash
cd cloud/frontend
npm start
```

**Terminal 3** – run the simulator, posting to the backend:

```bash
cd simulation/demo
python stm32_simulator.py --speed 60 --post http://localhost:5000/api/data
```

Open **http://localhost:3000** to see the dashboard update in real time.

---

### 4. Run batch test scenarios

```bash
cd simulation/demo

# Run all 4 scenarios
python scenario_generator.py --all

# Single scenario
python scenario_generator.py --scenario cloudy_day

# Buffered vs unbuffered comparison
python scenario_generator.py --compare
```

Output CSV files are written to `simulation/data/`.

**Available scenarios:**

| Name | Description |
|------|-------------|
| `clear_sky_morning` | Clear sky, battery starts at 20 % SoC |
| `cloudy_day` | Frequent cloud interruptions |
| `partial_shade` | Moderate, stable irradiance |
| `full_charge_cycle` | Start 5 % → float voltage |

---

## CSV Output Format

All simulation scripts write CSV with these columns:

```
time_s, v_solar_mV, v_battery_mV, v_supercap_mV,
i_input_mA, i_output_mA, p_input_mW, p_output_mW,
e_input_mJ, e_output_mJ, efficiency_pct, sc_soc_pct,
sc_energy_mJ, session_s
```

---

## Physical Parameters (edit in `charging_model.py`)

| Parameter | Value | Description |
|-----------|-------|-------------|
| `SOLAR_VMPP` | 17.5 V | Panel max-power-point voltage |
| `SOLAR_IMPP` | 1.37 A | Panel max-power-point current |
| `SCAP_CAPACITANCE_F` | 50 F | Supercapacitor capacitance |
| `SCAP_VMAX` | 10.8 V | Maximum supercap voltage |
| `DCDC_EFF` | 0.90 | DC-DC converter efficiency |
| `BAT_BULK_V` | 14.4 V | Battery bulk/absorption voltage |
| `BAT_CC_CURRENT_A` | 1.0 A | Constant-current charging rate |

---

*Last updated: May 2026*
