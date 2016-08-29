cd /d "C:\Users\Matt\Documents\VRBackend\VRBackend" &msbuild "VRBackend.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform=x64
exit %errorlevel% 