from flask import jsonify, Response
from json import dumps

class ResponseHandler:
    """Class to handle consistent JSON responses for the backend."""

    @staticmethod
    def generate_response(status:str, message:str, status_code:int, data:dict = None ) ->Response:

        response = {
            "status": status,
            "message": message,
            "data": data or {}
        } 
        return jsonify(response), status_code

    @staticmethod
    def generate_success_response(message: str, data: dict = None) ->Response:
        """Return a consistent JSON response for success."""
        response = ResponseHandler.generate_response("success", message, 200,data)
        return response

    @staticmethod
    def generate_error_response(message: str, status_code: int = 400) ->Response:
        """Return a consistent JSON response for errors."""
        response = ResponseHandler.generate_response("error", message, status_code)
        return response
    
    
    
