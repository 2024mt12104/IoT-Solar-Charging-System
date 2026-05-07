"""
charging_model.py
~~~~~~~~~~~~~~~~~
Physics-based model of the battery + supercapacitor charging system.

Models:
  - Solar irradiance (time-of-day, cloud events)
  - DC-DC converter with configurable efficiency
  - Supercapacitor charge/discharge dynamics
  - Lead-acid battery CC-CV charging
  - Per-sample energy calculations matching the STM32 firmware formulas

Usage:
    from charging_model import ChargingModel
    model = ChargingModel()
    readings = model.run(duration_s=3600, dt_s=1)
    for r in readings:
        print(r)
"""

from __future__ import annotations

import math
import random
from dataclasses import dataclass, field
from typing import List, Optional


# ---------------------------------------------------------------------------
# System parameters  (match hardware / firmware constants)
# ---------------------------------------------------------------------------

# Battery (12 V sealed lead-acid, 7 Ah)
BAT_NOMINAL_V        = 12.0    # V
BAT_BULK_V           = 14.4    # V  (end of CC phase)
BAT_FLOAT_V          = 13.8    # V  (CV float)
BAT_INTERNAL_R       = 0.15    # Ω
BAT_CAPACITY_AH      = 7.0     # Ah
BAT_CC_CURRENT_A     = 1.0     # A  (0.1 C rate)

# Supercapacitor (50 F, 4-cell series → 10.8 V max)
SCAP_CAPACITANCE_F   = 50.0    # F
SCAP_VMAX            = 10.8    # V
SCAP_VMIN            = 2.0     # V
SCAP_ESR             = 0.02    # Ω  (equivalent series resistance)

# DC-DC converter
DCDC_EFF             = 0.90    # 90 % conversion efficiency
DCDC_MIN_VIN         = 5.0     # V  (minimum input to regulate)

# Solar panel (simulated)
SOLAR_VOC            = 21.0    # V  open-circuit
SOLAR_ISC            = 1.5     # A  short-circuit
SOLAR_VMPP           = 17.5    # V  max-power-point
SOLAR_IMPP           = 1.37    # A  max-power-point


@dataclass
class SampleReading:
    """One simulated sensor sample – matches the STM32 JSON frame fields."""
    time_s:       float = 0.0
    v_solar:      int   = 0    # mV
    v_battery:    int   = 0    # mV
    v_supercap:   int   = 0    # mV
    i_input:      int   = 0    # mA
    i_output:     int   = 0    # mA
    p_input:      int   = 0    # mW
    p_output:     int   = 0    # mW
    e_input:      int   = 0    # mJ (cumulative)
    e_output:     int   = 0    # mJ (cumulative)
    efficiency:   int   = 0    # %
    sc_soc:       int   = 0    # %
    sc_energy:    int   = 0    # mJ
    session_s:    int   = 0    # s

    def to_csv_row(self) -> str:
        return (f"{self.time_s:.1f},{self.v_solar},{self.v_battery},"
                f"{self.v_supercap},{self.i_input},{self.i_output},"
                f"{self.p_input},{self.p_output},{self.e_input},"
                f"{self.e_output},{self.efficiency},{self.sc_soc},"
                f"{self.sc_energy},{self.session_s}")

    @staticmethod
    def csv_header() -> str:
        return ("time_s,v_solar_mV,v_battery_mV,v_supercap_mV,"
                "i_input_mA,i_output_mA,p_input_mW,p_output_mW,"
                "e_input_mJ,e_output_mJ,efficiency_pct,sc_soc_pct,"
                "sc_energy_mJ,session_s")


class SolarModel:
    """Simple solar irradiance model with optional cloud events."""

    def __init__(self, peak_hour: float = 12.0, cloud_prob: float = 0.05):
        self.peak_hour  = peak_hour
        self.cloud_prob = cloud_prob
        self._cloud_until: float = 0.0
        self._cloud_factor: float = 1.0

    def irradiance(self, time_s: float) -> float:
        """Return irradiance [0.0–1.0] at the given elapsed time."""
        hour = (time_s / 3600.0) % 24.0

        # Gaussian bell centred on peak_hour
        base = math.exp(-0.5 * ((hour - self.peak_hour) / 3.5) ** 2)
        base = max(0.0, base)

        # Occasional cloud events
        if time_s >= self._cloud_until:
            if random.random() < self.cloud_prob:
                self._cloud_until  = time_s + random.uniform(60, 600)
                self._cloud_factor = random.uniform(0.1, 0.5)
            else:
                self._cloud_factor = 1.0

        if time_s < self._cloud_until:
            base *= self._cloud_factor

        return base

    def panel_voltage(self, irr: float) -> float:
        """Voc shifts slightly with irradiance; Vmpp tracks."""
        return SOLAR_VMPP * (0.7 + 0.3 * irr) if irr > 0.01 else 0.0

    def panel_current(self, irr: float) -> float:
        return SOLAR_IMPP * irr


class SupercapModel:
    """Discrete-time supercapacitor model."""

    def __init__(self, initial_v: float = SCAP_VMIN):
        self.voltage = initial_v

    def soc(self) -> float:
        """State of charge [0.0–1.0] based on energy."""
        v2    = self.voltage ** 2
        vmin2 = SCAP_VMIN ** 2
        vmax2 = SCAP_VMAX ** 2
        return max(0.0, min(1.0, (v2 - vmin2) / (vmax2 - vmin2)))

    def energy_j(self) -> float:
        return 0.5 * SCAP_CAPACITANCE_F * self.voltage ** 2

    def step(self, current_a: float, dt_s: float) -> float:
        """Update voltage given charge/discharge current; return new voltage."""
        dv = current_a * dt_s / SCAP_CAPACITANCE_F
        self.voltage = max(0.0, min(SCAP_VMAX, self.voltage + dv))
        return self.voltage


class BatteryModel:
    """Lead-acid battery CC-CV charging model."""

    def __init__(self, initial_soc: float = 0.3):
        self.soc     = max(0.0, min(1.0, initial_soc))
        # Approximate OCV = 11.5 V at 0 % SoC, 12.8 V at 100 %
        self.voltage = 11.5 + 1.3 * self.soc

    def step(self, v_charger: float, i_charger_a: float, dt_s: float) -> float:
        """Simulate one time step; update SoC; return terminal voltage."""
        if v_charger < self.voltage:
            return self.voltage

        # CC phase: charger delivers CC_CURRENT; clamp to CV when near bulk_V
        i = min(i_charger_a, BAT_CC_CURRENT_A)

        if self.voltage < BAT_BULK_V:
            # CC – constant current
            dq_ah   = i * dt_s / 3600.0
            self.soc = min(1.0, self.soc + dq_ah / BAT_CAPACITY_AH)
        else:
            # CV – taper current
            i = max(0.0, (BAT_BULK_V - self.voltage) / BAT_INTERNAL_R)
            dq_ah   = i * dt_s / 3600.0
            self.soc = min(1.0, self.soc + dq_ah / BAT_CAPACITY_AH)

        self.voltage = 11.5 + 1.3 * self.soc + i * BAT_INTERNAL_R
        return self.voltage


class ChargingModel:
    """
    Integrate the solar, supercapacitor, and battery models over time.

    The simulation mirrors the control logic in the STM32 firmware:
    - Solar panel → DC-DC converter → supercapacitor buffer → battery
    - When supercap SoC > 15 %, it supplements the DC-DC output to the battery
    - MPPT is implicitly applied via solar panel MPP voltage tracking
    """

    def __init__(self,
                 peak_hour: float = 12.0,
                 cloud_prob: float = 0.05,
                 bat_initial_soc: float = 0.3,
                 scap_initial_v: float = SCAP_VMIN):
        self.solar   = SolarModel(peak_hour, cloud_prob)
        self.scap    = SupercapModel(scap_initial_v)
        self.battery = BatteryModel(bat_initial_soc)
        self._e_in_mj  = 0.0
        self._e_out_mj = 0.0

    def run(self,
            duration_s: int = 3600,
            dt_s: float = 1.0,
            start_offset_s: float = 0.0) -> List[SampleReading]:
        """
        Run the simulation and return a list of SampleReading objects.

        Args:
            duration_s       Total simulation time [s]
            dt_s             Time step [s]
            start_offset_s   Elapsed-time offset for irradiance model [s]
        """
        readings: List[SampleReading] = []
        t = 0.0
        prev_p_in = prev_p_out = 0.0

        while t <= duration_s:
            irr   = self.solar.irradiance(start_offset_s + t)
            v_pv  = self.solar.panel_voltage(irr)
            i_pv  = self.solar.panel_current(irr)
            p_pv  = v_pv * i_pv  # W

            # DC-DC: convert Vpv to target charging voltage
            v_dcdc = BAT_BULK_V if irr > 0.05 and v_pv >= DCDC_MIN_VIN else 0.0
            i_dcdc = (p_pv * DCDC_EFF) / v_dcdc if v_dcdc > 0 else 0.0

            # Supercapacitor charging/discharging
            sc_charge_current = i_pv * 0.3  # 30 % of panel current charges supercap
            self.scap.step(sc_charge_current, dt_s)

            # Battery charging
            bat_v = self.battery.step(v_dcdc, i_dcdc, dt_s)

            # Power
            p_in  = v_pv * i_pv
            p_out = bat_v * min(i_dcdc, BAT_CC_CURRENT_A)

            # Trapezoidal energy integration (mJ)
            self._e_in_mj  += (prev_p_in  + p_in)  * dt_s / 2.0 * 1000.0
            self._e_out_mj += (prev_p_out + p_out) * dt_s / 2.0 * 1000.0
            prev_p_in, prev_p_out = p_in, p_out

            eff = int((p_out / p_in * 100) if p_in > 0.5 else 0)
            eff = max(0, min(100, eff))

            r = SampleReading(
                time_s     = round(t, 1),
                v_solar    = int(v_pv  * 1000),
                v_battery  = int(bat_v * 1000),
                v_supercap = int(self.scap.voltage * 1000),
                i_input    = int(i_pv  * 1000),
                i_output   = int(min(i_dcdc, BAT_CC_CURRENT_A) * 1000),
                p_input    = int(p_in  * 1000),
                p_output   = int(p_out * 1000),
                e_input    = int(self._e_in_mj),
                e_output   = int(self._e_out_mj),
                efficiency = eff,
                sc_soc     = int(self.scap.soc() * 100),
                sc_energy  = int(self.scap.energy_j() * 1000),
                session_s  = int(t),
            )
            readings.append(r)
            t += dt_s

        return readings


if __name__ == "__main__":
    import sys, os

    print("Running ChargingModel demo (4 h at 10 s resolution, starting at 9 AM)…")
    random.seed(42)
    model    = ChargingModel(peak_hour=12.0, cloud_prob=0.05, bat_initial_soc=0.2)
    # start_offset_s = 9 h → irradiance rises through morning peak at noon
    readings = model.run(duration_s=14400, dt_s=10.0, start_offset_s=9 * 3600)

    out_path = os.path.join(
        os.path.dirname(__file__), "..", "data", "simulated_charging_data.csv")
    os.makedirs(os.path.dirname(out_path), exist_ok=True)

    with open(out_path, "w") as f:
        f.write(SampleReading.csv_header() + "\n")
        for r in readings:
            f.write(r.to_csv_row() + "\n")

    print(f"Saved {len(readings)} rows → {os.path.abspath(out_path)}")
    last = readings[-1]
    print(f"Final state: Vbat={last.v_battery} mV | SoC={last.sc_soc}% | η={last.efficiency}%")
