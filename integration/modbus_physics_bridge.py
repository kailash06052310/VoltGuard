import json
import subprocess
from pathlib import Path

from physics_engine.physics_engine import PhysicsEngine


PROJECT_ROOT = Path(__file__).resolve().parent.parent
CARGO_MANIFEST = PROJECT_ROOT / "decision_engine" / "Cargo.toml"


class ModbusPhysicsBridge:
    """
    Connects:

        Modbus Command
              ↓
        Python Physics Engine
              ↓
        Rust Decision Engine
              ↓
          ALLOW / BLOCK
    """

    def __init__(self):
        self.physics_engine = PhysicsEngine()

    def evaluate_command(self, pump_rpm: float, duration: float = 60):
        """
        Evaluate an industrial command through the complete
        Python Physics Engine → Rust Decision Engine pipeline.
        """

        # --------------------------------------------------
        # STEP 1: Run the existing Python Physics Engine
        # --------------------------------------------------

        physics_result = self.physics_engine.evaluate_command(
            pump_rpm=pump_rpm,
            duration=duration
        )

        predicted_state = physics_result["predicted_state"]

        # --------------------------------------------------
        # STEP 2: Prepare state for Rust Decision Engine
        # --------------------------------------------------

        state = {
            "pump_rpm": predicted_state["pump_rpm"],
            "valve_opening": predicted_state["valve_opening"],
            "flow_rate": predicted_state["flow_rate"],
            "pressure": predicted_state["pressure"],
            "safe": predicted_state["safe"],
        }

        # --------------------------------------------------
        # STEP 3: Send physical state to Rust
        # --------------------------------------------------

        decision = self._run_rust_decision_engine(state)

        # --------------------------------------------------
        # STEP 4: Combine everything into one result
        # --------------------------------------------------

        return {
            "command": {
                "pump_rpm": pump_rpm,
                "duration": duration,
            },
            "predicted_state": state,
            "decision": decision,
        }

    def _run_rust_decision_engine(self, state: dict):
        """
        Send JSON input to the Rust Decision Engine
        and receive an ALLOW/BLOCK JSON response.
        """

        input_json = json.dumps(state)

        command = [
            "cargo",
            "run",
            "--quiet",
            "--manifest-path",
            str(CARGO_MANIFEST),
            "--",
            "--json",
        ]

        try:
            process = subprocess.run(
                command,
                input=input_json,
                text=True,
                capture_output=True,
                cwd=PROJECT_ROOT,
                timeout=30,
            )

        except subprocess.TimeoutExpired:
            return {
                "decision": "BLOCK",
                "reason": "Rust Decision Engine timed out",
            }

        except FileNotFoundError:
            return {
                "decision": "BLOCK",
                "reason": "Cargo was not found in PATH",
            }

        # --------------------------------------------------
        # Rust process failed
        # --------------------------------------------------

        if process.returncode != 0:
            return {
                "decision": "BLOCK",
                "reason": (
                    "Rust Decision Engine failed: "
                    + process.stderr.strip()
                ),
            }

        # --------------------------------------------------
        # Parse Rust JSON response
        # --------------------------------------------------

        try:
            response = json.loads(process.stdout)

        except json.JSONDecodeError:
            return {
                "decision": "BLOCK",
                "reason": (
                    "Rust Decision Engine returned invalid JSON: "
                    + process.stdout.strip()
                ),
            }

        # --------------------------------------------------
        # Validate response
        # --------------------------------------------------

        if "result" not in response:
            return {
                "decision": "BLOCK",
                "reason": "Invalid response structure from Rust engine",
            }

        return response["result"]


def print_result(title: str, result: dict):
    """
    Display a complete VoltGuard decision.
    """

    print()
    print("=" * 65)
    print(f" {title}")
    print("=" * 65)

    print()
    print("COMMAND")
    print("-" * 65)

    print(f"Pump RPM       : {result['command']['pump_rpm']}")
    print(f"Duration       : {result['command']['duration']} seconds")

    print()
    print("PREDICTED PHYSICAL STATE")
    print("-" * 65)

    state = result["predicted_state"]

    print(f"Pump RPM       : {state['pump_rpm']}")
    print(f"Valve Opening  : {state['valve_opening']}%")
    print(f"Flow Rate      : {state['flow_rate']}")
    print(f"Pressure       : {state['pressure']} bar")
    print(f"Physics Safe   : {state['safe']}")

    print()
    print("RUST DECISION ENGINE")
    print("-" * 65)

    decision = result["decision"]

    print(f"Decision       : {decision['decision']}")
    print(f"Reason         : {decision['reason']}")

    print()
    print("=" * 65)


def main():
    print()
    print("=============================================================")
    print("       VOLTGUARD MODBUS → PHYSICS → DECISION BRIDGE")
    print("=============================================================")

    bridge = ModbusPhysicsBridge()

    # ----------------------------------------------------------
    # NORMAL INDUSTRIAL COMMAND
    # ----------------------------------------------------------

    normal_result = bridge.evaluate_command(
        pump_rpm=1000,
        duration=60,
    )

    print_result(
        "NORMAL COMMAND",
        normal_result,
    )

    # ----------------------------------------------------------
    # DANGEROUS INDUSTRIAL COMMAND
    # ----------------------------------------------------------

    dangerous_result = bridge.evaluate_command(
        pump_rpm=4000,
        duration=60,
    )

    print_result(
        "DANGEROUS COMMAND",
        dangerous_result,
    )


if __name__ == "__main__":
    main()