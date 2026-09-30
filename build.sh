#!/usr/bin/env bash
set -euo pipefail

# No arguments: full byte-for-byte verification of every function (~10 min
# solo with BUILD_POOL=8; longer while other clones build — a host-wide lock
# serializes full builds). To iterate quickly, pass source files or function
# names to verify just those (a few seconds, skips the baseline hash and
# no-op patch):
#   ./build.sh src/math/color.cpp

# Keep Wine quiet and avoid needing an X display.
#
# WINEDEBUG does not silence the MoltenVK banner: that comes from the Vulkan
# driver Wine loads on macOS, not from Wine's own debug channels. It printed a
# four-line extension list on every compile, and a body costs several compiles,
# so it was the bulk of what a converter had to read past. MVK_CONFIG_LOG_LEVEL
# is MoltenVK's own switch and 0 means none. Measured: 4 banner lines -> 0, with
# the byte comparison unaffected (Functions: OK 1/1 matched).
export WINEDEBUG="${WINEDEBUG:--all}"
export DISPLAY="${DISPLAY:-}"
export MVK_CONFIG_LOG_LEVEL="${MVK_CONFIG_LOG_LEVEL:-0}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec python3 "$SCRIPT_DIR/tools/build.py" "$@"
