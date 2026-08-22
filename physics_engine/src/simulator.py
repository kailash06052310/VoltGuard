"""
Physics Engine - Simulator

Runs the basic physical simulation and checks
whether the input values are within safe limits.
"""

from .physics_model import PhysicsModel
from .parameters import (
    PRESSURE_MIN,
    PRESSURE_MAX,
    FLOW_MIN,
    FLOW_MAX,
    RPM_MIN,
    RPM_MAX
)


class PhysicsSimulator:

    def __init__(self):
        self.model = PhysicsModel()

    def simulate(self, pressure, flow, rpm):

        self.model.pressure = pressure
        self.model.flow = flow
        self.model.rpm = rpm

        current_state = self.model.get_parameters()
        predicted_state = self.model.predict_state()

        status = self.check_limits(predicted_state)

        return {
            "current_state": current_state,
            "predicted_state": predicted_state,
            "status": status
        }

    def check_limits(self, state):

        if not (PRESSURE_MIN <= state["pressure"] <= PRESSURE_MAX):
            return "UNSAFE"

        if not (FLOW_MIN <= state["flow"] <= FLOW_MAX):
            return "UNSAFE"

        if not (RPM_MIN <= state["rpm"] <= RPM_MAX):
            return "UNSAFE"

        return "SAFE"