import importlib
import shutil
import subprocess
import sys
from pathlib import Path


# --------------------------------------------------
# Project paths
# --------------------------------------------------

PROJECT_ROOT = Path(__file__).resolve().parents[1]

PACKET_INTERCEPTOR = (
    PROJECT_ROOT
    / "build-mingw"
    / "VoltGuard.exe"
)

DASHBOARD_EXECUTABLE = (
    PROJECT_ROOT
    / "dashboard"
    / "build"
    / "VoltGuardDashboard.exe"
)

RUST_PROJECT = (
    PROJECT_ROOT
    / "decision_engine"
    / "Cargo.toml"
)

LOG_DIRECTORY = (
    PROJECT_ROOT
    / "logs"
)


# --------------------------------------------------
# Result tracking
# --------------------------------------------------

results = []


def check(name, passed, detail=""):
    """
    Record and display one health-check result.
    """

    status = "PASS" if passed else "FAIL"

    print(
        f"[{status}] {name}"
        + (f" - {detail}" if detail else "")
    )

    results.append(passed)


# --------------------------------------------------
# Python environment
# --------------------------------------------------

def check_python():
    version = sys.version_info

    passed = version >= (3, 9)

    check(
        "Python environment",
        passed,
        f"Python {version.major}.{version.minor}.{version.micro}",
    )


# --------------------------------------------------
# C++ Packet Interceptor
# --------------------------------------------------

def check_packet_interceptor():
    passed = PACKET_INTERCEPTOR.exists()

    detail = (
        str(PACKET_INTERCEPTOR)
        if passed
        else "executable not found"
    )

    check(
        "C++ Packet Interceptor",
        passed,
        detail,
    )


# --------------------------------------------------
# Rust Decision Engine
# --------------------------------------------------

def check_rust():
    cargo = shutil.which("cargo")

    if cargo is None:
        check(
            "Rust Decision Engine",
            False,
            "cargo executable not found in PATH",
        )
        return

    if not RUST_PROJECT.exists():
        check(
            "Rust Decision Engine",
            False,
            "Cargo.toml not found",
        )
        return

    try:
        result = subprocess.run(
            [
                cargo,
                "check",
                "--manifest-path",
                str(RUST_PROJECT),
                "--quiet",
            ],
            cwd=PROJECT_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=60,
        )

        passed = result.returncode == 0

        detail = (
            "cargo check passed"
            if passed
            else "cargo check failed"
        )

        check(
            "Rust Decision Engine",
            passed,
            detail,
        )

    except (
        subprocess.SubprocessError,
        OSError,
    ) as error:

        check(
            "Rust Decision Engine",
            False,
            str(error),
        )


# --------------------------------------------------
# Python Physics Engine
# --------------------------------------------------

def check_physics_engine():
    """
    Verify that the PhysicsEngine class can be imported
    using the same project-level Python import structure
    used by the VoltGuard integration code.
    """

    try:
        if str(PROJECT_ROOT) not in sys.path:
            sys.path.insert(0, str(PROJECT_ROOT))

        module = importlib.import_module(
            "physics_engine.physics_engine"
        )

        passed = hasattr(
            module,
            "PhysicsEngine",
        )

        detail = (
            "PhysicsEngine available"
            if passed
            else "PhysicsEngine class not found"
        )

        check(
            "Python Physics Engine",
            passed,
            detail,
        )

    except Exception as error:

        check(
            "Python Physics Engine",
            False,
            str(error),
        )


# --------------------------------------------------
# Security Logger
# --------------------------------------------------

def check_security_logger():
    logger_path = (
        PROJECT_ROOT
        / "integration"
        / "security_logger.py"
    )

    passed = logger_path.exists()

    detail = (
        "security_logger.py available"
        if passed
        else "security_logger.py not found"
    )

    check(
        "Security Logger",
        passed,
        detail,
    )


# --------------------------------------------------
# Qt Dashboard
# --------------------------------------------------

def check_dashboard():
    passed = DASHBOARD_EXECUTABLE.exists()

    detail = (
        str(DASHBOARD_EXECUTABLE)
        if passed
        else "Qt dashboard executable not found"
    )

    check(
        "Qt OT Dashboard",
        passed,
        detail,
    )


# --------------------------------------------------
# Security log directory
# --------------------------------------------------

def check_log_directory():
    if not LOG_DIRECTORY.exists():
        LOG_DIRECTORY.mkdir(
            parents=True,
            exist_ok=True,
        )

    check(
        "Security Log Directory",
        LOG_DIRECTORY.is_dir(),
        str(LOG_DIRECTORY),
    )


# --------------------------------------------------
# Main health check
# --------------------------------------------------

def main():

    print()
    print("=" * 50)
    print("        VOLTGUARD HEALTH CHECK")
    print("=" * 50)

    print()

    check_python()
    check_packet_interceptor()
    check_rust()
    check_physics_engine()
    check_security_logger()
    check_dashboard()
    check_log_directory()

    print()
    print("-" * 50)

    if all(results):
        print(
            "VOLTGUARD SYSTEM STATUS: READY"
        )
        print("-" * 50)
        print()
        return 0

    print(
        "VOLTGUARD SYSTEM STATUS: NOT READY"
    )
    print("-" * 50)
    print()
    return 1


if __name__ == "__main__":
    sys.exit(main())