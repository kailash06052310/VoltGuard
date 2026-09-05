import subprocess
import sys
from pathlib import Path


# --------------------------------------------------
# Project paths
# --------------------------------------------------

PROJECT_ROOT = Path(__file__).resolve().parents[1]

PIPELINE = (
    PROJECT_ROOT
    / "integration"
    / "voltguard_pipeline.py"
)


# --------------------------------------------------
# Demonstration packets
# --------------------------------------------------

SAFE_PACKET = (
    "0001000000060106000103E8"
)

DANGEROUS_PACKET = (
    "000100000006010600010FA0"
)


def run_demo_packet(
    name: str,
    packet: str,
):
    """
    Run one packet through the complete
    VoltGuard security pipeline.
    """

    print()
    print("=" * 70)
    print(f"              {name}")
    print("=" * 70)

    print()
    print(f"Packet: {packet}")

    result = subprocess.run(
        [
            sys.executable,
            str(PIPELINE),
            packet,
        ],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        timeout=30,
    )

    print(result.stdout)

    if result.returncode != 0:
        print()
        print("[ERROR] VoltGuard pipeline failed.")

        if result.stderr:
            print(result.stderr)

        return False

    return True


def main():
    """
    Execute the complete VoltGuard demonstration.

    The demo sends one safe command and one dangerous
    command through the simulated security pipeline.
    """

    print()
    print("=" * 70)
    print("                  VOLTGUARD DEMO")
    print("=" * 70)

    print()
    print("This demonstration processes two simulated")
    print("Modbus/TCP commands:")
    print()
    print("1. Safe command     : 1000 RPM")
    print("2. Dangerous command: 4000 RPM")

    print()
    print("The commands will pass through:")
    print()
    print(
        "C++ Parser -> Physics -> Rust Decision "
        "-> Enforcement -> Logger"
    )

    # --------------------------------------------------
    # Safe command
    # --------------------------------------------------

    safe_success = run_demo_packet(
        "SAFE COMMAND - EXPECT ALLOW",
        SAFE_PACKET,
    )

    # --------------------------------------------------
    # Dangerous command
    # --------------------------------------------------

    dangerous_success = run_demo_packet(
        "DANGEROUS COMMAND - EXPECT BLOCK",
        DANGEROUS_PACKET,
    )

    # --------------------------------------------------
    # Final result
    # --------------------------------------------------

    print()
    print("=" * 70)
    print("                  DEMO SUMMARY")
    print("=" * 70)

    print()

    if safe_success:
        print(
            "[PASS] Safe command processed successfully"
        )
    else:
        print(
            "[FAIL] Safe command processing failed"
        )

    if dangerous_success:
        print(
            "[PASS] Dangerous command processed successfully"
        )
    else:
        print(
            "[FAIL] Dangerous command processing failed"
        )

    print()

    if safe_success and dangerous_success:
        print(
            "[PASS] VOLTGUARD END-TO-END DEMO COMPLETED"
        )
    else:
        print(
            "[FAIL] VOLTGUARD DEMO FAILED"
        )

    print()

    print(
        "Open the Qt dashboard to view the resulting "
        "security events and physics trend."
    )

    print("=" * 70)


if __name__ == "__main__":
    main()