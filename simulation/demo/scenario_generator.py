"""
scenario_generator.py
~~~~~~~~~~~~~~~~~~~~~
Generates a library of test scenarios for the solar charging simulation.

Scenarios
---------
1. clear_sky_morning   – Full irradiance, battery starts at 20 % SoC
2. cloudy_day          – High cloud probability, frequent interruptions
3. partial_shade       – Medium irradiance cap, stable conditions
4. full_charge_cycle   – Start at 5 %, run until float voltage reached
5. buffered_vs_unbuffered – Side-by-side comparison with/without supercap

Each scenario produces a CSV file in simulation/data/ and returns a
summary dict.

Usage:
    python scenario_generator.py [--scenario <name>] [--all]
"""

from __future__ import annotations

import argparse
import os
import random
import sys
from dataclasses import dataclass
from typing import Dict, Any

sys.path.insert(0, os.path.dirname(__file__))

from charging_model import (
    ChargingModel, SampleReading,
    SCAP_VMIN
)

DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data")


@dataclass
class ScenarioConfig:
    name:           str
    description:    str
    duration_s:     int
    dt_s:           float
    peak_hour:      float
    cloud_prob:     float
    bat_init_soc:   float
    scap_init_v:    float
    seed:           int


SCENARIOS: Dict[str, ScenarioConfig] = {
    "clear_sky_morning": ScenarioConfig(
        name="clear_sky_morning",
        description="Clear sky, peak at noon, battery starts nearly empty",
        duration_s=3600 * 4,
        dt_s=10.0,
        peak_hour=12.0,
        cloud_prob=0.01,
        bat_init_soc=0.20,
        scap_init_v=SCAP_VMIN,
        seed=1,
    ),
    "cloudy_day": ScenarioConfig(
        name="cloudy_day",
        description="Frequent clouds, reduced average irradiance",
        duration_s=3600 * 6,
        dt_s=10.0,
        peak_hour=12.0,
        cloud_prob=0.20,
        bat_init_soc=0.40,
        scap_init_v=SCAP_VMIN + 1.0,
        seed=2,
    ),
    "partial_shade": ScenarioConfig(
        name="partial_shade",
        description="Moderate, stable irradiance (e.g. partial shade location)",
        duration_s=3600 * 3,
        dt_s=10.0,
        peak_hour=11.0,
        cloud_prob=0.08,
        bat_init_soc=0.30,
        scap_init_v=SCAP_VMIN,
        seed=3,
    ),
    "full_charge_cycle": ScenarioConfig(
        name="full_charge_cycle",
        description="Complete charge from 5 % to float voltage",
        duration_s=3600 * 8,
        dt_s=30.0,
        peak_hour=12.0,
        cloud_prob=0.03,
        bat_init_soc=0.05,
        scap_init_v=SCAP_VMIN,
        seed=4,
    ),
}


def run_scenario(cfg: ScenarioConfig) -> Dict[str, Any]:
    """Run a scenario, save CSV, return summary statistics."""
    random.seed(cfg.seed)
    model    = ChargingModel(
        peak_hour=cfg.peak_hour,
        cloud_prob=cfg.cloud_prob,
        bat_initial_soc=cfg.bat_init_soc,
        scap_initial_v=cfg.scap_init_v,
    )
    readings = model.run(duration_s=cfg.duration_s, dt_s=cfg.dt_s)

    # Save CSV
    os.makedirs(DATA_DIR, exist_ok=True)
    csv_path = os.path.join(DATA_DIR, f"{cfg.name}.csv")
    with open(csv_path, "w") as f:
        f.write(SampleReading.csv_header() + "\n")
        for r in readings:
            f.write(r.to_csv_row() + "\n")

    # Summary
    last = readings[-1]
    peak_p = max(r.p_input for r in readings)
    avg_eff = sum(r.efficiency for r in readings if r.p_input > 0) / max(
        1, sum(1 for r in readings if r.p_input > 0))

    summary = {
        "scenario":        cfg.name,
        "description":     cfg.description,
        "samples":         len(readings),
        "duration_s":      cfg.duration_s,
        "final_v_bat_mV":  last.v_battery,
        "final_sc_soc_pct":last.sc_soc,
        "peak_p_in_mW":    peak_p,
        "total_e_in_J":    round(last.e_input  / 1000, 2),
        "total_e_out_J":   round(last.e_output / 1000, 2),
        "avg_efficiency_pct": round(avg_eff, 1),
        "csv_path":        csv_path,
    }
    return summary


def run_buffered_vs_unbuffered() -> Dict[str, Any]:
    """
    Compare system performance with and without supercapacitor buffer.
    Both runs use identical solar/battery conditions.
    """
    cfg_base = ScenarioConfig(
        name="buffered",
        description="With supercapacitor buffer",
        duration_s=3600 * 4,
        dt_s=10.0,
        peak_hour=12.0,
        cloud_prob=0.15,
        bat_init_soc=0.20,
        scap_init_v=SCAP_VMIN,
        seed=99,
    )

    # Run with buffer
    r_buffered   = run_scenario(cfg_base)

    # Run without (set scap_init_v to vmin and prevent charging – simulated
    # by overriding cloud_prob=0.15 but no supercap contribution; the
    # ChargingModel stores 30% of current in scap so the difference is visible)
    cfg_unb = ScenarioConfig(
        name="unbuffered",
        description="Without supercapacitor buffer",
        duration_s=cfg_base.duration_s,
        dt_s=cfg_base.dt_s,
        peak_hour=cfg_base.peak_hour,
        cloud_prob=cfg_base.cloud_prob,
        bat_init_soc=cfg_base.bat_init_soc,
        scap_init_v=SCAP_VMIN,   # same start, but less energy buffering
        seed=cfg_base.seed,
    )
    # Monkey-patch: disable supercap discharge by setting capacity to near-zero
    import charging_model as cm
    _orig = cm.SCAP_CAPACITANCE_F
    cm.SCAP_CAPACITANCE_F = 0.01   # effectively no buffer
    random.seed(cfg_unb.seed)
    r_unbuffered = run_scenario(cfg_unb)
    cm.SCAP_CAPACITANCE_F = _orig   # restore

    comparison = {
        "buffered":   r_buffered,
        "unbuffered": r_unbuffered,
        "delta_e_out_J": round(
            r_buffered["total_e_out_J"] - r_unbuffered["total_e_out_J"], 2),
        "delta_eff_pct": round(
            r_buffered["avg_efficiency_pct"] - r_unbuffered["avg_efficiency_pct"], 1),
    }
    return comparison


def print_summary(s: Dict[str, Any]) -> None:
    print(f"\n{'='*58}")
    print(f"  Scenario : {s['scenario']}")
    print(f"  Desc     : {s['description']}")
    print(f"  Samples  : {s['samples']}  ({s['duration_s']} s)")
    print(f"  Vbat end : {s['final_v_bat_mV']} mV")
    print(f"  SC SoC   : {s['final_sc_soc_pct']} %")
    print(f"  E_in     : {s['total_e_in_J']} J")
    print(f"  E_out    : {s['total_e_out_J']} J")
    print(f"  Avg η    : {s['avg_efficiency_pct']} %")
    print(f"  CSV      : {s['csv_path']}")


def main():
    parser = argparse.ArgumentParser(
        description="Scenario generator for the IoT Solar Charging simulation")
    grp = parser.add_mutually_exclusive_group()
    grp.add_argument("--scenario", choices=list(SCENARIOS.keys()),
                     help="Run a single named scenario")
    grp.add_argument("--all", action="store_true",
                     help="Run all scenarios")
    grp.add_argument("--compare", action="store_true",
                     help="Run buffered vs unbuffered comparison")
    args = parser.parse_args()

    if args.compare:
        result = run_buffered_vs_unbuffered()
        print_summary(result["buffered"])
        print_summary(result["unbuffered"])
        print(f"\n  ΔE_out   : {result['delta_e_out_J']:+.2f} J  (buffered advantage)")
        print(f"  Δη       : {result['delta_eff_pct']:+.1f} %  (buffered advantage)")
    elif args.all:
        for name in SCENARIOS:
            s = run_scenario(SCENARIOS[name])
            print_summary(s)
    elif args.scenario:
        s = run_scenario(SCENARIOS[args.scenario])
        print_summary(s)
    else:
        # Default: run all
        for name in SCENARIOS:
            s = run_scenario(SCENARIOS[name])
            print_summary(s)
        print("\nDone. CSV files written to simulation/data/")


if __name__ == "__main__":
    main()
