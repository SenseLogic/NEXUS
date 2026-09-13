@echo off
"C:\Program Files (x86)\Flax\Flax_1.12\Binaries\Tools\Flax.Build.exe" -build -platform=Windows -configuration=Release -arch=x64 -buildTargets=GameEditorTarget -buildTargets=GameTarget -project="%~dp0Test.flaxproj"
