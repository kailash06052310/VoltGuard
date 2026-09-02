from physics_engine.plant_model import IndustrialPlant


class PhysicsEngine:
    """
    Evaluates an industrial command using the plant physics model.
    """

    def __init__(self):
        self.plant = IndustrialPlant()

    def evaluate_command(self, pump_rpm, duration=60):
        """
        Simulate the plant with the requested pump RPM
        and determine whether the resulting state is safe.
        """

        state = self.plant.simulate(pump_rpm, duration)

        return {
            "command": {
                "pump_rpm": pump_rpm,
                "duration": duration
            },
            "predicted_state": state,
            "safe": state["safe"],
            "decision": "ALLOW" if state["safe"] else "BLOCK"
        }


if __name__ == "__main__":
    engine = PhysicsEngine()

    print("=== VoltGuard Physics Engine ===")

    # Safe command
    safe_result = engine.evaluate_command(1000, 60)
    print("\nSafe command:")
    print(safe_result)

    # Dangerous command
    dangerous_result = engine.evaluate_command(4000, 60)
    print("\nDangerous command:")
    print(dangerous_result)