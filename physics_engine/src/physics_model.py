"""
Physics Engine - Physics Model

Defines the basic physical system parameters.
"""


class PhysicsModel:
    def __init__(self, pressure=0.0, flow=0.0, rpm=0):
        self.pressure = pressure
        self.flow = flow
        self.rpm = rpm

    def get_parameters(self):
        return {
            "pressure": self.pressure,
            "flow": self.flow,
            "rpm": self.rpm
        }