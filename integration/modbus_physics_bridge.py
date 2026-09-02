from physics_engine.physics_engine import PhysicsEngine


class ModbusPhysicsBridge:
    """
    Connects parsed Modbus commands with the VoltGuard
    physics engine.
    """

    def __init__(self):
        self.physics_engine = PhysicsEngine()

    def evaluate_modbus_command(self, command):
        """
        Evaluate a parsed Modbus command.

        Expected command format:
        {
            "pump_rpm": 1000,
            "duration": 60
        }
        """

        pump_rpm = command.get("pump_rpm", 0)
        duration = command.get("duration", 60)

        result = self.physics_engine.evaluate_command(
            pump_rpm,
            duration
        )

        return result


if __name__ == "__main__":
    bridge = ModbusPhysicsBridge()

    print("=== VoltGuard Modbus → Physics Bridge ===")

    normal_command = {
        "pump_rpm": 1000,
        "duration": 60
    }

    dangerous_command = {
        "pump_rpm": 4000,
        "duration": 60
    }

    print("\nNormal Modbus command:")
    print(bridge.evaluate_modbus_command(normal_command))

    print("\nDangerous Modbus command:")
    print(bridge.evaluate_modbus_command(dangerous_command))