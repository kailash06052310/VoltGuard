import json
from datetime import datetime, timezone
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
LOG_DIR = PROJECT_ROOT / "logs"
LOG_FILE = LOG_DIR / "security_events.jsonl"


class SecurityLogger:
    """
    Records VoltGuard security decisions as JSON Lines.

    Every packet decision is stored as one JSON object per line.
    """

    def __init__(self, log_file=LOG_FILE):
        self.log_file = Path(log_file)
        self.log_file.parent.mkdir(
            parents=True,
            exist_ok=True,
        )

    def log_event(
        self,
        packet,
        pump_rpm,
        predicted_state,
        decision,
    ):
        """
        Store one packet-processing event.
        """

        event = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "packet": packet,
            "pump_rpm": pump_rpm,
            "predicted_state": predicted_state,
            "decision": decision["decision"],
            "reason": decision["reason"],
        }

        with self.log_file.open(
            "a",
            encoding="utf-8",
        ) as file:

            file.write(
                json.dumps(event)
                + "\n"
            )

        return event

    def read_events(self):
        """
        Read all previously recorded security events.
        """

        if not self.log_file.exists():
            return []

        events = []

        with self.log_file.open(
            "r",
            encoding="utf-8",
        ) as file:

            for line in file:
                line = line.strip()

                if not line:
                    continue

                try:
                    events.append(
                        json.loads(line)
                    )
                except json.JSONDecodeError:
                    continue

        return events


if __name__ == "__main__":
    logger = SecurityLogger()

    sample_event = logger.log_event(
        packet="000100000006010600010FA0",
        pump_rpm=4000,
        predicted_state={
            "pump_rpm": 4000.0,
            "valve_opening": 60.0,
            "flow_rate": 80.0,
            "pressure": 122.67,
            "safe": False,
        },
        decision={
            "decision": "BLOCK",
            "reason": "Pressure exceeds safe limit: 122.67 bar",
        },
    )

    print("Security event recorded:")
    print(json.dumps(sample_event, indent=2))