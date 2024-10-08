from flask import jsonify
from typing import Any, Dict

class ResponseHandler:
    """Class to handle consistent JSON responses."""

    @staticmethod
    def success(message: str, data: Dict[str, Any] = None) -> Any:
        response = {"status": "success", "message": message}
        if data:
            response.update(data)
        return jsonify(response)

    @staticmethod
    def error(message: str, code: int = 400) -> Any:
        return jsonify({"status": "error", "message": message}), code
