import os
import subprocess
import filecmp
import shutil
import random

def run_cmd(args):
    result = subprocess.run(args, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Command failed: {' '.join(args)}")
        print(f"Stdout:\n{result.stdout}")
        print(f"Stderr:\n{result.stderr}")
        return False, result.stdout, result.stderr
    return True, result.stdout, result.stderr

def print_section(title):
    print("=" * 60)
    print(f" {title}")
    print("=" * 60)

def main():
    test_dir = "test_temp"
    os.makedirs(test_dir, exist_ok=True)
    
    executable = "./huffman_compressor.exe"
    if not os.path.exists(executable):
        executable = "huffman_compressor.exe"
    
    # 1. Define test cases
    test_cases = [
        ("empty", b""),
        ("single_char", b"a" * 100),
        ("simple_text", b"hello world! hello world! hello world!"),
        ("all_bytes", bytes(range(256))),
        ("random_binary", os.urandom(1024 * 50)), # 50 KB random
        ("structured_repeats", (b"ABC" * 1000 + b"DEF" * 1000) * 10), # repetitive structure
    ]
    
    # Add a larger text case (approx 500 KB of repetitive text)
    large_text = b"The quick brown fox jumps over the lazy dog. " * 10000
    test_cases.append(("large_text", large_text))
    
    success_count = 0
    total_cases = len(test_cases)
    
    print_section("RUNNING HUFFMAN COMPRESSOR TESTS")
    
    for name, content in test_cases:
        print(f"Test case: {name} ({len(content)} bytes)")
        
        orig_path = os.path.join(test_dir, f"{name}.orig")
        comp_path = os.path.join(test_dir, f"{name}.huff")
        dest_path = os.path.join(test_dir, f"{name}.restored")
        
        # Write original
        with open(orig_path, "wb") as f:
            f.write(content)
            
        # Compress
        ok, stdout, stderr = run_cmd([executable, "compress", orig_path, comp_path])
        if not ok:
            print(f"  [FAIL] Compression failed for {name}")
            continue
            
        # Decompress
        ok, stdout_dec, stderr_dec = run_cmd([executable, "decompress", comp_path, dest_path])
        if not ok:
            print(f"  [FAIL] Decompression failed for {name}")
            continue
            
        # Verify sizes and equality
        if not os.path.exists(dest_path):
            print(f"  [FAIL] Restored file does not exist")
            continue
            
        files_match = filecmp.cmp(orig_path, dest_path, shallow=False)
        if not files_match:
            print(f"  [FAIL] Restored content does not match original for {name}")
            continue
            
        orig_size = len(content)
        comp_size = os.path.getsize(comp_path)
        
        # Compression ratio
        saved = 0.0
        if orig_size > 0:
            saved = (1.0 - (comp_size / orig_size)) * 100.0
            
        print(f"  [PASS] Restored matches original perfectly!")
        print(f"         Size: {orig_size} bytes -> {comp_size} bytes ({saved:.2f}% space saved)")
        success_count += 1
        print("-" * 40)
        
    print_section("SUMMARY")
    print(f"Passed: {success_count} / {total_cases}")
    
    # Cleanup if all passed
    if success_count == total_cases:
        print("\nAll tests passed successfully! Cleaning up test files.")
        shutil.rmtree(test_dir)
        exit(0)
    else:
        print("\nSome tests failed. Keeping test files in 'test_temp' directory for debugging.")
        exit(1)

if __name__ == "__main__":
    main()
