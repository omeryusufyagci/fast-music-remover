FROM python:3.9-slim

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies (little heavy as FFmpeg devlibs are required)
RUN apt-get update && \
    apt-get install -y build-essential cmake ffmpeg wget pkg-config \
                       libavcodec-dev libavformat-dev libavfilter-dev libavdevice-dev libswscale-dev && \
    apt-get clean

# pkg manager doesn't find this?
RUN mkdir -p /usr/include/nlohmann && \
    wget https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp -O /usr/include/nlohmann/json.hpp

WORKDIR /app

# Copy the project into the container/app
COPY . /app

# Compile the C++ project with a fresh CMakeCache (issues with cache not matching source)
RUN mkdir -p MediaProcessor/build && \
    cd MediaProcessor/build && \
    rm -rf CMakeCache.txt && \
    cmake .. && \
    make

# Install python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Expose port:5000 for the backend and set env vars
EXPOSE 5000
ENV FLASK_APP=app.py
ENV FLASK_ENV=development

CMD ["flask", "run", "--host=0.0.0.0"]
