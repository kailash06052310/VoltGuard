"""
Physics Engine - Interface

Provides a simple interface for receiving simulation inputs
and returning the simulated physical state.
"""

from .simulator import PhysicsSimulator


class PhysicsEngineInterface:

    def __init__(self):
        self.simulator = PhysicsSimulator()

    def process_command(self, command):
        """
        Receive a simulation command and return the physical state.
        """

        pressure = command.get("pressure", 0.0)
        flow = command.get("flow", 0.0)
        rpm = command.get("rpm", 0)

        return self.simulator.simulate(
            pressure=pressure,
            flow=flow,
            rpm=rpm
        )
