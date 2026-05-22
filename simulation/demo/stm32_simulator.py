# =============================================================================
# Project Title:
#   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
#   SUPERCAPACITOR ENERGY BUFFERING
#
# Author: Ajeesh Kumar R | BITS ID: 2024MT12104
# Programme: M.Tech in Software Systems - Specialization in IoT
# Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division
#
# Academic Purpose Notice:
#   This source code, dataset, documentation, and associated project files are
#   developed solely for academic learning, research, experimentation, and project
#   evaluation purposes under the M.Tech in Software Systems - Specialization in
#   IoT programme at BITS Pilani WILP.
#
# Ownership Declaration:
#   The complete project work, including source code, datasets, documentation,
#   design files, reports, diagrams, and related resources, are the intellectual
#   work of the above-mentioned author unless otherwise referenced.
#
# Usage Restriction:
#   Unauthorized copying, redistribution, commercial usage, or modification of
#   this project material without prior permission from the author is discouraged.
#   This project is intended strictly for educational and research-oriented use.
# =============================================================================
"""
stm32_simulator.py
~~~~~~~~~~~~~~~~~~
Software simulation of the STM32 firmware control loop.

Wraps the ChargingModel to produce a real-time-like stream of JSON
frames matching the UART output format from the STM32, and optionally
POSTs them to the cloud backend so the dashboard can be demoed without
physical hardware.

Usage:
    # Print frames to stdout (hardware-free demo)
    python stm32_simulator.py

    # Send to local backend
    python stm32_simulator.py --post http://localhost:5000/api/data

    # Replay at 10× speed for 30 minutes of simulated time
    python stm32_simulator.py --duration 1800 --speed 10
"""

from __future__ import annotations

import argparse
import json
import random
import sys
import time
import urllib.request
import urllib.error

# Allow importing from sibling directory when run directly
import os
sys.path.insert(0, os.path.dirname(__file__))

from charging_model import ChargingModel, SampleReading


DEFAULT_DEVICE_ID = "solar-charger-sim-001"
DEFAULT_LOCATION  = "Simulation"


def build_json_frame(r: SampleReading,
                     device_id: str = DEFAULT_DEVICE_ID,
                     location:  str = DEFAULT_LOCATION) -> str:
    """Produce a JSON string identical in structure to the ESP8266 upload."""
    payload = {
        "device_id":  device_id,
        "location":   location,
        "v_solar":    r.v_solar,
        "v_battery":  r.v_battery,
        "v_supercap": r.v_supercap,
        "i_input":    r.i_input,
        "i_output":   r.i_output,
        "p_input":    r.p_input,
        "p_output":   r.p_output,
        "e_input":    r.e_input,
        "e_output":   r.e_output,
        "efficiency": r.efficiency,
        "sc_soc":     r.sc_soc,
        "sc_energy":  r.sc_energy,
        "session_s":  r.session_s,
    }
    return json.dumps(payload)


def post_frame(url: str, frame_json: str) -> int:
    """HTTP POST a JSON frame; returns HTTP status code or -1 on error."""
    data = frame_json.encode("utf-8")
    req  = urllib.request.Request(
        url,
        data=data,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=5) as resp:
            return resp.status
    except urllib.error.HTTPError as e:
        return e.code
    except Exception:
        return -1


def run_simulator(duration_s:   int   = 3600,
                  dt_s:         float = 1.0,
                  speed:        float = 1.0,
                  post_url:     str   = "",
                  device_id:    str   = DEFAULT_DEVICE_ID,
                  location:     str   = DEFAULT_LOCATION,
                  seed:         int   = 42,
                  peak_hour:    float = 12.0,
                  cloud_prob:   float = 0.05,
                  bat_init_soc: float = 0.2) -> None:
    """
    Run the simulated firmware loop.

    Args:
        duration_s    Total simulation duration [s]
        dt_s          Time step between frames [s]
        speed         Playback speed multiplier (1.0 = real-time)
        post_url      If set, POST each frame to this URL
        device_id     Device identifier embedded in the payload
        location      Location string embedded in the payload
        seed          Random seed for reproducibility
        peak_hour     Solar irradiance peak hour (0–24)
        cloud_prob    Per-second probability of a cloud event starting
        bat_init_soc  Initial battery state-of-charge [0.0–1.0]
    """
    random.seed(seed)
    model    = ChargingModel(
        peak_hour=peak_hour,
        cloud_prob=cloud_prob,
        bat_initial_soc=bat_init_soc,
    )

    print(f"[STM32 Simulator] Starting: {duration_s} s @ {speed}× speed", flush=True)
    if post_url:
        print(f"[STM32 Simulator] POSTing frames to {post_url}", flush=True)

    sleep_s     = dt_s / speed
    t           = 0.0
    total_sent  = 0
    total_fail  = 0

    while t <= duration_s:
        irr   = model.solar.irradiance(t)
        v_pv  = model.solar.panel_voltage(irr)
        i_pv  = model.solar.panel_current(irr)
        p_pv  = v_pv * i_pv

        from charging_model import DCDC_MIN_VIN, DCDC_EFF, BAT_BULK_V, BAT_CC_CURRENT_A
        v_dcdc = BAT_BULK_V if irr > 0.05 and v_pv >= DCDC_MIN_VIN else 0.0
        i_dcdc = (p_pv * DCDC_EFF) / v_dcdc if v_dcdc > 0 else 0.0

        model.scap.step(i_pv * 0.3, dt_s)
        bat_v = model.battery.step(v_dcdc, i_dcdc, dt_s)

        p_in  = v_pv  * i_pv
        p_out = bat_v * min(i_dcdc, BAT_CC_CURRENT_A)

        model._e_in_mj  += p_in  * dt_s * 1000.0
        model._e_out_mj += p_out * dt_s * 1000.0

        eff = int((p_out / p_in * 100) if p_in > 0.5 else 0)
        eff = max(0, min(100, eff))

        r = SampleReading(
            time_s     = round(t, 1),
            v_solar    = int(v_pv  * 1000),
            v_battery  = int(bat_v * 1000),
            v_supercap = int(model.scap.voltage * 1000),
            i_input    = int(i_pv  * 1000),
            i_output   = int(min(i_dcdc, BAT_CC_CURRENT_A) * 1000),
            p_input    = int(p_in  * 1000),
            p_output   = int(p_out * 1000),
            e_input    = int(model._e_in_mj),
            e_output   = int(model._e_out_mj),
            efficiency = eff,
            sc_soc     = int(model.scap.soc() * 100),
            sc_energy  = int(model.scap.energy_j() * 1000),
            session_s  = int(t),
        )

        frame = build_json_frame(r, device_id, location)

        # Print compact status line
        print(f"t={int(t):5d}s | Vbat={r.v_battery:6d}mV | "
              f"Pout={r.p_output:6d}mW | η={r.efficiency:3d}% | "
              f"ScSoC={r.sc_soc:3d}%", flush=True)

        if post_url:
            code = post_frame(post_url, frame)
            if code in (200, 201):
                total_sent += 1
            else:
                total_fail += 1
                if total_fail <= 3:
                    print(f"  [!] POST failed: HTTP {code}", flush=True)

        t += dt_s
        if sleep_s > 0:
            time.sleep(sleep_s)

    print(f"\n[STM32 Simulator] Done. Frames: {int(t/dt_s)} | "
          f"POSTed OK: {total_sent} | Failed: {total_fail}", flush=True)


def main():
    parser = argparse.ArgumentParser(
        description="STM32 firmware simulator for the IoT Solar Charging System")
    parser.add_argument("--duration",  type=int,   default=3600,
                        help="Simulation duration in seconds (default: 3600)")
    parser.add_argument("--dt",        type=float, default=1.0,
                        help="Time step between frames in seconds (default: 1.0)")
    parser.add_argument("--speed",     type=float, default=1.0,
                        help="Playback speed multiplier (default: 1.0)")
    parser.add_argument("--post",      type=str,   default="",
                        metavar="URL",
                        help="POST frames to this URL, e.g. http://localhost:5000/api/data")
    parser.add_argument("--device-id", type=str,   default=DEFAULT_DEVICE_ID)
    parser.add_argument("--location",  type=str,   default=DEFAULT_LOCATION)
    parser.add_argument("--seed",      type=int,   default=42)
    parser.add_argument("--peak-hour", type=float, default=12.0,
                        help="Hour of peak solar irradiance (0–24)")
    parser.add_argument("--cloud-prob",type=float, default=0.05,
                        help="Per-step probability of a cloud event")
    parser.add_argument("--bat-soc",   type=float, default=0.2,
                        help="Initial battery SoC 0.0–1.0 (default: 0.2)")
    args = parser.parse_args()

    run_simulator(
        duration_s   = args.duration,
        dt_s         = args.dt,
        speed        = args.speed,
        post_url     = args.post,
        device_id    = args.device_id,
        location     = args.location,
        seed         = args.seed,
        peak_hour    = args.peak_hour,
        cloud_prob   = args.cloud_prob,
        bat_init_soc = args.bat_soc,
    )


if __name__ == "__main__":
    main()
