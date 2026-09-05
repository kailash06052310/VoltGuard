import re
import subprocess
import sys
from pathlib import Path


# --------------------------------------------------
# Project paths
# --------------------------------------------------

PROJECT_ROOT = Path(__file__).resolve().parents[1]

if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))


from integration.modbus_physics_bridge import ModbusPhysicsBridge
from integration.security_logger import SecurityLogger


PACKET_INTERCEPTOR = (
    PROJECT_ROOT
    / "build-mingw"
    / "VoltGuard.exe"
)


class VoltGuardPipeline:
    """
    Complete VoltGuard processing pipeline.

    Modbus packet
        |
        v
    C++ Packet Interceptor
        |
        v
    Pump RPM
        |
        v
    Python Physics Engine
        |
        v
    Rust Decision Engine
        |
        v
    ALLOW / BLOCK
        |
        v
    Security Event Log
    """

    def __init__(self):
        self.bridge = ModbusPhysicsBridge()
        self.logger = SecurityLogger()

    def parse_packet(self, hex_packet: str):
        """
        Send a hexadecimal Modbus/TCP packet to the
        C++ Packet Interceptor and extract pump RPM.
        """

        if not PACKET_INTERCEPTOR.exists():
            raise FileNotFoundError(
                f"Packet interceptor not found: {PACKET_INTERCEPTOR}"
            )

        process = subprocess.run(
            [
                str(PACKET_INTERCEPTOR),
                hex_packet,
            ],
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT,
            timeout=10,
        )

        if process.returncode != 0:
            raise RuntimeError(
                "Packet parsing failed:\n"
                + process.stderr
            )

        output = process.stdout

        # Extract the value printed by the C++ parser.
        match = re.search(
            r"Pump RPM\s*:\s*([0-9]+(?:\.[0-9]+)?)",
            output,
        )

        if not match:
            raise ValueError(
                "Pump RPM could not be extracted from "
                "Packet Interceptor output."
            )

        pump_rpm = float(match.group(1))

        return {
            "pump_rpm": pump_rpm,
            "raw_output": output,
        }

    def process_packet(
        self,
        hex_packet: str,
        duration: float = 60,
    ):
        """
        Process one Modbus/TCP packet through the
        complete VoltGuard security pipeline.
        """

        print()
        print("=" * 70)
        print("              VOLTGUARD END-TO-END PIPELINE")
        print("=" * 70)

        # --------------------------------------------------
        # Incoming packet
        # --------------------------------------------------

        print()
        print("[1] Incoming Modbus/TCP Packet")
        print("-" * 70)
        print(hex_packet)

        # --------------------------------------------------
        # C++ Packet Interceptor
        # --------------------------------------------------

        print()
        print("[2] C++ Packet Interceptor")
        print("-" * 70)

        parsed = self.parse_packet(hex_packet)

        print(
            f"Pump RPM extracted from packet: "
            f"{parsed['pump_rpm']}"
        )

        # --------------------------------------------------
        # Python Physics Engine
        # --------------------------------------------------

        print()
        print("[3] Python Physics Engine")
        print("-" * 70)

        result = self.bridge.evaluate_command(
            pump_rpm=parsed["pump_rpm"],
            duration=duration,
        )

        state = result["predicted_state"]

        print(f"Pump RPM      : {state['pump_rpm']}")
        print(f"Valve Opening : {state['valve_opening']}%")
        print(f"Flow Rate     : {state['flow_rate']}")
        print(f"Pressure      : {state['pressure']} bar")
        print(f"Physics Safe  : {state['safe']}")

        # --------------------------------------------------
        # Rust Decision Engine
        # --------------------------------------------------

        print()
        print("[4] Rust Decision Engine")
        print("-" * 70)

        decision = result["decision"]

        print(f"Decision      : {decision['decision']}")
        print(f"Reason        : {decision['reason']}")

        # --------------------------------------------------
        # Security Event Logger
        # --------------------------------------------------

        print()
        print("[5] Security Event Logger")
        print("-" * 70)

        event = self.logger.log_event(
            packet=hex_packet,
            pump_rpm=parsed["pump_rpm"],
            predicted_state=state,
            decision=decision,
        )

        print(
            f"Event recorded at: {event['timestamp']}"
        )

        # --------------------------------------------------
        # Final security action
        # --------------------------------------------------

        print()
        print("=" * 70)

        if decision["decision"] == "ALLOW":

            print("                 [OK] COMMAND ALLOWED")
            print("                 ACTION: FORWARD")

        else:

            print("                 [BLOCKED] COMMAND BLOCKED")
            print("                 ACTION: DROP")
            print("                 SECURITY ALERT GENERATED")

        print("=" * 70)

        return {
            "packet": hex_packet,
            "pump_rpm": parsed["pump_rpm"],
            "predicted_state": state,
            "decision": decision,
            "security_event": event,
        }


def main():
    """
    Run the pipeline.

    If a packet is supplied as a command-line argument,
    process only that packet.

    Without an argument, run the two demonstration packets.
    """

    pipeline = VoltGuardPipeline()

    # --------------------------------------------------
    # Command-line mode
    # --------------------------------------------------

    if len(sys.argv) > 1:

        hex_packet = sys.argv[1].strip()

        pipeline.process_packet(
            hex_packet,
            duration=60,
        )

        return

    # --------------------------------------------------
    # Demo mode
    # --------------------------------------------------

    normal_packet = (
        "0001000000060106000103E8"
    )

    pipeline.process_packet(
        normal_packet,
        duration=60,
    )

    dangerous_packet = (
        "000100000006010600010FA0"
    )

    pipeline.process_packet(
        dangerous_packet,
        duration=60,
    )


if __name__ == "__main__":
    main()