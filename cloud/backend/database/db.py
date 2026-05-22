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
database/db.py
~~~~~~~~~~~~~~
SQLite database initialisation and helper utilities.

The database stores sensor readings in a single table:
  sensor_readings(id, device_id, timestamp, v_solar, v_battery, v_supercap,
                  i_input, i_output, p_input, p_output, e_input, e_output,
                  efficiency, sc_soc, sc_energy, session_s, location)

Usage:
    from database.db import get_db, init_db
    init_db(app)
"""

import sqlite3
import os
from flask import g

# Default database path – can be overridden via DATABASE_PATH env var
DEFAULT_DB_PATH = os.path.join(os.path.dirname(__file__), "..", "solar_charger.db")
DATABASE = os.environ.get("DATABASE_PATH", DEFAULT_DB_PATH)

# DDL for the main readings table
SCHEMA_SQL = """
CREATE TABLE IF NOT EXISTS sensor_readings (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id   TEXT    NOT NULL DEFAULT 'unknown',
    location    TEXT    NOT NULL DEFAULT '',
    timestamp   DATETIME DEFAULT CURRENT_TIMESTAMP,
    v_solar     INTEGER NOT NULL DEFAULT 0,   -- Solar voltage [mV]
    v_battery   INTEGER NOT NULL DEFAULT 0,   -- Battery voltage [mV]
    v_supercap  INTEGER NOT NULL DEFAULT 0,   -- Supercap voltage [mV]
    i_input     INTEGER NOT NULL DEFAULT 0,   -- Input current [mA]
    i_output    INTEGER NOT NULL DEFAULT 0,   -- Output current [mA]
    p_input     INTEGER NOT NULL DEFAULT 0,   -- Input power [mW]
    p_output    INTEGER NOT NULL DEFAULT 0,   -- Output power [mW]
    e_input     INTEGER NOT NULL DEFAULT 0,   -- Cumulative input energy [mJ]
    e_output    INTEGER NOT NULL DEFAULT 0,   -- Cumulative output energy [mJ]
    efficiency  INTEGER NOT NULL DEFAULT 0,   -- Efficiency [%]
    sc_soc      INTEGER NOT NULL DEFAULT 0,   -- Supercap SoC [%]
    sc_energy   INTEGER NOT NULL DEFAULT 0,   -- Supercap stored energy [mJ]
    session_s   INTEGER NOT NULL DEFAULT 0    -- Session duration [s]
);

CREATE INDEX IF NOT EXISTS idx_readings_timestamp ON sensor_readings(timestamp);
CREATE INDEX IF NOT EXISTS idx_readings_device    ON sensor_readings(device_id);
"""


def get_db():
    """Return the database connection for the current Flask request context."""
    db = getattr(g, "_database", None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
        db.row_factory = sqlite3.Row   # rows behave like dicts
        db.execute("PRAGMA journal_mode=WAL")
    return db


def close_db(exception=None):
    """Close the database connection at the end of the request."""
    db = getattr(g, "_database", None)
    if db is not None:
        db.close()


def init_db(app):
    """Create tables and indexes; register teardown handler with Flask app."""
    # Create tables using an application context
    with app.app_context():
        db = sqlite3.connect(DATABASE)
        db.executescript(SCHEMA_SQL)
        db.commit()
        db.close()

    app.teardown_appcontext(close_db)


def query_db(query, args=(), one=False):
    """Execute a SELECT query and return results as a list of dicts."""
    cur = get_db().execute(query, args)
    rv = cur.fetchall()
    cur.close()
    return (dict(rv[0]) if rv else None) if one else [dict(row) for row in rv]


def execute_db(query, args=()):
    """Execute an INSERT/UPDATE/DELETE and commit; return lastrowid."""
    db = get_db()
    cur = db.execute(query, args)
    db.commit()
    return cur.lastrowid
