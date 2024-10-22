from flask import jsonify, Response
from json import dumps

class ResponseHandler:
    """Class to handle consistent JSON responses for the backend."""

    @staticmethod
    def success(message: str, data: dict = None) ->Response:
        """Return a consistent JSON response for success."""
        response = {
            "status": "success",
            "message": message,
            "data": data or {}
        }
        return jsonify(response), 200

    @staticmethod
    def error(message: str, status_code: int = 400) ->tuple[Response, int]:
        """Return a consistent JSON response for errors."""
        response = {
            "status": "error",
            "message": message,
            "data": {}
        }
        return jsonify(response), status_code

    @staticmethod
    def core_data_passer(message: str, data: dict = None) -> str:
        """Return a JSON-formatted response for core components with provided data."""
        core_response = {
            "status": "success",
            "message": message,
            "data": data or {}
        }
        return dumps(core_response)
    #You can only use jasonify for http communication to send the data as a string use json.dumps
