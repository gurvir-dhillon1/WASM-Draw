# WebAssembly Drawing Tool

This is a C++ drawing tool that runs in the browser using WebAssembly. To host it locally, follow the steps below. Alternatively, you can check it out [here](https://wasm-draw.art/).

If you prefer not to work with Docker containers, follow the instructions below. Otherwise, simply run `docker-compose up -d` to self-host. Be sure to rebuild the relevant container if you make changes that require recompiling. I’m working on making this process more seamless.

### Prerequisites (for local setup without Docker)

Before running WASM-Draw locally, you’ll need to install the following tools:

- [Emscripten Compiler](https://emscripten.org/docs/getting_started/downloads.html): This tool compiles C++ code into WebAssembly.
- CMake: This is the build system used for compiling the C++ code.
- Go: The backend is written in Go.
- Python3 (optional): I use Python to serve a simple HTTP server, but you can use any alternative server you prefer.

### Frontend Setup

1. Clone the repository or download the source files.

2. Install Emscripten by following the [installation guide](https://emscripten.org/docs/getting_started/downloads.html).

3. Navigate to the repository root directory:
   ```
   cd <repository_root>
   ```

4. Change into the frontend directory:
   ```
   cd frontend
   ```

5. We use CMake for building, so you’ll need to have it installed.

6. Run the following commands to set up the build environment:
   ```
   emcmake cmake -S . -B build
   cmake --build build
   ```

7. To host the compiled files, use a simple HTTP server (Python example):
   ```
   python3 -m http.server
   ```

### Backend Setup

1. Navigate to the repository root directory:
   ```
   cd <repository_root>
   ```

2. Change into the backend directory:
   ```
   cd backend
   ```

3. Download Go dependencies:
   ```
   go mod download
   ```

4. Build the Go server:
   ```
   go build -o server .
   ```

5. Run the server:
   ```
   ./server
   ```

### Using WASM-Draw

Once the server is up and running, you can access the drawing tool directly in your browser.

### Troubleshooting

- If the webpage isn’t loading or you encounter issues, double-check that the environment is set up correctly and that the WASM-Draw project is compiled properly.
- If the page isn't rendering or functioning as expected, inspect the browser console for errors.

---

Let me know if you'd like to adjust anything further!
