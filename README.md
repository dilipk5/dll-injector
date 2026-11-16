
# DLL Injector

A simple Windows DLL injector written in C using the Win32 API.

### Features
- Enumerates running processes.
- Finds a target process by name.
- Allocates memory in the target process.
- Writes the DLL path into remote memory.
- Executes `LoadLibraryW` using a remote thread.

### Usage
```bash
DllInjector.exe <ProcessName.exe> <inject.dll>
```

<img width="1997" height="936" alt="image" src="https://github.com/user-attachments/assets/62a54082-74f5-4161-b218-9060be977d21" />
