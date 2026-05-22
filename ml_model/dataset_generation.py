"""
Synthetic Dataset Generation for IoT Solar Charging System
===========================================================
Generates realistic charging data comparing TWO system configurations:
  1. Non-Buffered Charging: Direct solar panel → battery (no energy buffer)
  2. Supercapacitor-Buffered Charging: Solar panel → supercapacitor → battery

The supercapacitor acts as an energy buffer that:
  - Smooths out intermittent solar energy (clouds, shading)
  - Provides burst current during high-demand charging phases
  - Reduces effective internal resistance losses
  - Maintains charging continuity during brief irradiance drops

Output: charging_dataset.csv with 1000 samples, 14 features + 3 target columns
Compatible with: scikit-learn, XGBoost, Linear Regression, Jupyter Notebooks
"""

import numpy as np
import pandas as pd

# Set random seed for REPRODUCIBILITY — same dataset generated every run
np.random.seed(42)

N_SAMPLES = 1000  # Number of simulated charging sessions

# =============================================================================
# FEATURE GENERATION — Realistic ranges based on actual hardware specifications
# =============================================================================

# --- Time of Day (hours) ---
# Solar charging occurs during daylight hours (5 AM to 7 PM)
# Uniform distribution: equal probability of starting charge at any daylight hour
time_of_day = np.random.uniform(5, 19, N_SAMPLES)

# --- Cloud Cover (0-100%) ---
# Beta(2,5) distribution: skewed towards clearer skies (most days are partly clear)
# This models typical weather patterns where fully overcast days are less common
cloud_cover = np.random.beta(2, 5, N_SAMPLES) * 100

# --- Solar Irradiance (W/m²) ---
# Models the solar energy hitting the panel surface
# Depends on: time of day (peaks at solar noon) and cloud cover (attenuates energy)
# Formula: Sinusoidal daily curve × cloud attenuation factor + measurement noise
solar_peak = 1000 * np.sin(np.pi * (time_of_day - 5) / 14)  # Peak ~1000 W/m² at noon
solar_irradiance = np.clip(
    solar_peak * (1 - cloud_cover / 120) + np.random.normal(0, 50, N_SAMPLES),
    50, 1100  # Min 50 W/m² (heavy clouds), Max 1100 W/m² (clear noon + reflection)
)

# --- Battery Capacity (mAh) ---
# Common Li-ion battery sizes used in IoT/portable solar systems
battery_capacity = np.random.choice([2000, 3000, 4000, 5000, 6000], N_SAMPLES)

# --- Battery Age (charge/discharge cycles) ---
# Exponential distribution: most batteries are relatively new, few are very old
# Affects internal resistance and efficiency
battery_age = np.random.exponential(200, N_SAMPLES).astype(int)
battery_age = np.clip(battery_age, 0, 1500)  # Max 1500 cycles (typical Li-ion lifespan)

# --- Battery Internal Resistance (Ohms) ---
# Increases with age: fresh battery ~50mΩ, aged battery up to 250mΩ
# Higher resistance → more energy lost as heat → longer charging time
battery_internal_resistance = (
    0.05 +                                          # Base resistance (new battery)
    (battery_age / 1500) * 0.15 +                   # Age degradation component
    np.random.normal(0, 0.01, N_SAMPLES)            # Manufacturing variation
)
battery_internal_resistance = np.clip(battery_internal_resistance, 0.03, 0.25)

# --- State of Charge at start (SOC %) ---
# The battery's charge level when charging begins
# Lower SOC → more energy needed → longer charging time
soc = np.random.uniform(5, 85, N_SAMPLES)  # 5% to 85% (never fully empty or full at start)

# --- Temperature (°C) ---
# Ambient temperature follows daily pattern (warmer midday, cooler morning/evening)
# Affects battery chemistry and charging efficiency
temperature = (
    20 +                                            # Base temperature
    10 * np.sin(np.pi * (time_of_day - 6) / 12) +  # Daily thermal cycle
    np.random.normal(0, 5, N_SAMPLES)               # Weather variation
)
temperature = np.clip(temperature, -5, 55)  # Operating range of Li-ion batteries

# --- Charging Efficiency (%) ---
# How much of the input solar energy actually charges the battery
# Affected by: temperature (optimal at 25°C) and battery age (degradation)
temp_factor = 1 - 0.005 * np.abs(temperature - 25)  # Drops 0.5% per °C away from 25°C
age_factor = 1 - (battery_age / 1500) * 0.15         # Up to 15% drop at end of life
charging_efficiency = np.clip(
    85 * temp_factor * age_factor + np.random.normal(0, 3, N_SAMPLES),
    50, 98  # Min 50% (very degraded), Max 98% (ideal conditions)
)

# --- Supercapacitor Voltage (V) ---
# The voltage across the supercapacitor energy buffer (2.0V – 5.5V range)
# Higher solar irradiance → supercap charges higher → more buffered energy available
supercap_voltage = (
    2.5 +                                           # Minimum operating voltage
    (solar_irradiance / 1100) * 3.0 +               # Charges proportional to solar input
    np.random.normal(0, 0.2, N_SAMPLES)             # Measurement noise
)
supercap_voltage = np.clip(supercap_voltage, 2.0, 5.5)

# --- Battery Voltage (V) ---
# Terminal voltage during charging — correlates with SOC (higher SOC → higher voltage)
# Li-ion typical range: 2.8V (empty) to 4.2V (full)
voltage = 3.0 + (soc / 100) * 1.2 + np.random.normal(0, 0.05, N_SAMPLES)
voltage = np.clip(voltage, 2.8, 4.25)

# --- Charging Current (A) ---
# Determined by available solar power and PWM duty cycle
# Higher irradiance → more current available from solar panel
base_current = (solar_irradiance / 1100) * 2.5  # Max 2.5A at peak irradiance
current = np.clip(base_current + np.random.normal(0, 0.1, N_SAMPLES), 0.1, 3.0)

# --- PWM Duty Cycle (%) ---
# STM32 PWM signal that controls the charging MOSFET gate
# Higher duty cycle → more time conducting → more current flows to battery
pwm_duty_cycle = np.clip(
    (current / 3.0) * 100 + np.random.normal(0, 5, N_SAMPLES),
    10, 100  # Min 10% (trickle charge), Max 100% (full power)
)

# --- Load Connected (W) ---
# Parasitic power draw from connected IoT devices during charging
# Many sessions have no load (0W), some have sensors/MCU drawing power
load_connected = np.random.choice([0, 0, 0, 0.5, 1.0, 1.5, 2.0, 3.0], N_SAMPLES)

# =============================================================================
# TARGET VARIABLE COMPUTATION
# =============================================================================
# We compute charging time for BOTH configurations using physics-based formulas:
#   Charging Time = Energy_Needed / Effective_Power
#   Energy_Needed = Capacity × (100% - SOC%) × Voltage  [in mWh]
#   Effective_Power = (I × V × η) - Load  [in W]
# =============================================================================

# --- Energy required to reach full charge (mWh) ---
energy_needed_mWh = battery_capacity * (100 - soc) / 100 * voltage  # mWh

# =============================================================================
# SCENARIO 1: NON-BUFFERED CHARGING (Direct Solar → Battery)
# =============================================================================
# Without a supercapacitor buffer:
# - Charging current fluctuates directly with solar irradiance
# - Cloud transients cause charging interruptions
# - Full internal resistance losses apply
# - No smoothing of power delivery
# =============================================================================

# Effective power without buffering (Watts)
effective_power_non_buffered = (current * voltage * charging_efficiency / 100) - load_connected
effective_power_non_buffered = np.clip(effective_power_non_buffered, 0.1, None)

# Base charging time (minutes) = Energy / Power × conversion
non_buffered_time = (energy_needed_mWh / effective_power_non_buffered) * 60 / 1000

# Apply degradation factors for non-buffered system:
# 1. Internal resistance losses (I²R heating wastes energy)
non_buffered_time *= (1 + battery_internal_resistance * 2.5)

# 2. Solar intermittency penalty: without buffer, clouds cause stop-start charging
#    which adds overhead (converter restart, MPPT re-acquisition)
intermittency_penalty = 1 + (cloud_cover / 100) * 0.15  # Up to 15% penalty at 100% clouds
non_buffered_time *= intermittency_penalty

# 3. Temperature penalty: extreme temps slow down charging
temp_penalty = 1 + 0.003 * np.abs(temperature - 25)  # 0.3% per °C away from optimal
non_buffered_time *= temp_penalty

# Add realistic noise (sensor measurement uncertainty, environmental variation)
non_buffered_time += np.random.normal(0, 5, N_SAMPLES)
non_buffered_time = np.clip(non_buffered_time, 10, 600)  # 10 min to 10 hours

# =============================================================================
# SCENARIO 2: SUPERCAPACITOR-BUFFERED CHARGING (Solar → Supercap → Battery)
# =============================================================================
# With a supercapacitor energy buffer:
# - Supercap absorbs solar energy continuously, delivers smoothly to battery
# - Eliminates charging interruptions during brief cloud events
# - Provides burst current capability (reduces I²R losses)
# - Maintains optimal charging current even when solar fluctuates
# - Reduces effective internal resistance impact (lower peak currents)
# =============================================================================

# Effective power WITH buffering — supercap provides smoother, more efficient delivery
# The buffer allows maintaining higher average power even during irradiance dips
effective_power_buffered = (current * voltage * charging_efficiency / 100) - load_connected
effective_power_buffered = np.clip(effective_power_buffered, 0.1, None)

# Base charging time with buffer
buffered_time = (energy_needed_mWh / effective_power_buffered) * 60 / 1000

# Apply REDUCED degradation factors for buffered system:

# 1. Reduced internal resistance impact — supercap delivers optimal current profile
#    (lower peak current = less I²R loss, factor reduced from 2.5 to 1.5)
buffered_time *= (1 + battery_internal_resistance * 1.5)

# 2. Greatly reduced intermittency penalty — supercap smooths out cloud transients
#    Buffer absorbs energy during sun, delivers during shade (90% reduction in penalty)
buffered_intermittency = 1 + (cloud_cover / 100) * 0.03  # Only 3% penalty vs 15%
buffered_time *= buffered_intermittency

# 3. Supercapacitor voltage contribution — higher supercap voltage means more stored
#    energy available for continuous charging (normalized 0-1 scale from voltage range)
supercap_benefit = (supercap_voltage - 2.0) / 3.5  # 0 at 2.0V, 1.0 at 5.5V
buffered_time *= (1 - supercap_benefit * 0.12)  # Up to 12% reduction from buffer energy

# 4. Reduced temperature penalty — supercap handles burst demands, battery charges gently
temp_penalty_buffered = 1 + 0.002 * np.abs(temperature - 25)  # 0.2% vs 0.3%
buffered_time *= temp_penalty_buffered

# Add noise (slightly less noise — buffered system is more stable)
buffered_time += np.random.normal(0, 3, N_SAMPLES)
buffered_time = np.clip(buffered_time, 8, 550)  # Slightly better bounds

# =============================================================================
# COMPUTE IMPROVEMENT PERCENTAGE
# =============================================================================
# Improvement = (Non_Buffered - Buffered) / Non_Buffered × 100%
# Positive value means the buffered system charges FASTER (which is expected)
# =============================================================================

improvement_percent = ((non_buffered_time - buffered_time) / non_buffered_time) * 100
# Clip to realistic range: buffer should help 5-35% typically
improvement_percent = np.clip(improvement_percent, 2, 40)

# Recalculate buffered time to be consistent with the clipped improvement
buffered_time = non_buffered_time * (1 - improvement_percent / 100)

# =============================================================================
# CREATE FINAL DATAFRAME
# =============================================================================

df = pd.DataFrame({
    # --- Input Features (14 sensor parameters) ---
    'Voltage_V': np.round(voltage, 3),
    'Current_A': np.round(current, 3),
    'Temperature_C': np.round(temperature, 2),
    'PWM_Duty_Cycle_Percent': np.round(pwm_duty_cycle, 2),
    'SOC_Percent': np.round(soc, 2),
    'Supercapacitor_Voltage_V': np.round(supercap_voltage, 3),
    'Solar_Irradiance_Wm2': np.round(solar_irradiance, 2),
    'Battery_Capacity_mAh': battery_capacity,
    'Battery_Internal_Resistance_Ohm': np.round(battery_internal_resistance, 4),
    'Charging_Efficiency_Percent': np.round(charging_efficiency, 2),
    'Time_of_Day_Hour': np.round(time_of_day, 2),
    'Cloud_Cover_Percent': np.round(cloud_cover, 2),
    'Battery_Age_Cycles': battery_age,
    'Load_Connected_W': load_connected,
    # --- Target Variables (3 outputs) ---
    'Non_Buffered_Charging_Time_Min': np.round(non_buffered_time, 2),
    'Buffered_Charging_Time_Min': np.round(buffered_time, 2),
    'Improvement_Percent': np.round(improvement_percent, 2)
})

# =============================================================================
# EXPORT TO CSV
# =============================================================================

df.to_csv('ml_model/charging_dataset.csv', index=False)

# --- Print Summary ---
print("=" * 70)
print("  IoT SOLAR CHARGING SYSTEM — DATASET GENERATION COMPLETE")
print("=" * 70)
print(f"\n  Samples:  {len(df)} charging sessions")
print(f"  Features: 14 input parameters")
print(f"  Targets:  3 (Non-Buffered Time, Buffered Time, Improvement %)")
print(f"  Output:   ml_model/charging_dataset.csv")
print(f"\n{'─' * 70}")
print(f"  NON-BUFFERED Charging Time:  {non_buffered_time.mean():.1f} ± {non_buffered_time.std():.1f} min (mean ± std)")
print(f"  BUFFERED Charging Time:      {buffered_time.mean():.1f} ± {buffered_time.std():.1f} min (mean ± std)")
print(f"  IMPROVEMENT:                 {improvement_percent.mean():.1f} ± {improvement_percent.std():.1f} %")
print(f"{'─' * 70}")
print(f"\n  Dataset ready for: Linear Regression, Random Forest, XGBoost,")
print(f"  scikit-learn workflows, Jupyter Notebook, and Databricks.")
print(f"\n{df.describe().to_string()}")