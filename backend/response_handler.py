from flask import jsonify, Response
from json import dumps

class ResponseHandler:
    """Class to handle consistent JSON responses for the backend."""

    @staticmethod
    def generate_success_response(message: str, data: dict = None) ->Response:
        """Return a consistent JSON response for success."""
        response = {
            "status": "success",
            "message": message,
            "data": data or {}
        }
        return jsonify(response), 200

    @staticmethod
    def generate_error_response(message: str, status_code: int = 400) ->Response:
        """Return a consistent JSON response for errors."""
        response = {
            "status": "error",
            "message": message,
            "data": {}
        }
        return jsonify(response), status_code