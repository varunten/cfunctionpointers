# Code — 16 Plugin Architectures

Build a plugin system around a struct-of-function-pointers contract. The runnable
code uses **static** plugins (portable, same concept); a reference file shows the
**dynamic** (`dlopen`/`LoadLibrary`) version.

## Files

| File | What it shows |
|------|----------------|
| `01_plugin_contract.c` | The contract (`Plugin` struct) + two plugins + a host that runs both through it. |
| `02_plugin_registry.c` | A registry: register many plugins, then drive them all uniformly (init/run/cleanup lifecycle). |
| `03_versioned_contract.c` | An ABI version field checked on load; an out-of-date plugin is rejected. |
| `dynamic_loading_reference.c` | NOT compiled by default — a documented reference of the `dlopen`+`dlsym` (and Windows `LoadLibrary`) flow. |

## Build & run

```sh
gcc 01_plugin_contract.c    -o plug    && ./plug
gcc 02_plugin_registry.c    -o preg    && ./preg
gcc 03_versioned_contract.c -o pver    && ./pver
```

`dynamic_loading_reference.c` is a reference; to actually try dynamic loading on
Linux/macOS you'd build the host with `-ldl` and each plugin as a `.so` (see the
comments inside that file). It's kept separate so the portable examples build
everywhere, including Windows/MinGW.

## What to look for

- In `02`, the host loop `registry[i]->run(input)` is virtual dispatch (section
  14) over a list (section 09) of contracts — every pattern in the series,
  combined.
- In `03`, changing one number (the plugin's `abi_version`) flips it from
  accepted to rejected. That field is what keeps plugin systems from crashing
  across upgrades.
