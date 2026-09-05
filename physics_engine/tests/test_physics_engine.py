import unittest

from physics_engine.plant_model import IndustrialPlant
from physics_engine.physics_engine import PhysicsEngine


class TestIndustrialPlant(unittest.TestCase):

    def test_safe_operating_state(self):
        plant = IndustrialPlant()

        state = plant.simulate(
            pump_rpm=1000,
            valve_opening=60
        )

        self.assertTrue(state["safe"])
        self.assertEqual(state["pump_rpm"], 1000.0)
        self.assertEqual(state["valve_opening"], 60.0)

    def test_dangerous_operating_state(self):
        plant = IndustrialPlant()

        state = plant.simulate(
            pump_rpm=4000,
            valve_opening=20
        )

        self.assertFalse(state["safe"])
        self.assertGreater(state["pressure"], 100.0)


class TestPhysicsEngine(unittest.TestCase):

    def test_safe_command_is_allowed(self):
        engine = PhysicsEngine()

        result = engine.evaluate_command(
            pump_rpm=1000,
            duration=60
        )

        self.assertEqual(result["decision"], "ALLOW")
        self.assertTrue(result["safe"])

    def test_dangerous_command_is_blocked(self):
        engine = PhysicsEngine()

        result = engine.evaluate_command(
            pump_rpm=4000,
            duration=60
        )

        self.assertEqual(result["decision"], "BLOCK")
        self.assertFalse(result["safe"])


if __name__ == "__main__":
    unittest.main()