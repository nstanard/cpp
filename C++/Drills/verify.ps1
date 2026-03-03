# Verify drill solutions

param(
    [string]$drill = "all"
)

$passed = 0
$failed = 0
$skipped = 0

function Test-Drill {
    param(
        [string]$file,
        [string]$expectedOutput,
        [string]$input = ""
    )
    
    $drillName = [System.IO.Path]::GetFileNameWithoutExtension($file)
    
    if (-not (Test-Path $file)) {
        Write-Host "  ⊘ $drillName - SKIPPED (file not found)" -ForegroundColor Gray
        return "skipped"
    }
    
    # Compile
    $exeName = "$drillName.exe"
    $compileResult = & g++ -g $file -o $exeName 2>&1
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  ✗ $drillName - COMPILE ERROR" -ForegroundColor Red
        Write-Host "    $compileResult" -ForegroundColor DarkRed
        return "failed"
    }
    
    # Run
    try {
        if ($input -ne "") {
            $output = ($input | & ".\$exeName" 2>&1) -join "`n"
        } else {
            $output = (& ".\$exeName" 2>&1) -join "`n"
        }
        
        $output = $output.Trim()
        $expectedOutput = $expectedOutput.Trim()
        
        if ($output -eq $expectedOutput) {
            Write-Host "  ✓ $drillName - PASSED" -ForegroundColor Green
            return "passed"
        } else {
            Write-Host "  ✗ $drillName - FAILED" -ForegroundColor Red
            Write-Host "    Expected: $expectedOutput" -ForegroundColor DarkYellow
            Write-Host "    Got:      $output" -ForegroundColor DarkYellow
            return "failed"
        }
    }
    catch {
        Write-Host "  ✗ $drillName - RUNTIME ERROR" -ForegroundColor Red
        Write-Host "    $_" -ForegroundColor DarkRed
        return "failed"
    }
}

Write-Host "`n=== C++ Drill Verification ===" -ForegroundColor Cyan
Write-Host ""

# Define drills with expected outputs
$drills = @(
    @{file="drill01.cpp"; expected="Hello World!"; input=""}
    @{file="drill02.cpp"; expected="My name is Alice and I am 25 years old."; input=""}
    @{file="drill03.cpp"; expected="What is your name? Hello, Bob!"; input="Bob"}
    @{file="drill04.cpp"; expected="Sum: 13`nDifference: 7`nProduct: 30`nQuotient: 3"; input=""}
    @{file="drill05.cpp"; expected="Pass"; input=""}
    @{file="drill06.cpp"; expected="1`n2`n3`n4`n5"; input=""}
    @{file="drill07.cpp"; expected="5`n4`n3`n2`n1`nBlast off!"; input=""}
    @{file="drill08.cpp"; expected="2`n4`n6`n8`n10"; input=""}
    @{file="drill09.cpp"; expected="42"; input=""}
    @{file="drill10.cpp"; expected="Hello, World!"; input=""}
)

if ($drill -eq "all") {
    foreach ($d in $drills) {
        $result = Test-Drill -file $d.file -expectedOutput $d.expected -input $d.input
        if ($result -eq "passed") { $passed++ }
        elseif ($result -eq "failed") { $failed++ }
        else { $skipped++ }
    }
} else {
    # Test single drill
    $drillFile = "drill$($drill.PadLeft(2,'0')).cpp"
    $drillDef = $drills | Where-Object { $_.file -eq $drillFile }
    
    if ($null -eq $drillDef) {
        Write-Host "Drill not found: $drillFile" -ForegroundColor Red
        exit 1
    }
    
    $result = Test-Drill -file $drillDef.file -expectedOutput $drillDef.expected -input $drillDef.input
    if ($result -eq "passed") { $passed++ }
    elseif ($result -eq "failed") { $failed++ }
    else { $skipped++ }
}

# Summary
Write-Host ""
Write-Host "=== Summary ===" -ForegroundColor Cyan
Write-Host "  Passed:  $passed" -ForegroundColor Green
Write-Host "  Failed:  $failed" -ForegroundColor Red
Write-Host "  Skipped: $skipped" -ForegroundColor Gray
Write-Host ""

if ($failed -eq 0 -and $passed -gt 0) {
    Write-Host "🎉 All drills passed!" -ForegroundColor Green
}
