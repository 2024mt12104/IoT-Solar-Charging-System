"""
models/sensor_data.py
~~~~~~~~~~~~~~~~~~~~~
Data-access model for sensor readings.

Provides:
  SensorReading – a plain dataclass representing one row
  SensorReadingModel – static methods for CRUD operations
"""

from __future__ import annotations
from dataclasses import dataclass, field, asdict
from datetime import datetime
from typing import Optional, List
from database.db import query_db, execute_db


@dataclass
class SensorReading:
    """Represents one row in the sensor_readings table."""
    id:          Optional[int]  = field(default=None)
    device_id:   str            = "unknown"
    location:    str            = ""
    timestamp:   str            = field(default_factory=lambda: datetime.utcnow().isoformat())
    v_solar:     int            = 0   # mV
    v_battery:   int            = 0   # mV
    v_supercap:  int            = 0   # mV
    i_input:     int            = 0   # mA
    i_output:    int            = 0   # mA
    p_input:     int            = 0   # mW
    p_output:    int            = 0   # mW
    e_input:     int            = 0   # mJ
    e_output:    int            = 0   # mJ
    efficiency:  int            = 0   # %
    sc_soc:      int            = 0   # %
    sc_energy:   int            = 0   # mJ
    session_s:   int            = 0   # s

    @classmethod
    def from_dict(cls, d: dict) -> "SensorReading":
        """Create a SensorReading from a dict (e.g. request JSON)."""
        return cls(
            device_id  = str(d.get("device_id",  "unknown")),
            location   = str(d.get("location",   "")),
            v_solar    = int(d.get("v_solar",    0)),
            v_battery  = int(d.get("v_battery",  0)),
            v_supercap = int(d.get("v_supercap", 0)),
            i_input    = int(d.get("i_input",    0)),
            i_output   = int(d.get("i_output",   0)),
            p_input    = int(d.get("p_input",    0)),
            p_output   = int(d.get("p_output",   0)),
            e_input    = int(d.get("e_input",    0)),
            e_output   = int(d.get("e_output",   0)),
            efficiency = int(d.get("efficiency", 0)),
            sc_soc     = int(d.get("sc_soc",     0)),
            sc_energy  = int(d.get("sc_energy",  0)),
            session_s  = int(d.get("session_s",  0)),
        )

    def to_dict(self) -> dict:
        return asdict(self)


class SensorReadingModel:
    """Static methods for CRUD operations on sensor_readings."""

    INSERT_SQL = """
        INSERT INTO sensor_readings
            (device_id, location, v_solar, v_battery, v_supercap,
             i_input, i_output, p_input, p_output, e_input, e_output,
             efficiency, sc_soc, sc_energy, session_s)
        VALUES
            (:device_id, :location, :v_solar, :v_battery, :v_supercap,
             :i_input, :i_output, :p_input, :p_output, :e_input, :e_output,
             :efficiency, :sc_soc, :sc_energy, :session_s)
    """

    @staticmethod
    def insert(reading: SensorReading) -> int:
        """Insert a new reading and return its row id."""
        d = reading.to_dict()
        d.pop("id", None)
        d.pop("timestamp", None)
        return execute_db(SensorReadingModel.INSERT_SQL, d)

    @staticmethod
    def get_latest(device_id: Optional[str] = None) -> Optional[dict]:
        """Return the most recent reading, optionally filtered by device."""
        if device_id:
            return query_db(
                "SELECT * FROM sensor_readings WHERE device_id=? "
                "ORDER BY id DESC LIMIT 1",
                (device_id,), one=True)
        return query_db(
            "SELECT * FROM sensor_readings ORDER BY id DESC LIMIT 1",
            one=True)

    @staticmethod
    def get_history(device_id: Optional[str] = None,
                    limit: int = 100,
                    offset: int = 0) -> List[dict]:
        """Return paginated history, newest first."""
        if device_id:
            return query_db(
                "SELECT * FROM sensor_readings WHERE device_id=? "
                "ORDER BY id DESC LIMIT ? OFFSET ?",
                (device_id, limit, offset))
        return query_db(
            "SELECT * FROM sensor_readings ORDER BY id DESC LIMIT ? OFFSET ?",
            (limit, offset))

    @staticmethod
    def get_session_summary(device_id: Optional[str] = None) -> dict:
        """Return aggregate statistics for the current/latest session."""
        where = "WHERE device_id=?" if device_id else ""
        args  = (device_id,) if device_id else ()
        row = query_db(
            f"""SELECT
                COUNT(*)              AS total_readings,
                MAX(e_input)          AS total_e_input_mJ,
                MAX(e_output)         AS total_e_output_mJ,
                AVG(efficiency)       AS avg_efficiency_pct,
                MAX(v_solar)          AS peak_v_solar_mV,
                MAX(p_input)          AS peak_p_input_mW,
                MAX(session_s)        AS session_duration_s
            FROM sensor_readings {where}""",
            args, one=True)
        return row or {}
