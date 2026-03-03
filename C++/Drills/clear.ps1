# Clear all drill files to practice again

Write-Host "Clearing all drill files..." -ForegroundColor Yellow

# Remove all .cpp files
Remove-Item -Path "*.cpp" -ErrorAction SilentlyContinue

# Remove all .exe files
Remove-Item -Path "*.exe" -ErrorAction SilentlyContinue

# Remove all compilation artifacts
Remove-Item -Path "*.obj" -ErrorAction SilentlyContinue
Remove-Item -Path "*.pdb" -ErrorAction SilentlyContinue
Remove-Item -Path "*.ilk" -ErrorAction SilentlyContinue
Remove-Item -Path "*.o" -ErrorAction SilentlyContinue

Write-Host "All drill files cleared! Ready to practice." -ForegroundColor Green
