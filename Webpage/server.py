import subprocess
import os
import sys
import signal
import logging
from logging.handlers import RotatingFileHandler
from threading import Thread

IP_ADDRESS = '192.168.1.101' # Replace with the IP address you want to bind to
PORT = 5000

# Paths to the frontend and backend directories
FRONTEND_DIR = os.path.join(os.getcwd(), 'frontend')
BACKEND_DIR = os.path.join(os.getcwd(), 'backend')

# Commands to start the frontend and backend
FRONTEND_COMMAND = ['python3', '-m', 'http.server', PORT, '--bind', IP_ADDRESS]
BACKEND_COMMAND = ['node', 'server.js']

# Log files for frontend and backend
FRONTEND_LOG_FILE = 'frontend.log'
BACKEND_LOG_FILE = 'backend.log'

# Log handlers for file rotation
FRONTEND_LOG_HANDLER = RotatingFileHandler(FRONTEND_LOG_FILE, maxBytes=10 * 1024 * 1024, backupCount=1)  # 10MB
BACKEND_LOG_HANDLER = RotatingFileHandler(BACKEND_LOG_FILE, maxBytes=10 * 1024 * 1024, backupCount=1)  # 10MB

# Configure loggers for frontend and backend
frontend_logger = logging.getLogger('frontend')
backend_logger = logging.getLogger('backend')

frontend_logger.setLevel(logging.INFO)
backend_logger.setLevel(logging.INFO)

frontend_logger.addHandler(FRONTEND_LOG_HANDLER)
backend_logger.addHandler(BACKEND_LOG_HANDLER)

# Process handlers
frontend_process = None
backend_process = None

def start_frontend():
    global frontend_process
    frontend_logger.info("Starting frontend server...")
    os.chdir(FRONTEND_DIR)
    frontend_process = subprocess.Popen(
        FRONTEND_COMMAND,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    log_process_output(frontend_process, frontend_logger)

def start_backend():
    global backend_process
    backend_logger.info("Starting backend server...")
    backend_process = subprocess.Popen(
        BACKEND_COMMAND,
        cwd=BACKEND_DIR,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    log_process_output(backend_process, backend_logger)

def log_process_output(process, logger):
    """Logs the output of a subprocess to the specified logger."""
    def log_stream(stream, log_func):
        for line in iter(stream.readline, ''):
            log_func(line.strip())

    # Start separate threads to read and log stdout and stderr
    Thread(target=log_stream, args=(process.stdout, logger.info), daemon=True).start()
    Thread(target=log_stream, args=(process.stderr, logger.error), daemon=True).start()

def stop_processes(signum, frame):
    print("\nStopping servers...")
    if frontend_process:
        frontend_process.terminate()
        frontend_process.wait()
        frontend_logger.info("Frontend server stopped.")
    if backend_process:
        backend_process.terminate()
        backend_process.wait()
        backend_logger.info("Backend server stopped.")
    sys.exit(0)

if __name__ == '__main__':
    # Register the signal handler to stop processes on termination
    signal.signal(signal.SIGINT, stop_processes)
    signal.signal(signal.SIGTERM, stop_processes)

    try:
        # Start both servers
        start_backend()
        start_frontend()

        # Wait indefinitely, allowing servers to run
        print("Servers are running. Press Ctrl+C to stop.")
        signal.pause()
    except Exception as e:
        backend_logger.error(f"Error starting servers: {e}")
        frontend_logger.error(f"Error starting servers: {e}")
        stop_processes(None, None)
