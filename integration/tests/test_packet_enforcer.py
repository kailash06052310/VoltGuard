import unittest

from integration.packet_enforcer import PacketEnforcer


SAFE_PACKET = "0001000000060106000103E8"
DANGEROUS_PACKET = "000100000006010600010FA0"


class TestPacketEnforcer(unittest.TestCase):

    def setUp(self):
        self.enforcer = PacketEnforcer()

    def test_allowed_packet_is_forwarded(self):
        result = self.enforcer.enforce(
            packet=SAFE_PACKET,
            decision="ALLOW",
            reason="Safe physical state",
        )

        self.assertEqual(
            result.decision,
            "ALLOW",
        )

        self.assertEqual(
            result.action,
            "FORWARD",
        )

        self.assertTrue(
            result.forwarded,
        )

        self.assertIn(
            SAFE_PACKET,
            self.enforcer.forwarded_packets,
        )

        self.assertEqual(
            len(self.enforcer.dropped_packets),
            0,
        )

    def test_blocked_packet_is_dropped(self):
        result = self.enforcer.enforce(
            packet=DANGEROUS_PACKET,
            decision="BLOCK",
            reason="Pressure exceeds safe limit",
        )

        self.assertEqual(
            result.decision,
            "BLOCK",
        )

        self.assertEqual(
            result.action,
            "DROP",
        )

        self.assertFalse(
            result.forwarded,
        )

        self.assertIn(
            DANGEROUS_PACKET,
            self.enforcer.dropped_packets,
        )

        self.assertEqual(
            len(self.enforcer.forwarded_packets),
            0,
        )

    def test_unknown_decision_fails_closed(self):
        result = self.enforcer.enforce(
            packet=DANGEROUS_PACKET,
            decision="UNKNOWN",
        )

        self.assertEqual(
            result.decision,
            "BLOCK",
        )

        self.assertEqual(
            result.action,
            "DROP",
        )

        self.assertFalse(
            result.forwarded,
        )

        self.assertIn(
            DANGEROUS_PACKET,
            self.enforcer.dropped_packets,
        )

        self.assertIn(
            "fail-closed",
            result.reason,
        )

    def test_enforcement_statistics(self):
        self.enforcer.enforce(
            packet=SAFE_PACKET,
            decision="ALLOW",
        )

        self.enforcer.enforce(
            packet=DANGEROUS_PACKET,
            decision="BLOCK",
        )

        statistics = (
            self.enforcer.get_statistics()
        )

        self.assertEqual(
            statistics["forwarded_packets"],
            1,
        )

        self.assertEqual(
            statistics["dropped_packets"],
            1,
        )

        self.assertEqual(
            statistics["total_packets"],
            2,
        )


if __name__ == "__main__":
    unittest.main()