#pragma once
#include <chrono>
#include <codecvt>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include "utils.h"

const std::wstring SCRIPT = LR"([Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8
$ErrorActionPreference = "Stop"

$logfile = "${PARAM_VIDEO_LOG_FILE}"

function Write-Log($msg) {
    $timestamp = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss")
    "$timestamp $msg" | Out-File -FilePath $logfile -Append -Encoding UTF8
}

function Write-Cmd($msg) {
    Write-Output ($msg -replace "`r`n", "" -replace "`n", "") + "`n"
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location -Path $ScriptDir

$apiKey    = "${PARAM_KEY}"
$prompt    = Get-Content -Path "script.prompt" -Raw -Encoding UTF8
$promptString = [string]$prompt
$seconds   = "${PARAM_SECONDS}" # must be "4", "8" or "12"
$resolution = "${PARAM_RESOLUTION}"
$output    = "${PARAM_VIDEO_NAME}"
$model     = "${PARAM_MODEL_NAME}"

Write-Log "Creating video job..."

$body = @{
    model = $model
    prompt = $promptString
    seconds = $seconds
    size = $resolution
} | ConvertTo-Json

$bytes = [System.Text.Encoding]::UTF8.GetBytes($body)

$headers = @{
    "Authorization" = "Bearer $apiKey"
    "Content-Type"  = "application/json"
}

# --- CREATE JOB ---
try {
    $response = Invoke-RestMethod -Method Post -Uri "https://api.openai.com/v1/videos" -Headers $headers -Body $bytes

    Write-Log "/videos response:"
    Write-Log ($response | ConvertTo-Json -Depth 10)
    Write-Cmd ("[SUCCESS]" + ($response | ConvertTo-Json -Depth 10 -Compress))
}
catch {
    Write-Log "Failed to create job: $_"
    Write-Cmd "[ERROR]$_"
    exit
}

$videoID = $response.id
if (-not $videoID) {
    Write-Log "Failed to get video ID: "
    Write-Log ($response | ConvertTo-Json -Depth 10)
    Write-Cmd ("[ERROR]" + ($response | ConvertTo-Json -Depth 10 -Compress))
    exit
}

Write-Log "Video Job ID: $videoID"

# --- CHECK STATUS ---
$status = ""
$progress = 0

while ($true) {

    $statusUrl = "https://api.openai.com/v1/videos/$videoID"

    try {
        $statusResp = Invoke-RestMethod -Method Get -Uri $statusUrl -Headers $headers
        
        Write-Log "Status response:"
        Write-Log ($statusResp | ConvertTo-Json -Depth 10)

        Write-Cmd ("[SUCCESS]" + ($statusResp | ConvertTo-Json -Depth 10 -Compress))

        $status = $statusResp.status
        $progress = $statusResp.progress
    }
    catch {
        Write-Log "Failed to query status: $_"
        Write-Cmd "[WARNING]$_"
        Start-Sleep -Seconds 2
        continue
    }

    if ($status -eq "queued") {
        Write-Log "Queued...${progress}%"
    }
    elseif ($status -eq "in_progress") {
        Write-Log "Generating...${progress}%"
        
    }
    elseif ($status -eq "completed") {
        Write-Log "Completed"
    }
    elseif ($status -eq "failed") {
        Write-Log "Video generation failed."
        exit
    }
    else {
        Write-Log "Status: $status"
    }

    if ($status -eq "completed") { break }

    Start-Sleep -Seconds 2
}

Write-Log "Downloading video..."

$contentUrl = "https://api.openai.com/v1/videos/$videoID/content"

try {
    Invoke-WebRequest -Uri $contentUrl -Headers $headers -OutFile $output
    Write-Log "Download completed: $output"
}
catch {
    Write-Log "Failed to download video: $_"
    Write-Cmd "[ERROR]$_"
    exit
}
)";

// Sends a request to the API and receives a response(file and progress) using CProcessManager and a PowerShell script
class CSora2
{
public:
	std::wstring m_key = L"";
	std::wstring m_prompt = L"AVS - best media suite.";
	std::wstring m_seconds = L"4";
	std::wstring m_resolution = L"1280x720";
	std::wstring m_file = L"";
	std::wstring m_model = L"sora-2";
	DWORD m_start_time;

	NSProcesses::CProcessManager* m_manager = nullptr;

	CSora2() = default;
	CSora2(const CSora2&) = delete;
	CSora2(CSora2&&) = delete;
	~CSora2();

	void Process(NSProcesses::CProcessRunnerCallback* callback, const std::wstring& workDirectory);

	void FakeStart();
	int GetFakeProgress();

protected:
	std::wstring CreateWorkDirectory();
	std::wstring GetCurrentDateTime();
};
