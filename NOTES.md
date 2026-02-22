# Project Notes

## .gitignore

The `.gitignore` at the repo root excludes:

| Pattern | Reason |
|---|---|
| `*.exe` | Compiled binaries — generated, not source |
| `*.obj` | Compiler object files |
| `*.ilk` | MSVC incremental linker files |
| `*.pdb` | Debug symbol databases |
| `build/` | CMake build directories (e.g. `phase1/build/`) |
| `.vscode/c_cpp_properties.json` | Machine-specific IntelliSense paths |

`.vscode/tasks.json` is **tracked** — it defines shared build tasks worth committing.
