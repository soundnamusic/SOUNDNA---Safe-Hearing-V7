#!/bin/bash
set -euo pipefail
echo "SOUNDNA 1.5.1 - script corretto, verifica architetture"
cd "$(dirname "$0")/.."
if [[ "$(uname -s)" != Darwin ]]; then echo 'Eseguire questo script su macOS.'; exit 1; fi
if ! xcrun --find clang++ >/dev/null 2>&1; then echo 'Servono gli strumenti Apple: xcode-select --install'; exit 1; fi
export PATH="/Applications/CMake.app/Contents/bin:$PATH"
if ! command -v cmake >/dev/null; then echo 'Serve CMake 3.22 o successivo: https://cmake.org/download/'; exit 1; fi
cmake -S . -B build-universal -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0
cmake --build build-universal --config Release --parallel 4
ctest --test-dir build-universal -C Release --output-on-failure
mkdir -p dist/macOS
for format in AU VST3; do
  if [[ "$format" == AU ]]; then suffix=component; else suffix=vst3; fi
  source_path="build-universal/SoundnaListen_artefacts/Release/$format/SOUNDNA - Safe Hearing.$suffix"
  target_path="dist/macOS/SOUNDNA - Safe Hearing.$suffix"
  if [[ -e "$target_path" ]]; then echo "Output gia presente: $target_path. Spostalo prima di ricompilare."; exit 1; fi
  ditto "$source_path" "$target_path"
  xcrun lipo "$target_path/Contents/MacOS/SOUNDNA - Safe Hearing" -verify_arch arm64 x86_64
  codesign --force --deep --sign - "$target_path"
  codesign --verify --deep --strict "$target_path"
done
ditto -c -k --sequesterRsrc --keepParent dist/macOS dist/SOUNDNA-Safe-Hearing-macOS-universal.zip
echo 'Plugin creati in dist/macOS. Nessuna installazione automatica.'
