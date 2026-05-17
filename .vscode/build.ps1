param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64"
)

$ErrorActionPreference = "Stop"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "Không tìm thấy vswhere.exe. Hãy cài Visual Studio Community hoặc Build Tools."
}

$msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if (-not $msbuild) {
    throw "Không tìm thấy MSBuild. Hãy cài workload 'Desktop development with C++' trong Visual Studio Installer."
}

& $msbuild "$PSScriptRoot\..\CaroGame.sln" /m /p:Configuration=$Configuration /p:Platform=$Platform
