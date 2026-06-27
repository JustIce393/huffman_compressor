$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Force -Path .\test_temp | Out-Null

function Verify-File($Name, $ContentBytes) {
    $orig = ".\test_temp\$Name.orig"
    $huff = ".\test_temp\$Name.huff"
    $restored = ".\test_temp\$Name.restored"

    [System.IO.File]::WriteAllBytes($orig, $ContentBytes)

    # Compress
    .\huffman_compressor.exe compress $orig $huff | Out-Null

    # Decompress
    .\huffman_compressor.exe decompress $huff $restored | Out-Null

    # Verify
    if (-not (Test-Path $restored)) {
        Write-Error "Test $Name failed: Restored file does not exist"
        return $false
    }

    $hash1 = (Get-FileHash -Algorithm SHA256 -Path $orig).Hash
    $hash2 = (Get-FileHash -Algorithm SHA256 -Path $restored).Hash

    if ($hash1 -ne $hash2) {
        Write-Error "Test $Name failed: Hash mismatch"
        return $false
    }

    $origSize = (Get-Item $orig).Length
    $huffSize = (Get-Item $huff).Length
    $saved = 0.0
    if ($origSize -gt 0) {
        $saved = (1.0 - ($huffSize / $origSize)) * 100.0
    }

    Write-Host "Test $Name passed: $origSize bytes -> $huffSize bytes ($([Math]::Round($saved, 2))% saved)"
    return $true
}

$allPassed = $true

# Test 1: Empty file
$bytes1 = [System.Byte[]]@()
$res = Verify-File "empty" $bytes1
$allPassed = $allPassed -and $res

# Test 2: Repetitive single character
$bytes2 = [System.Text.Encoding]::UTF8.GetBytes("a" * 100)
$res = Verify-File "single_char" $bytes2
$allPassed = $allPassed -and $res

# Test 3: Standard English text
$bytes3 = [System.Text.Encoding]::UTF8.GetBytes("The quick brown fox jumps over the lazy dog. " * 50)
$res = Verify-File "lorem" $bytes3
$allPassed = $allPassed -and $res

# Test 4: All possible byte values (0-255)
$bytes4 = [System.Byte[]](0..255)
$res = Verify-File "all_bytes" $bytes4
$allPassed = $allPassed -and $res

# Test 5: Repetitive structured bytes
$bytes5 = [System.Byte[]]( (0..9) * 100 )
$res = Verify-File "structured" $bytes5
$allPassed = $allPassed -and $res

if ($allPassed) {
    Write-Host "`nAll tests passed successfully! Cleaning up."
    Remove-Item -Recurse -Force .\test_temp | Out-Null
} else {
    Write-Error "`nSome tests failed."
    exit 1
}
