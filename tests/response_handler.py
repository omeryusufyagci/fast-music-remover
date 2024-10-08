from flask import jsonify
from typing import Any, Dict

class ResponseHandler:
    """Handles JSON responses for the application."""
    
    @staticmethod
    def success(data: Any = None, message: str = "Success") -> Dict[str, Any]:
        """Returns a success response."""
        return jsonify({"status": "success", "message": message, "data": data})

    @staticmethod
    def error(message: str, status_code: int = 400) -> Dict[str, Any]:
        """Returns an error response."""
        return jsonify({"status": "error", "message": message}), status_code
