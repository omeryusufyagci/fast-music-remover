import pytest
from flask import Flask
from backend.response_handler import ResponseHandler


@pytest.fixture
def app_context():
    """Fixture to set up and tear down a Flask application context."""
    app = Flask(__name__)
    with app.app_context():
        yield app


def test_generate_success_response_with_data(app_context):
    message = "Operation completed"
    data = {"key": "value"}
    response, status_code = ResponseHandler.generate_success_response(message, data)

    assert status_code == 200
    assert response.json == {
        "status": "success",
        "message": message,
        "payload": data
    }


def test_generate_success_response_without_data(app_context):
    message = "Operation completed"
    response, status_code = ResponseHandler.generate_success_response(message)

    assert status_code == 200
    assert response.json == {
        "status": "success",
        "message": message,
        "payload": {}
    }


def test_generate_error_response_default_status_code(app_context):
    message = "An error occurred"
    response, status_code = ResponseHandler.generate_error_response(message)

    assert status_code == 400
    assert response.json == {
        "status": "error",
        "message": message,
        "payload": {}
    }


def test_generate_error_response_custom_status_code(app_context):
    message = "Not found"
    status_code = 404
    response, status_code = ResponseHandler.generate_error_response(message, status_code)

    assert status_code == 404
    assert response.json == {
        "status": "error",
        "message": message,
        "payload": {}
    }
