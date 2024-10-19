import json
import logging
import os
import re
import subprocess
import typing
import flask
from pathlib import Path
from urllib.parse import urlparse


import yt_dlp
from flask import Flask, jsonify, render_template, request, send_from_directory, url_for