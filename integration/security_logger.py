import json
from datetime import datetime, timezone
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]

LOG_DIRECTORY = PROJECT_ROOT / "logs"
LOG_FILE = LOG_DIRECTORY / "security_events.jsonl"


class SecurityLogger:
    """
    Records VoltGuard security decisions and physical-state data.

    Each event contains:

    - Timestamp
    - Original packet
    - Pump RPM
    - Predicted physical state
    - Simulated actual physical state
    - Security decision
    - Decision reason
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
        actual_state=None,
    ):
        """
        Write one security event to the JSONL log.
        """

        timestamp = datetime.now(
            timezone.utc
        ).isoformat()

        event = {
            "timestamp": timestamp,
            "packet": packet,
            "pump_rpm": pump_rpm,
            "predicted_state": predicted_state,
            "actual_state": actual_state,
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


if __name__ == "__main__":

    logger = SecurityLogger()

    predicted_state = {
        "pump_rpm": 4000.0,
        "valve_opening": 50.0,
        "flow_rate": 66.67,
        "pressure": 126.67,
        "safe": False,
    }

    actual_state = {
        "pump_rpm": 3400.0,
        "valve_opening": 50.0,
        "flow_rate": 56.67,
        "pressure": 110.67,
        "safe": False,
    }

    decision = {
        "decision": "BLOCK",
        "reason": "Pressure exceeds safe limit: 126.67 bar",
    }

    event = logger.log_event(
        packet="000100000006010600010FA0",
        pump_rpm=4000.0,
        predicted_state=predicted_state,
        actual_state=actual_state,
        decision=decision,
    )

    print("=== VoltGuard Security Logger ===")
    print()
    print("Security event recorded:")
    print(json.dumps(event, indent=2))