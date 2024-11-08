import unittest
from flask import Flask
from backend.response_handler import ResponseHandler

class TestResponseHandler(unittest.TestCase):

    def setUp(self):
        # Set up a Flask application context for testing
        self.app = Flask(__name__)
        self.app_context = self.app.app_context()
        self.app_context.push()

    def tearDown(self):
        # Tear down the Flask application context
        self.app_context.pop()

    def test_generate_success_response_with_data(self):
        message = "Operation completed"
        data = {"key": "value"}
        response, status_code = ResponseHandler.generate_success_response(message, data)
        
        self.assertEqual(status_code, 200)
        self.assertEqual(response.json, {
            "status": "success",
            "message": message,
            "data": data
        })

    def test_generate_success_response_without_data(self):
        message = "Operation completed"
        response, status_code = ResponseHandler.generate_success_response(message)
        
        self.assertEqual(status_code, 200)
        self.assertEqual(response.json, {
            "status": "success",
            "message": message,
            "data": {}
        })

    def test_generate_error_response_default_status_code(self):
        message = "An error occurred"
        response, status_code = ResponseHandler.generate_error_response(message)
        
        self.assertEqual(status_code, 400)
        self.assertEqual(response.json, {
            "status": "error",
            "message": message,
            "data": {}
        })

    def test_generate_error_response_custom_status_code(self):
        message = "Not found"
        status_code = 404
        response, status_code = ResponseHandler.generate_error_response(message, status_code)
        
        self.assertEqual(status_code, 404)
        self.assertEqual(response.json, {
            "status": "error",
            "message": message,
            "data": {}
        })

if __name__ == "__main__":
    unittest.main()
