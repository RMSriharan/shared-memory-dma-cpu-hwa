# shared-memory-dma-cpu-hwa
Simulate CPU ↔ HWA Shared Memory Communication with Mutex and DMA-style Transfer using POSIX APIs.
# 🔧 Shared Memory DMA Simulation with Mutex (CPU ↔ HWA)

This project simulates a CPU ↔ Hardware Accelerator (HWA) communication system using **POSIX shared memory** and **mutex-based synchronization**. It's designed to demonstrate concepts like:

- Shared Memory (via `shm_open` + `mmap`)
- DMA-style data transfers (2048-byte bulk buffer)
- Mutex-based inter-process coordination (`pthread_mutex_t`)
- Real-time synchronization between simulated CPU and HWA processes

> ✅ No actual hardware is needed — the entire project runs on Ubuntu Linux.

---

## 📁 Project Structure

```bash
shared_memory_dma_mutex/
├── cpu.c      # CPU-side process
├── hwa.c      # HWA-side process
├── Makefile   # Build script
```

---

## 🧪 How It Works

1. **CPU Process**:
   - Creates shared memory
   - Initializes a mutex (`pthread_mutex_t`) inside the memory
   - Writes a large block of data (DMA-style)
   - Signals `flag = 1` to indicate data is ready

2. **HWA Process**:
   - Waits for CPU to create memory
   - Waits until `flag = 1`
   - Locks the buffer, processes the data (increment chars)
   - Signals `flag = 2` to indicate done

3. **Synchronization**:
   - Uses `pthread_mutex_t` with `PTHREAD_PROCESS_SHARED`
   - Uses `msync()` to simulate memory flush / cache coherency

---

## 🛠️ How to Build & Run

### 🧱 Prerequisites
- Ubuntu 20.04+ or any POSIX Linux
- GCC

### 🔨 Build:

```bash
make
```

### 🚀 Run:

Open two terminals:

#### Terminal 1:
```bash
./hwa
```

#### Terminal 2:
```bash
./cpu
```

---

## 💡 Sample Output

### HWA Terminal:
```
[HWA] Waiting for shared memory to be created...
[HWA] Waiting for CPU to write data...
[HWA] Processing data from CPU...
```

### CPU Terminal:
```
[CPU] Writing DMA-like large data to shared memory...
[CPU] Waiting for HWA to process...
[CPU] HWA response (preview): BCDEFGHIJKLMNOPQRSTUVWXYZABCDE...
```
