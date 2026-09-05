import subprocess
import sys
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]

DEMO_RUNNER = (
    PROJECT_ROOT
    / "integration"
    / "demo_runner.py"
)


class TestDemoRunner(unittest.TestCase):

    def test_demo_completes_successfully(self):

        result = subprocess.run(
            [
                sys.executable,
                str(DEMO_RUNNER),
            ],
            cwd=PROJECT_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=60,
        )

        self.assertEqual(
            result.returncode,
            0,
            msg=result.stderr,
        )

        output = result.stdout

        self.assertIn(
            "[PASS] Safe command processed successfully",
            output,
        )

        self.assertIn(
            "[PASS] Dangerous command processed successfully",
            output,
        )

        self.assertIn(
            "[PASS] VOLTGUARD END-TO-END DEMO COMPLETED",
            output,
        )

    def test_demo_contains_expected_security_actions(self):

        result = subprocess.run(
            [
                sys.executable,
                str(DEMO_RUNNER),
            ],
            cwd=PROJECT_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            timeout=60,
        )

        self.assertEqual(
            result.returncode,
            0,
            msg=result.stderr,
        )

        output = result.stdout

        self.assertIn(
            "ACTION: FORWARD",
            output,
        )

        self.assertIn(
            "ACTION: DROP",
            output,
        )

        self.assertIn(
            "OT ENDPOINT: COMMAND ACCEPTED",
            output,
        )

        self.assertIn(
            "OT ENDPOINT: COMMAND REJECTED",
            output,
        )


if __name__ == "__main__":
    unittest.main()