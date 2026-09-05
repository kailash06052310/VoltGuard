import json
import subprocess
import sys
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[2]

PIPELINE = (
    PROJECT_ROOT
    / "integration"
    / "voltguard_pipeline.py"
)

LOG_FILE = (
    PROJECT_ROOT
    / "logs"
    / "security_events.jsonl"
)

SAFE_PACKET = "0001000000060106000103E8"
DANGEROUS_PACKET = "000100000006010600010FA0"


class TestVoltGuardPipeline(unittest.TestCase):

    def run_pipeline(self, packet):
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
        )

        self.assertEqual(
            result.returncode,
            0,
            msg=result.stderr,
        )

        return result.stdout

    def test_safe_packet_is_allowed(self):
        output = self.run_pipeline(SAFE_PACKET)

        self.assertIn(
            "Pump RPM      : 1000.0",
            output,
        )

        self.assertIn(
            "Decision      : ALLOW",
            output,
        )

        self.assertIn(
            "ACTION: FORWARD",
            output,
        )

    def test_dangerous_packet_is_blocked(self):
        output = self.run_pipeline(DANGEROUS_PACKET)

        self.assertIn(
            "Pump RPM      : 4000.0",
            output,
        )

        self.assertIn(
            "Decision      : BLOCK",
            output,
        )

        self.assertIn(
            "ACTION: DROP",
            output,
        )

        self.assertIn(
            "SECURITY ALERT GENERATED",
            output,
        )

    def test_security_log_is_generated(self):
        self.run_pipeline(DANGEROUS_PACKET)

        self.assertTrue(
            LOG_FILE.exists(),
            "Security log file was not generated.",
        )

        lines = (
            LOG_FILE
            .read_text(encoding="utf-8")
            .strip()
            .splitlines()
        )

        self.assertGreater(
            len(lines),
            0,
            "Security log is empty.",
        )

        event = json.loads(lines[-1])

        self.assertEqual(
            event["pump_rpm"],
            4000,
        )

        self.assertEqual(
            event["decision"],
            "BLOCK",
        )


if __name__ == "__main__":
    unittest.main()