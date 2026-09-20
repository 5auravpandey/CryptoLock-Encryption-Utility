# ============================================================================
#  CryptoLock – PowerShell Build Script
#  Usage:  .\build.ps1          (build)
#          .\build.ps1 clean    (remove artifacts)
#          .\build.ps1 rebuild  (clean + build)
# ============================================================================

param(
    [string]$Action = "build"
)

$CXX      = "C:\msys64\ucrt64\bin\g++.exe"
$CXXFLAGS = @("-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-O2")
$INCLUDES = @("-Iinclude")
$TARGET   = "cryptolock.exe"
$SRCDIR   = "src"
$OBJDIR   = "obj"

function Do-Clean {
    if (Test-Path $OBJDIR) { Remove-Item -Recurse -Force $OBJDIR }
    if (Test-Path $TARGET) { Remove-Item -Force $TARGET }
    Write-Host "  Cleaned." -ForegroundColor Yellow
}

function Do-Build {
    if (-not (Test-Path $OBJDIR)) {
        New-Item -ItemType Directory -Path $OBJDIR | Out-Null
    }

    $sources = Get-ChildItem -Path $SRCDIR -Filter "*.cpp"
    $objects = @()

    foreach ($src in $sources) {
        $objName = $src.BaseName + ".o"
        $objPath = Join-Path $OBJDIR $objName
        $objects += $objPath

        Write-Host "  Compiling $($src.Name) ..." -ForegroundColor Cyan
        & $CXX @CXXFLAGS @INCLUDES -c $src.FullName -o $objPath
        if ($LASTEXITCODE -ne 0) {
            Write-Host "  COMPILE FAILED on $($src.Name)" -ForegroundColor Red
            exit 1
        }
    }

    Write-Host "  Linking -> $TARGET ..." -ForegroundColor Cyan
    & $CXX @CXXFLAGS @objects -o $TARGET
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  LINK FAILED" -ForegroundColor Red
        exit 1
    }

    Write-Host ""
    Write-Host "  Build complete -> .\$TARGET" -ForegroundColor Green
    Write-Host ""
}

switch ($Action.ToLower()) {
    "clean"   { Do-Clean }
    "rebuild" { Do-Clean; Do-Build }
    "build"   { Do-Build }
    default   { Write-Host "Unknown action: $Action. Use build, clean, or rebuild." }
}
