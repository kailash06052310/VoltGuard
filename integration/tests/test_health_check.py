import subprocess
import sys
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]

HEALTH_CHECK = (
    PROJECT_ROOT
    / "integration"
    / "health_check.py"
)


class TestHealthCheck(unittest.TestCase):

    def test_health_check_reports_ready(self):

        result = subprocess.run(
            [
                sys.executable,
                str(HEALTH_CHECK),
            ],
            cwd=PROJECT_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=120,
        )

        self.assertEqual(
            result.returncode,
            0,
            msg=result.stderr,
        )

        output = result.stdout

        self.assertIn(
            "[PASS] Python environment",
            output,
        )

        self.assertIn(
            "[PASS] C++ Packet Interceptor",
            output,
        )

        self.assertIn(
            "[PASS] Rust Decision Engine",
            output,
        )

        self.assertIn(
            "[PASS] Python Physics Engine",
            output,
        )

        self.assertIn(
            "[PASS] Security Logger",
            output,
        )

        self.assertIn(
            "[PASS] Qt OT Dashboard",
            output,
        )

        self.assertIn(
            "VOLTGUARD SYSTEM STATUS: READY",
            output,
        )


if __name__ == "__main__":
    unittest.main()