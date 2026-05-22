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
app.py
~~~~~~
Flask REST API for the IoT Solar Charging System.

Run:
    python app.py                   # development
    gunicorn -w 4 -b 0.0.0.0:5000 app:app   # production

Environment variables:
    DATABASE_PATH  – path to SQLite file (default: solar_charger.db)
    ADMIN_TOKEN    – token required for DELETE /api/data
    FLASK_DEBUG    – set to 1 for development debug mode
    PORT           – server port (default: 5000)
"""

import os
import time

from flask import Flask, jsonify
from flask_cors import CORS

from database.db import init_db
from routes.data_routes import data_bp


def create_app() -> Flask:
    """Application factory."""
    app = Flask(__name__)

    # --- Configuration --------------------------------------------------
    app.config["DATABASE_PATH"] = os.environ.get(
        "DATABASE_PATH",
        os.path.join(os.path.dirname(__file__), "solar_charger.db")
    )
    app.config["ADMIN_TOKEN"]   = os.environ.get("ADMIN_TOKEN", "")
    app.config["START_TIME"]    = time.time()
    app.config["JSON_SORT_KEYS"] = False

    # --- CORS (allow React dev server on port 3000) ----------------------
    CORS(app, resources={r"/api/*": {"origins": "*"}})

    # --- Database --------------------------------------------------------
    init_db(app)

    # --- Blueprints ------------------------------------------------------
    app.register_blueprint(data_bp)

    # --- Global error handlers ------------------------------------------
    @app.errorhandler(404)
    def not_found(e):
        return jsonify({"error": "Endpoint not found"}), 404

    @app.errorhandler(405)
    def method_not_allowed(e):
        return jsonify({"error": "Method not allowed"}), 405

    @app.errorhandler(500)
    def internal_error(e):
        return jsonify({"error": "Internal server error"}), 500

    return app


app = create_app()

if __name__ == "__main__":
    port  = int(os.environ.get("PORT", 5000))
    debug = os.environ.get("FLASK_DEBUG", "0") == "1"
    app.run(host="0.0.0.0", port=port, debug=debug)
