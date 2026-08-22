"""
Physics Engine - Physics Model

Defines the basic physical system parameters
and predicts the next physical state.
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

    def predict_state(self):
        """
        Predict a simple next physical state.

        This is a baseline model for initial testing.
        """

        predicted_pressure = self.pressure + (self.flow * 0.01)
        predicted_rpm = self.rpm

        return {
            "pressure": round(predicted_pressure, 2),
            "flow": self.flow,
            "rpm": predicted_rpm
        }