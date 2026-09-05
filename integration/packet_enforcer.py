from dataclasses import dataclass
from datetime import datetime, timezone


@dataclass
class EnforcementResult:
    """
    Result of applying a security decision to a packet.
    """

    decision: str
    action: str
    forwarded: bool
    packet: str
    timestamp: str
    reason: str


class PacketEnforcer:
    """
    Controlled packet enforcement layer for VoltGuard.

    The enforcer converts the Decision Engine result into
    an explicit security action.

    ALLOW -> FORWARD
    BLOCK -> DROP

    This implementation uses a simulated OT endpoint.
    It does not send packets onto a real industrial network.
    """

    def __init__(self):
        self.forwarded_packets = []
        self.dropped_packets = []

    def enforce(
        self,
        packet: str,
        decision: str,
        reason: str = "",
    ) -> EnforcementResult:
        """
        Apply an ALLOW or BLOCK decision to a packet.
        """

        normalized_decision = decision.strip().upper()

        timestamp = datetime.now(
            timezone.utc
        ).isoformat()

        # --------------------------------------------------
        # ALLOW -> FORWARD
        # --------------------------------------------------

        if normalized_decision == "ALLOW":

            self.forwarded_packets.append(packet)

            return EnforcementResult(
                decision="ALLOW",
                action="FORWARD",
                forwarded=True,
                packet=packet,
                timestamp=timestamp,
                reason=(
                    reason
                    if reason
                    else "Command approved for OT endpoint"
                ),
            )

        # --------------------------------------------------
        # BLOCK -> DROP
        # --------------------------------------------------

        if normalized_decision == "BLOCK":

            self.dropped_packets.append(packet)

            return EnforcementResult(
                decision="BLOCK",
                action="DROP",
                forwarded=False,
                packet=packet,
                timestamp=timestamp,
                reason=(
                    reason
                    if reason
                    else "Command rejected by security policy"
                ),
            )

        # --------------------------------------------------
        # Unknown decision -> fail closed
        # --------------------------------------------------

        self.dropped_packets.append(packet)

        return EnforcementResult(
            decision="BLOCK",
            action="DROP",
            forwarded=False,
            packet=packet,
            timestamp=timestamp,
            reason=(
                "Unknown security decision; "
                "packet rejected using fail-closed policy"
            ),
        )

    def get_statistics(self):
        """
        Return enforcement statistics.
        """

        return {
            "forwarded_packets": len(
                self.forwarded_packets
            ),
            "dropped_packets": len(
                self.dropped_packets
            ),
            "total_packets": (
                len(self.forwarded_packets)
                + len(self.dropped_packets)
            ),
        }


def main():
    """
    Demonstrate the enforcement layer with
    one allowed and one blocked command.
    """

    enforcer = PacketEnforcer()

    safe_packet = (
        "0001000000060106000103E8"
    )

    dangerous_packet = (
        "000100000006010600010FA0"
    )

    print("=" * 70)
    print("             VOLTGUARD PACKET ENFORCER")
    print("=" * 70)

    # --------------------------------------------------
    # Safe command
    # --------------------------------------------------

    safe_result = enforcer.enforce(
        packet=safe_packet,
        decision="ALLOW",
        reason=(
            "Command keeps the physical system "
            "within safe limits"
        ),
    )

    print()
    print("[SAFE COMMAND]")
    print("-" * 70)
    print(f"Decision : {safe_result.decision}")
    print(f"Action   : {safe_result.action}")
    print(f"Forwarded: {safe_result.forwarded}")
    print(f"Packet   : {safe_result.packet}")
    print(f"Reason   : {safe_result.reason}")

    # --------------------------------------------------
    # Dangerous command
    # --------------------------------------------------

    dangerous_result = enforcer.enforce(
        packet=dangerous_packet,
        decision="BLOCK",
        reason=(
            "Pressure exceeds safe limit: 122.67 bar"
        ),
    )

    print()
    print("[DANGEROUS COMMAND]")
    print("-" * 70)
    print(f"Decision : {dangerous_result.decision}")
    print(f"Action   : {dangerous_result.action}")
    print(f"Forwarded: {dangerous_result.forwarded}")
    print(f"Packet   : {dangerous_result.packet}")
    print(f"Reason   : {dangerous_result.reason}")

    # --------------------------------------------------
    # Statistics
    # --------------------------------------------------

    statistics = enforcer.get_statistics()

    print()
    print("=" * 70)
    print("ENFORCEMENT STATISTICS")
    print("=" * 70)

    print(
        f"Forwarded packets : "
        f"{statistics['forwarded_packets']}"
    )

    print(
        f"Dropped packets   : "
        f"{statistics['dropped_packets']}"
    )

    print(
        f"Total packets     : "
        f"{statistics['total_packets']}"
    )

    print("=" * 70)


if __name__ == "__main__":
    main()