import subprocess
import sys
from pathlib import Path


# --------------------------------------------------
# Project paths
# --------------------------------------------------

PROJECT_ROOT = Path(__file__).resolve().parents[1]

BUILD_DIRECTORY = (
    PROJECT_ROOT / "build-mingw"
)

DECISION_ENGINE = (
    PROJECT_ROOT
    / "decision_engine"
    / "Cargo.toml"
)


# --------------------------------------------------
# Test runner
# --------------------------------------------------

def run_command(name, command, cwd=PROJECT_ROOT):
    """
    Execute one test command and display its result.
    """

    print()
    print("=" * 70)
    print(f"TEST: {name}")
    print("=" * 70)

    print()
    print("Command:")
    print(" ".join(str(item) for item in command))
    print()

    result = subprocess.run(
        command,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )

    if result.stdout:
        print(result.stdout)

    if result.stderr:
        print(result.stderr)

    if result.returncode == 0:
        print(f"[PASS] {name}")
        return True

    print(f"[FAIL] {name}")
    return False


# --------------------------------------------------
# C++ tests
# --------------------------------------------------

def run_cpp_tests():
    """
    Run the C++ Packet Interceptor tests through CTest.
    """

    if not BUILD_DIRECTORY.exists():
        print(
            "[FAIL] C++ tests - build-mingw directory not found"
        )
        return False

    return run_command(
        "C++ Packet Interceptor",
        [
            "ctest",
            "--test-dir",
            str(BUILD_DIRECTORY),
            "--output-on-failure",
        ],
    )


# --------------------------------------------------
# Rust tests
# --------------------------------------------------

def run_rust_tests():
    """
    Run the Rust Decision Engine unit tests.
    """

    if not DECISION_ENGINE.exists():
        print(
            "[FAIL] Rust tests - Cargo.toml not found"
        )
        return False

    return run_command(
        "Rust Decision Engine",
        [
            "cargo",
            "test",
            "--manifest-path",
            str(DECISION_ENGINE),
        ],
    )


# --------------------------------------------------
# Python tests
# --------------------------------------------------

def run_python_tests():
    """
    Run the complete Python integration test suite.
    """

    return run_command(
        "Python Integration Tests",
        [
            sys.executable,
            "-m",
            "unittest",
            "discover",
            "-s",
            "integration/tests",
            "-p",
            "test_*.py",
            "-v",
        ],
    )


# --------------------------------------------------
# Main
# --------------------------------------------------

def main():

    print()
    print("=" * 70)
    print("                    VOLTGUARD TEST SUITE")
    print("=" * 70)

    print()
    print(
        "Running C++, Rust and Python validation..."
    )

    results = []

    # --------------------------------------------------
    # C++
    # --------------------------------------------------

    results.append(
        run_cpp_tests()
    )

    # --------------------------------------------------
    # Rust
    # --------------------------------------------------

    results.append(
        run_rust_tests()
    )

    # --------------------------------------------------
    # Python
    # --------------------------------------------------

    results.append(
        run_python_tests()
    )

    # --------------------------------------------------
    # Final summary
    # --------------------------------------------------

    passed = sum(results)
    total = len(results)

    print()
    print("=" * 70)
    print("                    TEST SUMMARY")
    print("=" * 70)

    print()
    print(
        f"Test groups passed: {passed}/{total}"
    )

    print()

    if all(results):
        print(
            "[PASS] ALL VOLTGUARD TESTS PASSED"
        )
        print()
        print(
            "System validation completed successfully."
        )
        print("=" * 70)
        return 0

    print(
        "[FAIL] VOLTGUARD TEST SUITE FAILED"
    )
    print()
    print(
        "One or more test groups require attention."
    )
    print("=" * 70)

    return 1


if __name__ == "__main__":
    sys.exit(main())