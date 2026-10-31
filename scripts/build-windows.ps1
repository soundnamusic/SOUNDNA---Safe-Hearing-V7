$ErrorActionPreference = 'Stop'
Set-Location (Join-Path $PSScriptRoot '..')
function Run-Checked { param([scriptblock]$Command); & $Command; if ($LASTEXITCODE -ne 0) { throw "Comando fallito: $LASTEXITCODE" } }
Run-Checked { cmake -S . -B build-windows -A x64 }
Run-Checked { cmake --build build-windows --config Release --parallel 4 }
Run-Checked { ctest --test-dir build-windows -C Release --output-on-failure }
New-Item -ItemType Directory -Force -Path 'dist/Windows' | Out-Null
$target = 'dist/Windows/SOUNDNA - Safe Hearing.vst3'
if (Test-Path $target) { throw 'Output gia presente: spostalo prima di ricompilare.' }
Copy-Item 'build-windows/SoundnaListen_artefacts/Release/VST3/SOUNDNA - Safe Hearing.vst3' $target -Recurse
Compress-Archive -Path 'dist/Windows/*' -DestinationPath 'dist/SOUNDNA_Listen_Windows_x64.zip' -Force
Write-Host 'Plugin creato in dist/Windows. Nessuna installazione automatica.'
