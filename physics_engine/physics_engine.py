from physics_engine.plant_model import IndustrialPlant


class PhysicsEngine:
    """
    Physics-aware industrial command evaluator.

    The engine produces two physical states:

    1. Predicted state:
       The expected plant state if the requested command
       is applied immediately.

    2. Simulated actual state:
       A more realistic plant response using a response factor,
       representing the fact that physical equipment does not
       necessarily reach the requested value instantaneously.

    Security decisions are made using the predicted state.
    """

    def __init__(self):
        self.plant = IndustrialPlant()

        # Separate plant model for simulated actual response.
        self.actual_plant = IndustrialPlant()

        # Represents the response of real equipment.
        # 1.0 = instantaneous response
        # Lower values = slower physical response.
        self.response_factor = 0.80

    def evaluate_command(self, pump_rpm, duration=60):
        """
        Evaluate an industrial pump command.

        The requested RPM is first simulated directly to obtain
        the predicted physical state.

        A separate response model then estimates the simulated
        actual state after accounting for physical response delay.

        The security decision is based on the predicted state.
        """

        pump_rpm = float(pump_rpm)
        duration = float(duration)

        # --------------------------------------------------
        # Predicted physical state
        # --------------------------------------------------

        predicted_state = self.plant.simulate(
            pump_rpm=pump_rpm
        )

        # --------------------------------------------------
        # Simulated actual physical response
        # --------------------------------------------------

        previous_rpm = self.actual_plant.pump_rpm

        actual_rpm = (
            previous_rpm
            + self.response_factor
            * (pump_rpm - previous_rpm)
        )

        actual_state = self.actual_plant.simulate(
            pump_rpm=actual_rpm
        )

        # --------------------------------------------------
        # Security decision
        # --------------------------------------------------

        safe = predicted_state["safe"]

        decision = "ALLOW" if safe else "BLOCK"

        return {
            "command": {
                "pump_rpm": pump_rpm,
                "duration": duration,
            },

            "predicted_state": predicted_state,

            "actual_state": actual_state,

            "safe": safe,

            "decision": decision,
        }


if __name__ == "__main__":
    engine = PhysicsEngine()

    print("=== VoltGuard Physics Engine ===")

    # --------------------------------------------------
    # Safe command
    # --------------------------------------------------

    safe_result = engine.evaluate_command(
        pump_rpm=1000,
        duration=60,
    )

    print("\nSafe command:")
    print(safe_result)

    # --------------------------------------------------
    # Dangerous command
    # --------------------------------------------------

    dangerous_result = engine.evaluate_command(
        pump_rpm=4000,
        duration=60,
    )

    print("\nDangerous command:")
    print(dangerous_result)