"""
Physics Engine - Simulator

Provides a basic interface for running the physics simulation.
"""

from .physics_model import PhysicsModel


NORMAL = "NORMAL"
WARNING = "WARNING"
CATASTROPHIC_FAILURE = "CATASTROPHIC_FAILURE"


class Simulator:
    def __init__(self):
        self.model = PhysicsModel()
        self.state = NORMAL

    def get_state(self):
        return self.state

    def run(self):
        return {
            "state": self.state,
            "parameters": self.model.get_parameters()
        }