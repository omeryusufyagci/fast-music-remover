FROM python:3.10-slim

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies (including FFmpeg and libsndfile)
RUN apt-get update && \
    apt-get install -y build-essential cmake ffmpeg wget pkg-config \
                       libavcodec-dev libavformat-dev libavfilter-dev \
                       libavdevice-dev libswscale-dev libsndfile-dev git && \
    apt-get clean

WORKDIR /app

# Copy the project into the container
COPY . /app

# Handle library wrt arch
# This is used in CI to avoid having separate images per arch
ARG TARGETPLATFORM
RUN echo "Detected platform: ${TARGETPLATFORM}" && \
    case "${TARGETPLATFORM}" in \
      "linux/amd64") cp /app/MediaProcessor/lib/libdf-linux-amd64.so /app/MediaProcessor/lib/libdf.so ;; \
      "linux/arm64") cp /app/MediaProcessor/lib/libdf-linux-arm64.so /app/MediaProcessor/lib/libdf.so ;; \
      *) echo "Unsupported platform: ${TARGETPLATFORM}" && exit 1 ;; \
    esac

# Compile the C++ project with a fresh CMakeCache
RUN mkdir -p MediaProcessor/build && \
    cd MediaProcessor/build && \
    rm -rf CMakeCache.txt CMakeFiles _deps && \
    cmake .. && \
    make

# Install python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Set Flask environment variables
ENV FLASK_APP=app.py
ENV FLASK_ENV=development

# Expose the port for Flask
EXPOSE 8080

# Run Flask application
CMD ["flask", "run", "--host=0.0.0.0", "--port=8080"]
