import json
import subprocess
import sys
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]

if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))


from physics_engine.physics_engine import PhysicsEngine


class ModbusPhysicsBridge:
    """
    Connects the Modbus command processing layer
    with the Python physics engine and Rust decision engine.

    Data flow:

        Modbus command
              |
              v
        Physics Engine
              |
       +------+------+
       |             |
       v             v
    Predicted      Actual
       |             |
       +------+------+
              |
              v
       Rust Decision Engine
    """

    def __init__(self):
        self.physics_engine = PhysicsEngine()

    def evaluate_command(self, pump_rpm, duration=60):
        """
        Evaluate a pump command using the physics engine
        and then pass the predicted physical state to
        the Rust decision engine.
        """

        # --------------------------------------------------
        # Physics simulation
        # --------------------------------------------------

        physics_result = self.physics_engine.evaluate_command(
            pump_rpm=pump_rpm,
            duration=duration,
        )

        predicted_state = physics_result["predicted_state"]
        actual_state = physics_result["actual_state"]

        # --------------------------------------------------
        # Convert predicted state to JSON
        # --------------------------------------------------

        state_json = json.dumps(predicted_state)

        # --------------------------------------------------
        # Rust Decision Engine
        # --------------------------------------------------

        process = subprocess.run(
            [
                "cargo",
                "run",
                "--quiet",
                "--manifest-path",
                str(
                    PROJECT_ROOT
                    / "decision_engine"
                    / "Cargo.toml"
                ),
                "--",
                "--json",
            ],
            input=state_json,
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT,
            timeout=30,
        )

        if process.returncode != 0:
            raise RuntimeError(
                "Rust Decision Engine failed:\n"
                + process.stderr
            )

        if not process.stdout.strip():
            raise RuntimeError(
                "Rust Decision Engine returned no output."
            )

        try:
            rust_result = json.loads(
                process.stdout
            )
        except json.JSONDecodeError as error:
            raise RuntimeError(
                "Invalid JSON returned by Rust Decision Engine:\n"
                + process.stdout
            ) from error

        # --------------------------------------------------
        # Final combined result
        # --------------------------------------------------

        rust_decision = rust_result["result"]

        return {
            "command": physics_result["command"],

            "predicted_state": predicted_state,

            "actual_state": actual_state,

            "safe": physics_result["safe"],

            "decision": {
                "decision": rust_decision["decision"],
                "reason": rust_decision["reason"],
            },
        }


if __name__ == "__main__":

    bridge = ModbusPhysicsBridge()

    print("=== VoltGuard Modbus → Physics → Rust Bridge ===")

    result = bridge.evaluate_command(
        pump_rpm=4000,
        duration=60,
    )

    print("\nCommand:")
    print(result["command"])

    print("\nPredicted physical state:")
    print(result["predicted_state"])

    print("\nSimulated actual physical state:")
    print(result["actual_state"])

    print("\nSecurity decision:")
    print(result["decision"])