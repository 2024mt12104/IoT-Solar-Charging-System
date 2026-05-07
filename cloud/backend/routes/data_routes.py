"""
routes/data_routes.py
~~~~~~~~~~~~~~~~~~~~~
REST API Blueprint for sensor data endpoints.

Endpoints
---------
POST   /api/data           – Receive a new sensor reading from ESP8266
GET    /api/data/latest    – Fetch the most recent reading
GET    /api/data/history   – Paginated historical data
GET    /api/status         – System health / uptime
GET    /api/session        – Aggregated session statistics
DELETE /api/data           – Clear all stored readings (admin/test)
"""

from flask import Blueprint, request, jsonify, current_app
from models.sensor_data import SensorReading, SensorReadingModel

data_bp = Blueprint("data", __name__, url_prefix="/api")


# ---------------------------------------------------------------------------
# POST /api/data
# ---------------------------------------------------------------------------
@data_bp.route("/data", methods=["POST"])
def receive_data():
    """Accept a JSON sensor reading from the ESP8266 gateway."""
    payload = request.get_json(silent=True)
    if not payload:
        return jsonify({"error": "Invalid or missing JSON body"}), 400

    try:
        reading = SensorReading.from_dict(payload)
    except (ValueError, TypeError):
        return jsonify({"error": "Data validation error: invalid field type or value"}), 422

    row_id = SensorReadingModel.insert(reading)
    return jsonify({"status": "ok", "id": row_id}), 201


# ---------------------------------------------------------------------------
# GET /api/data/latest
# ---------------------------------------------------------------------------
@data_bp.route("/data/latest", methods=["GET"])
def get_latest():
    """Return the most recent sensor reading."""
    device_id = request.args.get("device_id")
    row = SensorReadingModel.get_latest(device_id)
    if row is None:
        return jsonify({"error": "No data available"}), 404
    return jsonify(row), 200


# ---------------------------------------------------------------------------
# GET /api/data/history
# ---------------------------------------------------------------------------
@data_bp.route("/data/history", methods=["GET"])
def get_history():
    """Return paginated historical readings (newest first).

    Query params:
        device_id (str)  – filter by device (optional)
        limit     (int)  – max rows per page (default 100, max 1000)
        offset    (int)  – row offset for pagination (default 0)
    """
    device_id = request.args.get("device_id")
    try:
        limit  = min(int(request.args.get("limit",  100)), 1000)
        offset = max(int(request.args.get("offset", 0)),   0)
    except ValueError:
        return jsonify({"error": "limit and offset must be integers"}), 400

    rows = SensorReadingModel.get_history(device_id, limit, offset)
    return jsonify({
        "count":  len(rows),
        "limit":  limit,
        "offset": offset,
        "data":   rows,
    }), 200


# ---------------------------------------------------------------------------
# GET /api/status
# ---------------------------------------------------------------------------
@data_bp.route("/status", methods=["GET"])
def get_status():
    """Return API health and basic statistics."""
    import time, os
    uptime = time.time() - current_app.config.get("START_TIME", time.time())
    latest = SensorReadingModel.get_latest()
    return jsonify({
        "status":        "ok",
        "uptime_s":      int(uptime),
        "version":       "1.0.0",
        "last_reading":  latest,
    }), 200


# ---------------------------------------------------------------------------
# GET /api/session
# ---------------------------------------------------------------------------
@data_bp.route("/session", methods=["GET"])
def get_session():
    """Return aggregated statistics for the latest charging session."""
    device_id = request.args.get("device_id")
    summary = SensorReadingModel.get_session_summary(device_id)
    return jsonify(summary), 200


# ---------------------------------------------------------------------------
# DELETE /api/data  (for testing / admin only)
# ---------------------------------------------------------------------------
@data_bp.route("/data", methods=["DELETE"])
def clear_data():
    """Delete all stored readings. Protected by a simple admin token."""
    token = request.headers.get("X-Admin-Token", "")
    expected = current_app.config.get("ADMIN_TOKEN", "")
    if not expected or token != expected:
        return jsonify({"error": "Unauthorized"}), 403

    from database.db import execute_db
    execute_db("DELETE FROM sensor_readings")
    return jsonify({"status": "cleared"}), 200
